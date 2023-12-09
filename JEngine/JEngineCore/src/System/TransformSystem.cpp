#include "TransformSystem.h"
#include "Components.h"
#include "Util/Math.h"

void TransformSystem::RegisterSystem(flecs::world& _world)
{
	_world.system<Transform, Transform>("TransformSystem")
		// select 2nd transform argument
		.term_at(2).parent().cascade().optional().kind(flecs::PreUpdate)
		.iter([&](flecs::iter& iter, Transform* transform, Transform* p_transform)
			{
				UpdateTransform(iter, transform, p_transform);
			});
}

void TransformSystem::UpdateTransform(flecs::iter& iter, Transform* transform, Transform* p_transform)
{
	//imgui option to toggle vqs
	bool useVQS = false;
	for (auto i: iter) //iter is sorted index with breath first search parent->child
	{
		Transform& transformComp = transform[i];

		auto localVQS = VQS{
			transformComp.Position,
			transformComp.Rotation,
			Math::GetMaxElement(transformComp.Scale) //since vqs only support uniform scaling
		};
		//final transform matrix
		glm::mat4 final;

		//compute local
		if (useVQS)
		{
			transformComp.FinalVQS = localVQS;
			final = localVQS.toMat();
		}
		else
		{
			final = glm::translate(glm::mat4(1.0f), transformComp.Position)
				* glm::toMat4(transformComp.Rotation)
				* glm::scale(glm::mat4(1.0f), transformComp.Scale);

			transformComp.ParentTransformMatrix = glm::mat4{ 1.f };
			transformComp.CurrentTransformMatrix = final;
		}

		//compute global matrix if they have parents
		if (p_transform)
		{
			if (useVQS)
			{
				transformComp.FinalVQS = p_transform->FinalVQS * localVQS;
				final = transformComp.FinalVQS.toMat();
			}
			else
			{
				transformComp.CurrentTransformMatrix = final;
				transformComp.ParentTransformMatrix = p_transform->FinalTransformMatrix;
				final = p_transform->FinalTransformMatrix * final;
			}
		}
		transformComp.FinalTransformMatrix = final;
	}
}
