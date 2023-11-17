#include "CurveSystem.h"

#include "Components.h"

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
}

void CurveSystem::Update(flecs::iter& iter, PathComponent* path)
{
	for (auto i: iter)
	{
		auto& pathComp = path[i];

		/*for (auto& curve: pathComp.Curves)
		{
			float coi_t = pathComp.t + 0.01f;
			glm::vec3 coi = (coi_t >= 1.f) ? curve.GetEndTangent() : curve.Compute(coi_t);


			auto owner = iter.entity(i);
		}*/
		auto owner = iter.entity(i);

		owner.get_mut<Transform>()->Position = pathComp.Curves[0].Compute(pathComp.t);

		glm::vec3 coi = pathComp.Curves[0].Compute(pathComp.t + 0.001f);
		glm::vec3 w = glm::normalize(coi - owner.get_mut<Transform>()->Position);
		glm::vec3 u = glm::normalize(glm::cross(glm::vec3{ 0.f,1.f,0.f }, w));
		glm::vec3 v = glm::normalize(glm::cross(w, u));

		owner.get_mut<Transform>()->Rotation = glm::toQuat(glm::mat3{ u, v, w });

		//update t
		pathComp.t += iter.delta_time();
		//todo: change after testing
		const float to_clamp = 1.f;
		if (pathComp.t >= to_clamp)
		{
			pathComp.t -= to_clamp;
		}
	}
}
