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
			for (auto i: iter)
			{
				flecs::entity entity = iter.entity(i);
				UpdateFinalBoneMatrices(entity, animator[i]);
			}
		});
}

void AnimationSystem::UpdateAnimation(flecs::iter& iter, AnimatorComponent* animator)
{
	for (auto i:iter)
	{
		AnimatorComponent& animatorComp = animator[i];
		if(!animatorComp.Play)
			continue;
		if(animatorComp.CurrentAnimation)
		{
			float ticksPerSec = static_cast<float>(animatorComp.CurrentAnimation->TicksPerSecond);
			if(animatorComp.NumOfCyclePerSec!=0.f)
			{
				ticksPerSec = animatorComp.CurrentAnimation->Duration * animatorComp.NumOfCyclePerSec;
			}
			animatorComp.CurrentTime += ticksPerSec * iter.delta_time();
			animatorComp.CurrentTime = fmod(animatorComp.CurrentTime, animatorComp.CurrentAnimation->Duration);
			flecs::entity entity = iter.entity(i);
			UpdateTransforms(entity, animator[i], animatorComp.CurrentTime);
		}
	}
}

void AnimationSystem::UpdateAnimation(flecs::entity owner, AnimatorComponent* animator, float dt)
{
	AnimatorComponent& animatorComp = *animator;
	if (animatorComp.CurrentAnimation)
	{
		float ticksPerSec = static_cast<float>(animatorComp.CurrentAnimation->TicksPerSecond);
		if (animatorComp.NumOfCyclePerSec != 0.f)
		{
			ticksPerSec = animatorComp.CurrentAnimation->Duration * animatorComp.NumOfCyclePerSec;
		}
		animatorComp.CurrentTime += ticksPerSec * dt;
		animatorComp.CurrentTime = fmod(animatorComp.CurrentTime, animatorComp.CurrentAnimation->Duration);
		flecs::entity entity = owner;
		UpdateTransforms(entity, *animator, animatorComp.CurrentTime);
	}
}

void AnimationSystem::UpdateTransforms(flecs::entity entity, AnimatorComponent& animator, float currentTime)
{
	auto& channels = animator.CurrentAnimation->ChannelsMap;

	if (auto iter = channels.find(entity.name().c_str()); iter != channels.end())
	{
		auto& channel = iter->second;
		//Get index of keyframes based on current animation Time
		int posIndex = channel.GetPositionIndex(currentTime);
		int rotIndex = channel.GetRotationIndex(currentTime);
		int scaleIndex = channel.GetScaleIndex(currentTime);
		Transform* toUpdate = entity.get_mut<Transform>();

		//Interpolation Lerp, Slerp, Elerp

		//Get Interpolation Factor
		float posfactor=
		Math::GetInterpolationFactor(channel.Positions[posIndex].timeStamp, channel.Positions[posIndex + 1].timeStamp, currentTime);

		float rotfactor =
			Math::GetInterpolationFactor(channel.Rotations[rotIndex].timeStamp, channel.Rotations[rotIndex + 1].timeStamp, currentTime);

		float scalefactor =
			Math::GetInterpolationFactor(channel.Scales[scaleIndex].timeStamp, channel.Scales[scaleIndex + 1].timeStamp, currentTime);

		//Compute Interpolation when the time is located between [k, k+1] 
		glm::vec3 finalPos = Math::Lerp(channel.Positions[posIndex].position, channel.Positions[posIndex+1].position
			, posfactor);

		glm::quat finalRot = Math::Slerp(channel.Rotations[rotIndex].Rotation, channel.Rotations[rotIndex + 1].Rotation
			, rotfactor);

		glm::vec3 finalScale = Math::Elerp(channel.Scales[scaleIndex].scale, channel.Scales[scaleIndex + 1].scale
			, scalefactor);

		toUpdate->Position = finalPos;
		toUpdate->Rotation = finalRot;
		toUpdate->Scale = finalScale;
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
		animator.FinalBoneMatrices[bone_component->BoneMatrixID] = entity.get<Transform>()->FinalTransformMatrix * bone_component->BoneOffset;
	}
	entity.children([&](flecs::entity child)
	{
		UpdateFinalBoneMatrices(child, animator);
	});
}
