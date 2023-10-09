#include "SceneCamera.h"

#include "Application.h"
#include "Input.h"

void SceneCamera::RegisterSystem(flecs::world& _world)
{
	_world.system<Transform, MainCamera, Camera>("CameraUpdate").kind(flecs::PreUpdate).iter([&]
	(flecs::iter& iter, Transform* transform, MainCamera*, Camera*)
	{
		UpdateCamera(iter, transform);
	});
}

void SceneCamera::UpdateCamera(flecs::iter& iter, Transform* transform)
{
	constexpr float MouseSensitivity =5.f;
	constexpr float speed = 5000.f;
	for (int i: iter)
	{
		auto& camTransform = transform[i];
		glm::vec3 movement{ 0,0,0 };

		if (Input::IsPressed(KeyCode::W))
			movement += camTransform.GetForward();
		if (Input::IsPressed(KeyCode::S))
			movement -= camTransform.GetForward();

		if (Input::IsPressed(KeyCode::A))
			movement -= camTransform.GetRight();
		if (Input::IsPressed(KeyCode::D))
			movement += camTransform.GetRight();
		if (Input::IsPressed(KeyCode::Space))
		{
			movement += camTransform.GetUp();
		}
		if (Input::IsPressed(KeyCode::LeftControl))
		{
			movement -= camTransform.GetUp();
		}
		const float len = static_cast<float>(movement.length());
		if (len != 0.f)
		{
			movement /= len;
		}

		if (Input::IsPressed(KeyCode::LeftShift))
		{
			movement *= 5.f;
		}

		camTransform.position += movement * speed * iter.delta_time();

		if (Input::IsPressed(KeyCode::LeftAlt) && Input::IsPressed(MouseCode::Left))
		{
			auto [x_offset, y_offset] = Input::GetMouseOffset();

			x_offset *= MouseSensitivity * iter.delta_time();
			y_offset *= MouseSensitivity * iter.delta_time();
			glm::vec3 rotation = glm::degrees(glm::eulerAngles(camTransform.rotation));
			float Pitch = rotation.x + y_offset;

			//Pitch = glm::clamp(Pitch, glm::radians(-89.0f), glm::radians(89.0f));

			rotation.x = Pitch;
			rotation.y -= x_offset;

			camTransform.rotation = glm::quat(glm::radians(rotation));
		}

		Application::Get().GetWorld().set<MainCamera>({ glm::lookAt(camTransform.position, camTransform.position + camTransform.GetForward(), { 0.f,1.f,0.f }) });
	}
}
