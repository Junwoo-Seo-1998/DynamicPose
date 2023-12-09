#include "PhysicsSystem.h"

#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

void PhysicsSystem::RegisterSystem(flecs::world& _world)
{
	_world.system<RigidBody>("Compute Physics")
		.kind(flecs::OnUpdate)
		.iter([&](flecs::iter& iter, RigidBody* body)
			{
				Update(iter, body);
			});

	_world.system<SpringJointComponent>("Compute Spring Damper system")
		.kind(flecs::OnUpdate)
		.iter([&](flecs::iter& iter, SpringJointComponent* spring)
			{
				UpdateSpringPhysics(iter, spring);
			});
}

void PhysicsSystem::Update(flecs::iter& iter, RigidBody* bodies)
{
	for (auto i: iter)
	{
		RigidBody& body = bodies[i];
		//skip since it's fixed point
		if (body.InverseMass == 0.f)
		{
			body.CurrentForceAccumulated = glm::vec3{ 0.f };
			body.CurrentTorqueAccumulated = glm::vec3{ 0.f };
			continue;
		}
		auto object = iter.entity(i);

		float halfdt = iter.delta_time() * 0.5f;
		//change of P
		//for rk2
		auto halfStepLinearMomentum = body.LinearMomentum + body.CurrentForceAccumulated * halfdt;
		body.LinearMomentum += body.CurrentForceAccumulated * iter.delta_time();

		//change of L
		//for rk2
		auto halfStepAngularMomentum = body.AngularMomentum + body.CurrentTorqueAccumulated * halfdt;
		body.AngularMomentum += body.CurrentTorqueAccumulated * iter.delta_time();

		//v
		//for rk2
		auto halfStepVelocity = halfStepLinearMomentum * body.InverseMass;
		auto prevHalfStepVelocity = body.PrevLinearMomentumHalfStep * body.InverseMass;
		//rk2 ver of body.Velocity = body.LinearMomentum * body.InverseMass 
		body.Velocity = prevHalfStepVelocity + halfStepVelocity;
		//for next frame rk2
		body.PrevLinearMomentumHalfStep = halfStepLinearMomentum;

		//w
		//for rk2
		auto halfStepAngularVelocity = body.InverseInertiaTensor * halfStepAngularMomentum;
		auto prevHalfStepAngularVelocity = body.InverseInertiaTensor * body.PrevAngularMomentumHalfStep;
		//rk2 ver body.AngularVelocity = body.InverseInertiaTensor * body.AngularMomentum;
		body.AngularVelocity = halfStepAngularVelocity + prevHalfStepAngularVelocity;
		//for next frame rk2
		body.PrevAngularMomentumHalfStep = halfStepAngularMomentum;

		//x(t)
		Transform& transform = *object.get_mut<Transform>();
		auto rk2Velocity = body.Velocity;
		transform.Position += rk2Velocity * iter.delta_time();

		//R(t)
		glm::mat3 rotation = glm::toMat4(transform.Rotation);
		glm::mat3 ChangeOfRotation = Math::GetCrossProductMatrix(body.AngularVelocity) * rotation;
		rotation += ChangeOfRotation * iter.delta_time();
		transform.Rotation = glm::normalize(glm::quat(rotation));
		rotation = glm::toMat4(transform.Rotation);

		//update InverseInertiaTensor
		body.InverseInertiaTensor = rotation * body.OriginalInverseInertiaTensor * glm::transpose(rotation);

		//clear
		body.CurrentForceAccumulated = glm::vec3{ 0.f };
		body.CurrentTorqueAccumulated = glm::vec3{ 0.f };
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
			//current object
			{
				glm::vec3 globalCOM = entityTransform * glm::vec4(rigid_body.CenterOfMass, 1.f);
				glm::vec3 r = anchorGlobalPos - globalCOM;

				//glm::vec3 springForce = Math::ComputeSpringForce(c.springConstant, c.springLength, (targetGlobalPos - anchorGlobalPos));
				glm::vec3 springForce = c.springConstant * (targetGlobalPos - anchorGlobalPos);
				glm::vec3 gravity = c.MassOfAnchor * gravityDir;
				glm::vec3 tangentVelocity = glm::cross(rigid_body.AngularVelocity, r);
				glm::vec3 damping = -c.damping * (rigid_body.Velocity + tangentVelocity);
				glm::vec3 totalForce = springForce + gravity + damping;

				rigid_body.CurrentForceAccumulated += totalForce;
				rigid_body.CurrentTorqueAccumulated += glm::cross(r, totalForce);
			}

			//target object
			if(target.has<RigidBody>())
			{
				RigidBody& target_rigid_body = *target.get_mut<RigidBody>();
				glm::vec3 globalCOM = target.get<Transform>()->CurrentTransformMatrix * glm::vec4(target_rigid_body.CenterOfMass, 1.f);
				glm::vec3 r = targetGlobalPos - globalCOM;
				
				//glm::vec3 springForce = Math::ComputeSpringForce(c.springConstant, c.springLength, (anchorGlobalPos - targetGlobalPos));
				glm::vec3 springForce = c.springConstant * (anchorGlobalPos - targetGlobalPos);
				glm::vec3 gravity = c.TargetMassOfAnchor * gravityDir;
				glm::vec3 tangentVelocity = glm::cross(target_rigid_body.AngularVelocity, r);
				glm::vec3 damping = -c.damping * (target_rigid_body.Velocity + tangentVelocity);
				glm::vec3 totalForce = springForce + gravity + damping;

				target_rigid_body.CurrentForceAccumulated += totalForce;
				target_rigid_body.CurrentTorqueAccumulated += glm::cross(r, totalForce);
			}
		}
	}
}
