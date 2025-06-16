#pragma once





#include "../IRenderer.h"
#include "volk.h"

#include "VulkanClasses.h"



namespace vulkan
{


	class VulkanRenderer : public IRenderer
	{
	public:
		VULKAN_NON_COPIABLE(VulkanRenderer)
		bool Init(GLFWwindow* window) override;
		void DrawFrame() override;
		void Cleanup() override;

	private:
		GLFWwindow* _window = nullptr;
		std::unique_ptr<class vulkan::Instance> _instance;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkQueue _graphicsQueue = VK_NULL_HANDLE;
		VkQueue _presentQueue = VK_NULL_HANDLE;
		VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
		VkImageView _imageView = VK_NULL_HANDLE;
		VkRenderPass _renderPass = VK_NULL_HANDLE;
		VkPipeline _graphicsPipline = VK_NULL_HANDLE;
	private:
		


	};
}

