#include "EntityUtil.h"

#include <iostream>

flecs::entity EntityUtil::FindChildWithName(const std::string& name, flecs::entity base)
{
	if (!base.is_valid() || name.empty())
		return {};

	flecs::entity foundEntity{};
	bool found = false;
	base.children([&](flecs::entity child)
	{
		FindChildWithNameHelper(name, child, found, foundEntity);
	});

	return foundEntity;
}

void EntityUtil::FindChildWithNameHelper(const std::string& name, flecs::entity current, bool& found, flecs::entity& foundEntity)
{
	if(found)
		return;
	if(name==current.name().c_str())
	{
		found = true;
		foundEntity = current;
		return;
	}
	current.children([&](flecs::entity child)
	{
			FindChildWithNameHelper(name, child, found, foundEntity);
	});
}
