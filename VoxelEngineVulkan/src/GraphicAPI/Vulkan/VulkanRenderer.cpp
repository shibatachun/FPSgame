#include "VulkanRenderer.h"

bool vulkan::VulkanRenderer::Init(GLFWwindow* window)
{
	_window = window;
	_instance.reset(new vulkan::Instance(_window));

}
