#pragma once
#include <flecs.h>
#include <string>

class EntityUtil
{
public:
	static flecs::entity FindChildWithName(const std::string& name, flecs::entity base);
private:
	static void FindChildWithNameHelper(const std::string& name, flecs::entity current, bool& found, flecs::entity& foundEntity);
};
