#pragma once
#include "../IRenderer.h"
#include <vulkan/vulkan.h>
#include "volk.h"
#include <GLFW/glfw3.h>

class VulkanRenderer : public IRenderer
{
public:
	bool Init(GLFWwindow* window) override;
	void DrawFrame() override;
	void Cleanup() override;

private:
	GLFWwindow* _window = nullptr;
	VkInstance _instance = VK_NULL_HANDLE;
	VkSurfaceKHR _surface = VK_NULL_HANDLE;
	VkDevice _device = VK_NULL_HANDLE;
	VkQueue _graphicsQueue = VK_NULL_HANDLE;
	VkQueue _presentQueue = VK_NULL_HANDLE;
	VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
	VkImageView _imageView = VK_NULL_HANDLE;
	VkRenderPass _renderPass = VK_NULL_HANDLE;
	VkPipeline _graphicsPipline = VK_NULL_HANDLE;

};

