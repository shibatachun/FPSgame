#pragma once
#include <GLFW/glfw3.h>
enum class API
{
	VULKAN,
	OPENGL,
	DIRECTX
};

class IRenderer {
public:
	virtual ~IRenderer() = default;
	virtual bool Init(GLFWwindow* window) = 0;
	virtual void DrawFrame() = 0;
	virtual void Cleanup() = 0;
};