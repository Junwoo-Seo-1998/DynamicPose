#include "CurveSystem.h"

#include <iostream>
#include <limits>
#include "Components.h"
#include "Graphics/AnimationSystem.h"
#include "Math/DistanceTime.h"
#include "Util/EntityUtil.h"

void CurveSystem::RegisterSystem(flecs::world& _world)
{
	_world.observer<PathComponent>().event(flecs::OnSet)
	.each([&](flecs::iter& _eventIter, size_t _entityID, PathComponent& _path)
	{
		OnChange(_path);
	});

	_world.system<PathComponent>("CurveSystem").kind(flecs::OnUpdate).iter([&](flecs::iter& iter, PathComponent* path)
	{
		Update(iter, path);
	});
}

void CurveSystem::OnChange(PathComponent& _path)
{
	_path.Curves.clear();

	//the insertion of Bezier control points algorithm 
	auto& controlPoints = _path.controlPoints;
	int numOfPoints = static_cast<int>(controlPoints.size());
	if (numOfPoints <= 1)
		return;
	else if (numOfPoints == 2)
	{
		_path.Curves.emplace_back(SpaceCurve{ controlPoints[0], controlPoints[1], {},{} });
	}
	else if (numOfPoints == 3)
	{
		_path.Curves.emplace_back(SpaceCurve{ controlPoints[0], controlPoints[1], controlPoints[2],{} });
	}
	else if (numOfPoints == 4)
	{
		_path.Curves.emplace_back(SpaceCurve{ controlPoints[0], controlPoints[1], controlPoints[2],controlPoints[3] });
	}

	if (numOfPoints > 4)
	{
		_path.Curves.reserve(numOfPoints - 1);

		//compute a and b for inserting
		std::vector<glm::vec3> a, b;
		{
			auto h = (controlPoints[1] - controlPoints[numOfPoints - 1]) / 4.f;
			a.push_back(controlPoints[0] + h);
			b.push_back(controlPoints[0] - h);
		}
		for (int i = 1; i < numOfPoints - 1; ++i)
		{
			auto h = (controlPoints[i + 1] - controlPoints[i - 1]) / 4.f;
			a.push_back(controlPoints[i] + h);
			b.push_back(controlPoints[i] - h);
		}
		{
			auto h = (controlPoints[0] - controlPoints[numOfPoints - 2]) / 4.f;
			a.push_back(controlPoints[numOfPoints - 1] + h);
			b.push_back(controlPoints[numOfPoints - 1] - h);
		}

		//generate each curve
		for (int i = 0; i < numOfPoints - 1; ++i)
		{
			_path.Curves.emplace_back(SpaceCurve{ controlPoints[i], a[i], b[i + 1], controlPoints[i + 1] });
		}
	}
	
	//merge table
	float prev = 0.f;

	_path.PreComputedPoints.clear();
	_path.UValues.clear();
	_path.CurveLength.clear();
	_path.InverseValues.clear();

	for (auto& curve:_path.Curves)
	{
		auto& points = curve.GetPreComputedPoints();
		auto& UValues = curve.GetUValues();
		int size = static_cast<int>(points.size());
		for (int i = 0; i < size; ++i)
		{
			_path.PreComputedPoints.push_back(points[i]);
			_path.UValues.push_back(prev + UValues[i]); //normalize
		}

		auto& arcLens = curve.GetCurveLength();
		auto& inverseValues = curve.GetInverseValues();
		size = static_cast<int>(arcLens.size());
		for (int i=0; i<size; ++i)
		{
			_path.CurveLength.push_back((prev + arcLens[i])); //normalize 
			_path.InverseValues.push_back((prev + inverseValues[i])); //normalize
		}
		prev += 1.f;
	}

	//normalize
	float endUV = _path.UValues[_path.UValues.size() - 1];
	for (auto& u : _path.UValues)
		u /= endUV;

	float endCurveLength = _path.CurveLength[_path.CurveLength.size() - 1];
	for (auto& l : _path.CurveLength)
		l /= endCurveLength;

	float endInverseValue = _path.InverseValues[_path.InverseValues.size() - 1];
	for (auto& u : _path.InverseValues)
		u /= endInverseValue;
}

void CurveSystem::Update(flecs::iter& iter, PathComponent* path)
{
	static Parabolic distance_time{0.1f, 0.9f };
	for (auto i: iter)
	{
		auto& pathComp = path[i];
		auto owner = iter.entity(i);
		if(static_cast<int>(pathComp.Curves.size()) == 0)
			continue;
		//sync speed with animation
		if(owner.has<AnimatorComponent>())
		{
			AnimatorComponent* animator=owner.get_mut<AnimatorComponent>();
			//compute NumOfCyclePerSec
			float P = 1.f;
			float NumOfCyclePerSec = distance_time.GetSpeed(pathComp.t) / P;
			animator->NumOfCyclePerSec = NumOfCyclePerSec;
		}

		//get arc length from distance time function
		float arcLen = distance_time.GetDistance(pathComp.t);
		//get U value to find position (for inverse function)
		float U = pathComp.GetInverse(arcLen);
		//get position
		owner.get_mut<Transform>()->Position = pathComp.GetPoint(U);

		//center of interest part
		glm::vec3 coi;
		float small = std::numeric_limits<float>::epsilon() * 100.f;
		//to avoid getting coi at the end of the curve
		if ((U + small) > 1.f)
			coi = owner.get_mut<Transform>()->Position + pathComp.Curves[pathComp.Curves.size() - 1].GetTangent(1.f);
		else
			coi = pathComp.GetPoint(U + small);

		glm::vec3 w = glm::normalize(coi - owner.get_mut<Transform>()->Position);
		glm::vec3 u = glm::normalize(glm::cross(glm::vec3{ 0.f,1.f,0.f }, w));
		glm::vec3 v = glm::normalize(glm::cross(w, u));

		//glm::mat3{ u, v, w } final coi matrix
		owner.get_mut<Transform>()->Rotation = glm::toQuat(glm::mat3{ u, v, w });

		if (owner.has<IKComponent>())
		{
			auto found = iter.world().lookup("Goal");
			if (found.is_valid())
			{
				auto rootPos = owner.get<Transform>()->Position;
				auto pos = found.get<Transform>()->Position;
				pos.y = 0.f;
				auto distance = glm::length(rootPos - pos);
				if (distance<0.6f)
				{
					if (owner.has<AnimatorComponent>())
					{
						AnimatorComponent* animator = owner.get_mut<AnimatorComponent>();
						if (animator->CurrentAnimation != iter.world().get<Config>()->AnimationList[5])
						{
							animator->Play = false;
							animator->CurrentAnimation =
								iter.world().get<Config>()->AnimationList[5];
							AnimationSystem::UpdateAnimation(owner, animator, 0.0f);
						}
					}
					continue;
				}
				else
				{
					if (owner.has<AnimatorComponent>())
					{
						AnimatorComponent* animator = owner.get_mut<AnimatorComponent>();
						animator->Play = true;
						animator->CurrentAnimation =
							iter.world().get<Config>()->AnimationList[16];
					}
				}
			}
		}


		//update t and clamp
		pathComp.t += iter.delta_time()*(1.f/(static_cast<float>(pathComp.Curves.size()*2.f))); //1sec will take to traverse
		constexpr float to_clamp = 1.f;
		if (pathComp.t >= to_clamp)
		{
			pathComp.t -= to_clamp;
		}
	}
}
