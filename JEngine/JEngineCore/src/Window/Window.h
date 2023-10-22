#pragma once
#include <functional>
#include <tuple>
class Window
{
public:
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
	virtual void Run(std::function<void()> callback) = 0;
	virtual void* GetRawHandle() = 0;
	virtual std::tuple<int, int> GetWindowSize() = 0;
};