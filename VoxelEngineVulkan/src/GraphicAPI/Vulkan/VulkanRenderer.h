#pragma once
#include "../IRenderer.h"
#include "VulkanClasses.h"

namespace vulkan
{


	class VulkanRenderer : public IRenderer
	{
	public:
		VULKAN_NON_COPIABLE(VulkanRenderer)
		VulkanRenderer(GLFWwindow* window);
		bool Init() override;
		void DrawFrame() override;
		void Cleanup() override;

	private:
		GLFWwindow* _window;
		std::unique_ptr<class vulkan::Instance> _instance;
		std::unique_ptr<class vulkan::Surface> _surface;
		std::unique_ptr<class vulkan::DebugUtilsMessenger> _debugMessenger;
		std::unique_ptr<class vulkan::Device> _devices;
		VkQueue _graphicsQueue = VK_NULL_HANDLE;
		VkQueue _presentQueue = VK_NULL_HANDLE;
		VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
		VkImageView _imageView = VK_NULL_HANDLE;
		VkRenderPass _renderPass = VK_NULL_HANDLE;
		VkPipeline _graphicsPipline = VK_NULL_HANDLE;
	private:
		void SetPhysicalDevices();


	};
}

