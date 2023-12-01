#include "CurveSystem.h"

#include <iostream>
#include <limits>
#include "Components.h"
#include "Math/DistanceTime.h"

void CurveSystem::RegisterSystem(flecs::world& _world)
{
	_world.observer<PathComponent>().event(flecs::OnSet)
	.each([&](flecs::iter& _eventIter, size_t _entityID, PathComponent& _path)
	{
		OnChange(_path);
	});

	_world.system<PathComponent>("CurveSystem").iter([&](flecs::iter& iter, PathComponent* path)
	{
		Update(iter, path);
	});
}

void CurveSystem::OnChange(PathComponent& _path)
{
	_path.Curves.clear();
	auto& controlPoints = _path.controlPoints;
	int numOfPoints = static_cast<int>(controlPoints.size());
	if (numOfPoints <= 1)
		return;
	else if (numOfPoints == 2)
	{
		_path.Curves.emplace_back(SpaceCurve{ controlPoints[0], controlPoints[1], {},{} });
		return;
	}
	else if (numOfPoints == 3)
	{
		_path.Curves.emplace_back(SpaceCurve{ controlPoints[0], controlPoints[1], controlPoints[2],{} });
		return;
	}

	_path.Curves.reserve(numOfPoints - 1);

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



	for (int i = 0; i < numOfPoints - 1; ++i)
	{
		_path.Curves.emplace_back(SpaceCurve{ controlPoints[i], a[i], b[i + 1], controlPoints[i + 1] });

	}

	
	//update table
	float prev = 0.f;

	_path.PreComputedPoints.clear();
	_path.CurveLength.clear();
	_path.InverseValues.clear();

	float numOfCurves = static_cast<float>(numOfPoints) - 1.f;
	for (auto& curve:_path.Curves)
	{
		auto& points = curve.GetPreComputedPoints();
		auto& arcLens = curve.GetCurveLength();
		auto& UValues = curve.GetInverseValues();
		int size = arcLens.size();
		for (int i=0; i<size; ++i)
		{
			_path.PreComputedPoints.push_back(points[i]);
			_path.CurveLength.push_back((prev + arcLens[i]) / numOfCurves);
			_path.InverseValues.push_back((prev + UValues[i]) / numOfCurves);
		}
		prev += 1.f;
	}

	
}

void CurveSystem::Update(flecs::iter& iter, PathComponent* path)
{
	static Parabolic distance_time{0.3f, 0.7f };
	for (auto i: iter)
	{
		auto& pathComp = path[i];
		auto owner = iter.entity(i);
		//sync speed with animation
		if(owner.has<AnimatorComponent>())
		{
			AnimatorComponent* animator=owner.get_mut<AnimatorComponent>();
			//compute NumOfCyclePerSec
			float P = 1.f;
			float NumOfCyclePerSec = distance_time.GetSpeed(pathComp.t) / P;
			animator->NumOfCyclePerSec = NumOfCyclePerSec;
		}

		float arcLen = distance_time.GetDistance(pathComp.t);
		float U = pathComp.GetInverse(arcLen);
		owner.get_mut<Transform>()->Position = pathComp.GetPoint(U);

		
		glm::vec3 coi;
		float small = std::numeric_limits<float>::epsilon() * 10.f;
		//to avoid getting coi at the end of the curve
		if ((U + small) > 1.f)
			coi = owner.get_mut<Transform>()->Position + pathComp.Curves[pathComp.Curves.size() - 1].GetTangent(1.f);
		else
			coi = pathComp.GetPoint(U + small);
		glm::vec3 w = glm::normalize(coi - owner.get_mut<Transform>()->Position);
		glm::vec3 u = glm::normalize(glm::cross(glm::vec3{ 0.f,1.f,0.f }, w));
		glm::vec3 v = glm::normalize(glm::cross(w, u));

		owner.get_mut<Transform>()->Rotation = glm::toQuat(glm::mat3{ u, v, w });

		//update t
		pathComp.t += iter.delta_time()*(1.f/8.f);
		constexpr float to_clamp = 1.f;
		if (pathComp.t >= to_clamp)
		{
			pathComp.t -= to_clamp;
		}
	}
}
