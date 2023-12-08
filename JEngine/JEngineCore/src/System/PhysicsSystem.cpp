#include "PhysicsSystem.h"

#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

void PhysicsSystem::RegisterSystem(flecs::world& _world)
{
	_world.system<SpringJointComponent>("Compute Spring Damper system")
		.kind(flecs::OnUpdate)
		.iter([&](flecs::iter& iter, SpringJointComponent* spring)
		{
			UpdateSpringPhysics(iter, spring);
		});

	_world.system<RigidBody>("Compute Physics")
		.kind(flecs::OnUpdate)
		.iter([&](flecs::iter& iter, RigidBody* body)
			{
				Update(iter, body);
			});
}

void PhysicsSystem::Update(flecs::iter& iter, RigidBody* bodies)
{

	for (auto i: iter)
	{
		RigidBody& body = bodies[i];
		//skip since it's fixed point
		if(body.InverseMass==0.f)
			continue;
		auto object = iter.entity(i);

		//change of P
		bodies->LinearMomentum += bodies->ForceAccumulated * iter.delta_time();
		//change of L
		bodies->AngularMomentum += bodies->TorqueAccumulated * iter.delta_time();

		//v
		body.Velocity = bodies->LinearMomentum * bodies->InverseMass;
		//w
		bodies->AngularVelocity = bodies->InverseInertiaTensor * bodies->AngularMomentum;

		//x(t)
		Transform& transform = *object.get_mut<Transform>();
		transform.Position += body.Velocity * iter.delta_time();

		//R(t)
		glm::mat3 rotation = glm::toMat4(transform.Rotation);
		glm::mat3 ChangeOfRotation = Math::GetCrossProductMatrix(bodies->AngularVelocity) * rotation;
		rotation += ChangeOfRotation * iter.delta_time();
		transform.Rotation = glm::normalize(glm::quat(rotation));
		rotation = glm::toMat4(transform.Rotation);

		/*//w,x,y, and z order
		glm::quat angVelQuat(0.f, body.AngularVelocity.x, body.AngularVelocity.y, body.AngularVelocity.z);
		transform.Rotation += (0.5f * angVelQuat * transform.Rotation) * iter.delta_time();
		transform.Rotation = glm::normalize(transform.Rotation);//to unit
		glm::mat3 rotation = glm::toMat4(transform.Rotation);*/

		//update InverseInertiaTensor
		bodies->InverseInertiaTensor = rotation * bodies->OriginalInverseInertiaTensor * glm::transpose(rotation);

		//clear
		bodies->ForceAccumulated = glm::vec3{ 0.f };
		bodies->TorqueAccumulated = glm::vec3{ 0.f };
	}
}

void PhysicsSystem::UpdateSpringPhysics(flecs::iter& iter, SpringJointComponent* springComponents)
{
	constexpr glm::vec3 gravityDir = { 0.f, -9.8f, 0.f };
	for(auto i:iter)
	{
		auto entity = iter.entity(i);
		RigidBody& rigid_body = *entity.get_mut<RigidBody>();
		const glm::mat4& entityTransform = entity.get<Transform>()->CurrentTransformMatrix;
		SpringJointComponent& springComp = springComponents[i];
		for (auto& c:springComp.Connections)
		{
			auto target = iter.world().entity(c.Target);
			glm::vec3 targetGlobalPos = target.get<Transform>()->CurrentTransformMatrix * glm::vec4(c.TargetAnchorPos, 1.f);
			glm::vec3 anchorGlobalPos = entityTransform * glm::vec4(c.AnchorPos, 1.f);
			glm::vec3 globalCOM = entityTransform * glm::vec4(rigid_body.CenterOfMass, 1.f);
			glm::vec3 r = anchorGlobalPos - globalCOM;
			//apply gravity

			glm::vec3 springForce = 10.f * (targetGlobalPos - anchorGlobalPos);
			glm::vec3 gravity = c.MassOfAnchor * gravityDir;
			glm::vec3 tangentVelocity = glm::cross(rigid_body.AngularVelocity, r);
			glm::vec3 damping = -c.damping * (rigid_body.Velocity + tangentVelocity);
			glm::vec3 totalForce = springForce + gravity + damping;

			
			rigid_body.ForceAccumulated += totalForce;
			rigid_body.TorqueAccumulated += glm::cross(r, totalForce);
			//std::cout<<glm::to_string(glm::cross(c.AnchorPos - rigid_body.CenterOfMass, totalForce))<<std::endl;
		}
	}
}
