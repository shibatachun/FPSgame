#include "RenderFactory.h"




std::unique_ptr<IRenderer> CreateRenderer(API api, GLFWwindow* windows, VkPresentModeKHR presentMode)
{
	switch (api)
	{
	case API::VULKAN:
		return std::make_unique<vulkan::VulkanRenderer>(windows, presentMode);
	case API::OPENGL:
		break;
	case API::DIRECTX:
		
		break;
	default:
		break;
	}
	return std::unique_ptr<IRenderer>();
}
