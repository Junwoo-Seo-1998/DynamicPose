#include "TransformSystem.h"
#include "Components.h"
void TransformSystem::RegisterSystem(flecs::world& _world)
{
	_world.system<Transform, Transform>("TransformSystem")
		// select 2nd transform argument
		.term_at(2).parent().cascade().optional()
		.iter([&](flecs::iter& iter, Transform* transform, Transform* p_transform)
			{
				UpdateTransform(iter, transform, p_transform);
			});
}

void TransformSystem::UpdateTransform(flecs::iter& iter, Transform* transform, Transform* p_transform)
{
	for (int i: iter) //breath first
	{
		Transform& transformComp = transform[i];
		glm::mat4 final = glm::translate(glm::mat4(1.0f), transformComp.position)
			* glm::toMat4(glm::quat(transformComp.rotation))
			* glm::scale(glm::mat4(1.0f), transformComp.scale);
		if (p_transform)
		{
			final = p_transform->final * final;
		}
		transformComp.final = final;
	}
}
