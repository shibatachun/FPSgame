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
		GLFWwindow* getWindow() const;

		const std::vector<const char*>& GetValidationLayers() const { return validationLayers_; }

	private:
		GLFWwindow* _window;
		uint32_t _vulkanVersion = VK_API_VERSION_1_3;
		std::vector<const char*> validationLayers_;
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


	class DebugUtilsMessenger final
	{
	public:
		VULKAN_NON_COPIABLE(DebugUtilsMessenger)
		DebugUtilsMessenger(const Instance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold);
		~DebugUtilsMessenger();

		VkDebugUtilsMessageSeverityFlagBitsEXT Threshold() const { return threshold_; }
	private:
		const Instance& instance_;
		const VkDebugUtilsMessageSeverityFlagBitsEXT threshold_;

		VULKAN_HANDLE(VkDebugUtilsMessengerEXT, messenger_)
	};
}


