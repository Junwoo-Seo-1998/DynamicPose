#include "AnimationSystem.h"

#include <iostream>

#include "Components.h"
#include "flecs.h"
#include "Util/Math.h"

void AnimationSystem::RegisterSystem(flecs::world& _world)
{
	_world.system<AnimatorComponent>("UpdateAnimation").kind(flecs::PreUpdate).iter([&](flecs::iter& iter, AnimatorComponent* animator)
		{
			UpdateAnimation(iter, animator);
		});
	_world.system<AnimatorComponent>("UpdateAnimationMatrix").kind(flecs::OnUpdate).iter([&](flecs::iter& iter, AnimatorComponent* animator)
		{
			for (int i: iter)
			{
				flecs::entity entity = iter.entity(i);
				UpdateFinalBoneMatrices(entity, animator[i]);
			}
		});
}

void AnimationSystem::UpdateAnimation(flecs::iter& iter, AnimatorComponent* animator)
{
	for (int i:iter)
	{
		AnimatorComponent& animatorComp = animator[i];
		if(animatorComp.CurrentAnimation)
		{
			animatorComp.CurrentTime += animatorComp.CurrentAnimation->TicksPerSecond * iter.delta_time();
			animatorComp.CurrentTime = fmod(animatorComp.CurrentTime, animatorComp.CurrentAnimation->Duration);
			flecs::entity entity = iter.entity(i);
			UpdateTransforms(entity, animator[i], animatorComp.CurrentTime);
		}
	}
}

void AnimationSystem::UpdateTransforms(flecs::entity entity, AnimatorComponent& animator, float currentTime)
{
	auto& channels = animator.CurrentAnimation->ChannelsMap;

	if (auto iter = channels.find(entity.name().c_str()); iter != channels.end())
	{
		//std::cout << entity.name() << std::endl;
		auto& channel = iter->second;
		int posIndex = channel.GetPositionIndex(currentTime);
		int rotIndex = channel.GetRotationIndex(currentTime);
		int scaleIndex = channel.GetScaleIndex(currentTime);
		Transform* toUpdate = entity.get_mut<Transform>();

		//todo: interpol
		float posfactor=
		Math::GetInterpolationFactor(channel.Positions[posIndex].timeStamp, channel.Positions[posIndex + 1].timeStamp, currentTime);
		glm::vec3 finalPos = Math::Lerp(channel.Positions[posIndex].position, channel.Positions[posIndex+1].position
			, posfactor);
		toUpdate->position = finalPos;

		float rotfactor =
			Math::GetInterpolationFactor(channel.Rotations[rotIndex].timeStamp, channel.Rotations[rotIndex + 1].timeStamp, currentTime);
		glm::quat finalRot = Math::Slerp(channel.Rotations[rotIndex].Rotation, channel.Rotations[rotIndex + 1].Rotation
			, rotfactor);
		toUpdate->rotation = finalRot;
		float scalefactor =
			Math::GetInterpolationFactor(channel.Scales[scaleIndex].timeStamp, channel.Scales[scaleIndex + 1].timeStamp, currentTime);
		glm::vec3 finalScale = Math::Lerp(channel.Scales[scaleIndex].scale, channel.Scales[scaleIndex + 1].scale
			, scalefactor);
		toUpdate->scale = finalScale;
	}
	entity.children([&](flecs::entity child)
	{
		UpdateTransforms(child, animator, currentTime);
	});
}

void AnimationSystem::UpdateFinalBoneMatrices(flecs::entity entity, AnimatorComponent& animator)
{
	if(entity.has<BoneComponent>())
	{
		const BoneComponent* bone_component = entity.get<BoneComponent>();
		animator.FinalBoneMatrices[bone_component->BoneMatrixID] = entity.get<Transform>()->final * bone_component->BoneOffset;
	}
	entity.children([&](flecs::entity child)
	{
		UpdateFinalBoneMatrices(child, animator);
	});
}
