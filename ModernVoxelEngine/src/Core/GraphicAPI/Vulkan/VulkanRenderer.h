#pragma once
#include "src/Core/GraphicAPI/IRenderer.h"
#include "src/ResourcesManager/AssetManager.h"
#include "VulkanGraphicResourceManager.h"



namespace vulkan
{
	struct DebugPipeline {
		VkDescriptorSet _set;
		VkDescriptorSetLayout _setLayout;
		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;
	};
	class VulkanRenderer : public IRenderer
	{
	public:
		NON_COPIABLE(VulkanRenderer)
		VulkanRenderer(void* window, PRESENTMODE presentmode,asset::AssetManager& assetManager);
		virtual bool Init() override final;
		virtual void DrawFrame() override final;
		virtual void Cleanup() override final;
		API getAPI() const override { return API::VULKAN; }
		
	public:
		bool													_framebufferResized = false;

	
	private:
		const VkPresentModeKHR									_presentMode;
		void*													_window;
		asset::AssetManager&									_assetManager;
		
		Camera													_camera;

		std::unique_ptr<class vulkan::Instance>					_instance;
		std::unique_ptr<class vulkan::Surface>					_surface;
		std::unique_ptr<class vulkan::DebugUtilsMessenger>		_debugMessenger;
		std::unique_ptr<class vulkan::Device>					_devices;
		std::unique_ptr<class vulkan::SwapChain>				_swapchain;
		std::unique_ptr<class vulkan::GraphicPipelineManager>	_graphicsPipline;
		std::unique_ptr<class vulkan::RenderPass>				_renderPass;
		std::unique_ptr<class vulkan::CommandPoolManager>		_commandPools;
		std::unique_ptr<class vulkan::DescriptorLayoutManager>	_descriptorLayouts;
		std::unique_ptr<class vulkan::DescriptorPoolManager>	_descriptorPools;
		std::unique_ptr<class vulkan::BufferManager>			_bufferManager;
		std::unique_ptr<class vulkan::VulkanResouceManager>		_resouceManager;

		

		

		std::vector<VkCommandBuffer>							_commandBuffers;
		std::vector<VkSemaphore>								_imageAvailableSemaphores;
		std::vector<VkSemaphore>								_renderFinishedSemaphores;
		std::vector<VkFence>									_inFlightFences;


		std::vector<VkBuffer>									_uniformBuffers;
		std::vector<VkDeviceMemory>								_uniformBuffersMemory;
		std::vector<void*>										_uniformBuffersMapped;
		
		std::vector<ShaderData>									_uniformData;
		uint32_t												_currentFrame = 0;
		
		
		VkDescriptorSetLayout descriptorSetLayoutImage = VK_NULL_HANDLE;
		VkDescriptorSetLayout descriptorSetLayoutUbo = VK_NULL_HANDLE;
		VkMemoryPropertyFlags memoryPropertyFlags = 0;
		std::unique_ptr<VulkanGraphicResourceManager>			_GpuResouce;
		DebugPipeline											_debugPipeline;

		std::vector<VulkanRenderObject>							_renderObjects;
		
	private:
		bool InitVulkan();
		void SetPhysicalDevices();
		void SetSwapChain();
		void SetUpGraphicPipelineManager();
		void SetUpDescriptorLayoutManager();
		void SetUpDescriptorPoolsManager();
		void SetUpCommandPools();
		void SetUpBufferManager();
		void SetUpVulkanResouceManager();



		void CreateFrameBuffer();
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VulkanRenderObject object);
		void CreateCommandBuffer(QueueFamily family);
		void CreateSyncObjects();
		void RecreateSwapChain();
		void CreateUniformBuffers();
		void ConfigureDescriptorSet(VulkanRenderObject& object);
		void ConfigurePipeline(VulkanRenderObject& object);
		void UpdateUniformBuffer(uint32_t currentImage);
		bool IsMinimized() const;
		void PrepareRenderObject();
		void BuildCommandBuffer();
		void CreateDebugPipeline();


	};
}

