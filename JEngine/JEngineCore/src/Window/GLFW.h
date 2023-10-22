#pragma once
#include "Window.h"
#include <memory>

struct GLFWwindow;

class GLFW : public  Window
{
public:
	bool Init() override;
	void Shutdown() override;
	void Run(std::function<void()> callback) override;

	void* GetRawHandle() override;
	std::tuple<int, int> GetWindowSize() override;

private:
	std::shared_ptr<GLFWwindow> m_Window;
};