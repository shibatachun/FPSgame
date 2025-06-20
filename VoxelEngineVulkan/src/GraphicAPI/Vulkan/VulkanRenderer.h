#pragma once
#include "../IRenderer.h"
#include "VulkanClasses.h"

namespace vulkan
{


	class VulkanRenderer : public IRenderer
	{
	public:
		VULKAN_NON_COPIABLE(VulkanRenderer)
		VulkanRenderer(GLFWwindow* window, VkPresentModeKHR presentMode);
		bool Init() override;
		void DrawFrame() override;
		void Cleanup() override;

	private:
		const VkPresentModeKHR _presentMode;
		GLFWwindow* _window;
		std::unique_ptr<class vulkan::Instance> _instance;
		std::unique_ptr<class vulkan::Surface> _surface;
		std::unique_ptr<class vulkan::DebugUtilsMessenger> _debugMessenger;
		std::unique_ptr<class vulkan::Device> _devices;
		std::unique_ptr<class vulkan::SwapChain> _swaphcin;
		VkRenderPass _renderPass = VK_NULL_HANDLE;
		
		VkPipeline _graphicsPipline = VK_NULL_HANDLE;
	private:
		void SetPhysicalDevices();
		void SetSwapChain();
		bool isMinimized() const;


	};
}

