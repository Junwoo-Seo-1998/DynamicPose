#pragma once
#include "flecs.h"
#include "System/System.h"

struct AnimatorComponent;


class AnimationSystem :public System
{
public:
	void RegisterSystem(flecs::world& _world) override;

	void UpdateAnimation(flecs::iter& iter, AnimatorComponent* animator);
private:
	void UpdateTransforms(flecs::entity entity, AnimatorComponent& animator, float currentTime);
	void UpdateFinalBoneMatrices(flecs::entity entity, AnimatorComponent& animator);
};
