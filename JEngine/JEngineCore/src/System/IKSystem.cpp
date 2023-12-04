#include "IKSystem.h"

#include <iostream>
#include <numbers>

#include "Components.h"

void IKSystem::RegisterSystem(flecs::world& _world)
{
	_world.system<IKComponent>().kind(flecs::PostUpdate).iter([&](flecs::iter& iter, IKComponent* iks)
		{
			UpdateIK(iter, iks);
		});;
}

void IKSystem::UpdateIK(flecs::iter& iter, IKComponent* iks)
{
	for (auto i: iter)
	{
		IKComponent& IKComp = iks[i];
		auto IKee = iter.world().entity(IKComp.EndEffect);
		auto IKeeComp = IKee.get<IKEndEffectComponent>();
		auto goal = iter.world().entity(IKeeComp->targetID);
		//this is prioritized joint(representation of flexibility of the joints)
		auto & joints = IKComp.Joints;

		std::vector<glm::quat> finalRotations;
		finalRotations.resize(joints.size());
		//solve
		glm::vec3 pv{}, pc{}, pd{};
		do
		{
			pv = pc;
			for (int i = 0; i < static_cast<int>(joints.size()); ++i)
			{
				auto jointID = joints[i];
				auto joint = iter.world().entity(jointID);

				auto toRelative = glm::inverse(joint.get<Transform>()->FinalTransformMatrix);
				auto jk = joint.get<Transform>()->GetWorldOrigin();
				pc = IKee.get<Transform>()->GetWorldOrigin();
				pd = goal.get<Transform>()->GetWorldOrigin();

				auto vck = pc - jk;
				auto vdk =pd - jk;
				//just in case before normalize
				if (vck.length() == 0.f || vdk.length() == 0.f)
					continue;
				vck = glm::normalize(vck);
				vdk = glm::normalize(vdk);

				auto ak = glm::acos(glm::dot(vck, vdk));

				//only accept values in reasonable range
				if (0.f <= ak && ak <= std::numbers::pi_v<float>)
				{
					//to rotate vector we have to be careful since if the object is not uniform scale.
					//therefore do inverse transpose of the matrix like we compute normal matrix
					//and change the global vector to local vector so that we can rotate join with that vector.
					auto vk = glm::mat3(glm::transpose(glm::inverse(toRelative))) * glm::normalize(glm::cross(vck, vdk));
					auto desiredRotation = glm::rotate(joint.get<Transform>()->Rotation, ak, vk);

					//apply constrain
					{
						JointConstrain constrain = joint.get<IKJointComponent>()->constrain;
						//main constrain
						float a_min = glm::radians(constrain.x_min);
						float a_max = glm::radians(constrain.x_max);
						float b_min = glm::radians(constrain.y_min);
						float b_max = glm::radians(constrain.y_max);

						//optional
						float c_min = glm::radians(constrain.z_min);
						float c_max = glm::radians(constrain.z_max);

						auto contrainedAngle = glm::eulerAngles(desiredRotation);
						if (joint.get<IKJointComponent>()->useConstrainX)
							contrainedAngle.x = glm::clamp(contrainedAngle.x, a_min, a_max);
						if (joint.get<IKJointComponent>()->useConstrainY)
							contrainedAngle.y = glm::clamp(contrainedAngle.y, b_min, b_max);
						if (joint.get<IKJointComponent>()->useConstrainZ)
							contrainedAngle.z = glm::clamp(contrainedAngle.z, c_min, c_max);
						desiredRotation = glm::quat(contrainedAngle);
					}

					finalRotations[i] = desiredRotation;

					auto relativePC = pc - jk;

					relativePC = glm::rotate(desiredRotation, relativePC);
					pc = relativePC + jk;
				}

				if (glm::length(pc - pd) <= 0.001f)
					break;
			}

		} while (glm::length(pc - pv) >= 0.01f);

		for (int i=0; i<joints.size(); ++i)
		{
			auto jointID = joints[i];
			auto joint = iter.world().entity(jointID);
			//update angle with slerp
			joint.get_mut<Transform>()->Rotation
			= Math::Slerp(joint.get_mut<Transform>()->Rotation, finalRotations[i], 0.8f * iter.delta_time());
		}
	}
}
