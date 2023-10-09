#pragma once
#include <flecs.h>
#include <string>

// Scene relationships/tags
struct ActiveScene { }; // Represents the current scene
struct SceneRoot { }; // Parent for all entities unique to the scene

struct Scene
{
	using Pipeline = flecs::entity;
	Pipeline pipeline;
};
