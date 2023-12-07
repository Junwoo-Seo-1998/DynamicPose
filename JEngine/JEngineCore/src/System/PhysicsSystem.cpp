#include "PhysicsSystem.h"

#include <glm/gtx/matrix_decompose.hpp>

void PhysicsSystem::RegisterSystem(flecs::world& _world)
{
	_world.system<RigidBody>("Apply Gravity")
		.kind(flecs::OnUpdate)
		.iter([&](flecs::iter& iter, RigidBody* body)
			{
				Update(iter, body);
			});
}

void PhysicsSystem::Update(flecs::iter& iter, RigidBody* bodies)
{
	constexpr glm::vec3 gravity = { 0.f, -9.8f, 0.f };
	for (auto i: iter)
	{
		RigidBody& body = bodies[i];
		//skip since it's fixed point
		if(body.InverseMass==0.f)
			continue;
		auto object = iter.entity(i);

		//change of P
		//apply gravity
		bodies->ForceAccumulated += gravity;
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
		glm::mat3 rotation = glm::toMat3(transform.Rotation);
		rotation += Math::GetCrossProductMatrix(bodies->AngularVelocity) * rotation * iter.delta_time();
		transform.Rotation = glm::quat(rotation);

		//clear
		bodies->ForceAccumulated = glm::vec3{ 0.f };
		bodies->TorqueAccumulated = glm::vec3{ 0.f };
	}
}
