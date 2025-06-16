#include "VulkanRenderer.h"

bool vulkan::VulkanRenderer::Init()
{
	
	_instance.reset(new vulkan::Instance(_window));
	_surface.reset(new vulkan::Surface(*_instance));
	return true;

}

void vulkan::VulkanRenderer::DrawFrame()
{
}

void vulkan::VulkanRenderer::Cleanup()
{
}

vulkan::VulkanRenderer::VulkanRenderer(GLFWwindow* window) :_window(window)
{
	if (!Init())
	{
		std::cout << "Init vulkan Failed!" << std::endl;
	}
}
