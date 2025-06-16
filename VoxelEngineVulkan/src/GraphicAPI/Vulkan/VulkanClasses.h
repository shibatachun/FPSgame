#pragma once
#include "VulkanCommon.h"
namespace vulkan
{
	class Instance
	{
	public:
		VULKAN_NON_COPIABLE(Instance);
		Instance(GLFWwindow* window);
		~Instance();

	private:
		uint32_t _vulkanVersion = VK_VERSION_1_3;
	private:
		void CheckVulkanMinimumVersion(const uint32_t minVersion);
	};
}


