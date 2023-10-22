#include "TransformSystem.h"
#include "Components.h"
#include "Util/Math.h"

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
	constexpr bool useVQS = true;
	for (int i: iter) //breath first
	{
		Transform& transformComp = transform[i];
		auto localVQS = VQS{
			transformComp.Position,
			transform->Rotation,
			Math::GetMaxElement(transformComp.Scale) //since vqs only support uniform scaling
		};

		glm::mat4 final;
		if (useVQS)
		{
			transformComp.FinalVQS = localVQS;
			final = localVQS.toMat();
		}
		else
		{
			final = glm::translate(glm::mat4(1.0f), transformComp.Position)
				* glm::toMat4(glm::quat(transformComp.Rotation))
				* glm::scale(glm::mat4(1.0f), transformComp.Scale);
		}

		if (p_transform)
		{
			if (useVQS)
			{
				transformComp.FinalVQS = p_transform->FinalVQS * localVQS;
				final = transformComp.FinalVQS.toMat();
			}
			else
			{
				final = p_transform->FinalTransformMatrix * final;
			}
		}
		transformComp.FinalTransformMatrix = final;
	}
}
