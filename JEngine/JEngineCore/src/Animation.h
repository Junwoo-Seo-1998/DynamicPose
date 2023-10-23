#pragma once
#include <map>
#include <string>
#include <vector>
#include "GraphicData.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation
{
	glm::quat Rotation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

struct Channel
{
	std::string Name;
	std::vector<KeyPosition> Positions;
	std::vector<KeyRotation> Rotations;
	std::vector<KeyScale> Scales;

	int GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < Positions.size() - 1; ++index)
		{
			if (animationTime < Positions[index + 1].timeStamp)
				return index;
		}
		//shouldn't happen
		assert(0);
		return -1;
	}

	int GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < Rotations.size() - 1; ++index)
		{
			if (animationTime < Rotations[index + 1].timeStamp)
				return index;
		}
		//shouldn't happen
		assert(0);
		return -1;
	}

	int GetScaleIndex(float animationTime)
	{
		for (int index = 0; index < Scales.size() - 1; ++index)
		{
			if (animationTime < Scales[index + 1].timeStamp)
				return index;
		}
		//shouldn't happen
		assert(0);
		return -1;
	}
};
struct Animation
{
	std::string AnimationName;
	float Duration;
	int TicksPerSecond;
	std::vector<Channel> Channels;
	std::map<std::string, Channel> ChannelsMap;
};


