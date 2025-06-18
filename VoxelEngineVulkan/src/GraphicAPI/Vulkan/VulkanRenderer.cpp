#include "VulkanRenderer.h"

bool vulkan::VulkanRenderer::Init()
{
	
	_instance.reset(new vulkan::Instance(_window));
	_surface.reset(new vulkan::Surface(*_instance));
	_debugMessenger.reset(new vulkan::DebugUtilsMessenger(*_instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT));
	SetPhysicalDevices();
	
	return true;

}

void vulkan::VulkanRenderer::DrawFrame()
{
}

void vulkan::VulkanRenderer::Cleanup()
{
}

void vulkan::VulkanRenderer::SetPhysicalDevices()
{
	if (_devices)
	{
		throw std::logic_error("physical devices has already been set");
		return;
	}
	std::vector<const char*> requeredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	VkPhysicalDeviceFeatures  deviceFeatures = {};

	auto isDeviceSuitable = [&](VkPhysicalDevice device)->bool {
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceFeatures features{};
		vkGetPhysicalDeviceProperties(device, &properties);
		vkGetPhysicalDeviceFeatures(device, &features);

		return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && features.geometryShader;
		};
	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
	
	for (VkPhysicalDevice device : _instance->PhysicalDevices())
	{
		if (isDeviceSuitable(device))
		{
			_physicalDevice = device;
			
		}
	}
	VkPhysicalDeviceProperties props{};
	vkGetPhysicalDeviceProperties(_physicalDevice, &props);
	std::cout << "Using physical device : " << props.deviceName << std::endl;
	_devices.reset(new vulkan::Device(_physicalDevice, *_surface, requeredExtensions, deviceFeatures, nullptr));
	SetSwapChain();


}

void vulkan::VulkanRenderer::SetSwapChain()
{
	while (isMinimized())
	{
		glfwWaitEvents();
	}

	_swaphcin.reset(new vulkan::SwapChain(*_devices, _presentMode));


}

bool vulkan::VulkanRenderer::isMinimized() const
{
	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);

	return width==0 && height==0;
}

vulkan::VulkanRenderer::VulkanRenderer(GLFWwindow* window, VkPresentModeKHR presentmode) :_window(window), _presentMode(presentmode)
{
	if (!Init())
	{
		std::cout << "Init vulkan Failed!" << std::endl;
	}
}
