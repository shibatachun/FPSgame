#include "VulkanRenderer.h"

bool vulkan::VulkanRenderer::Init()
{
	
	_instance.reset(new vulkan::Instance(_window));
	_surface.reset(new vulkan::Surface(*_instance));
	_debugMessenger.reset(new vulkan::DebugUtilsMessenger(*_instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT));
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
