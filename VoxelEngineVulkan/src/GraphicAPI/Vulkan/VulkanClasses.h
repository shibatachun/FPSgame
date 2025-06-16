#pragma once
#include "VulkanCommon.h"
namespace vulkan
{
	class Instance final
	{
	public:
		VULKAN_NON_COPIABLE(Instance);
		Instance(GLFWwindow* window);
		~Instance();

	private:
		GLFWwindow* _window;
		uint32_t _vulkanVersion = VK_API_VERSION_1_3;
		VULKAN_HANDLE(VkInstance, instance_)

	private:
		void CheckVulkanMinimumVersion(const uint32_t minVersion);
		void CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

	};
	class Surface final
	{
	public:
		VULKAN_NON_COPIABLE(Surface)
		explicit Surface(const Instance& instance);
		~Surface();
		const class Instance& getInstance() const {
			return  instance_;
		}
	private:
		const Instance& instance_;
		VULKAN_HANDLE(VkSurfaceKHR, surface_);
	};
}


