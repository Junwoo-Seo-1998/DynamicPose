#pragma once
#include "flecs.h"
#include "System/System.h"

struct AnimatorComponent;


class AnimationSystem :public System
{
public:
	void RegisterSystem(flecs::world& _world) override;

	void UpdateAnimation(flecs::iter& iter, AnimatorComponent* animator);
	static void UpdateAnimation(flecs::entity owner, AnimatorComponent* animator, float dt);
private:
	static void UpdateTransforms(flecs::entity entity, AnimatorComponent& animator, float currentTime);
	static void UpdateFinalBoneMatrices(flecs::entity entity, AnimatorComponent& animator);
};
