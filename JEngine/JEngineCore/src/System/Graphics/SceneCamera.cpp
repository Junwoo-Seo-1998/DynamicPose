#include "SceneCamera.h"

#include "Application.h"
#include "Input.h"
#include "Window/Window.h"

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
	constexpr float MouseSensitivity = 10.f;
	constexpr float speed = 50.f;
	for (auto i: iter)
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
			movement *= 100.f;
		}

		camTransform.Position += movement * speed * iter.delta_time();

		if (Input::IsPressed(KeyCode::LeftAlt) && Input::IsPressed(MouseCode::Left))
		{
			auto [x_offset, y_offset] = Input::GetMouseOffset();

			x_offset *= MouseSensitivity * iter.delta_time();
			y_offset *= MouseSensitivity * iter.delta_time();
			glm::vec3 rotation = glm::degrees(glm::eulerAngles(camTransform.Rotation));
			float Pitch = rotation.x + y_offset;

			Pitch = glm::clamp(Pitch, -89.0f, 89.0f);

			rotation.x = Pitch;
			rotation.y -= x_offset;

			camTransform.Rotation = glm::quat(glm::radians(rotation));
		}

		auto [width, height] = Application::Get().GetWindow()->GetWindowSize();
		if(width<=0 || height<=0)
			continue;
		Application::Get().GetWorld().set<MainCamera>({ camTransform.Position,
			glm::lookAt(camTransform.Position, camTransform.Position + camTransform.GetForward(), { 0.f,1.f,0.f }),
			glm::perspective(glm::radians(60.f), (float)width/height, 0.01f, 1000.f)
			});
	}
}
