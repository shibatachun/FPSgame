#pragma once
#include "src/Core/GraphicAPI/IRenderer.h"
#include "src/ResourcesManager/AssetManager.h"
#include "VulkanGraphicResourceManager.h"



namespace vulkan
{
	struct PostProcedurePass {
		utils::vector<VkDescriptorSet> _sets;
		VkDescriptorSetLayout _setLayout;
		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;
	};

	
	struct SkyData{
		Buffer buffer;
		struct SkyUBOValue {
			glm::mat4 projInverse;
			glm::mat4 viewInverse;
			//glm::vec4 cameraPos;
			glm::vec4 sunDirection;
			glm::vec2 screenSize;
		} values;
	};

	struct GridData{
		Buffer buffer;
		struct GridUBOValue {
			glm::mat4 proj;
			glm::mat4 view;
			glm::mat4 projInverse;
			glm::mat4 viewInverse;
		} values;
	};

	struct PBRData {
		Buffer buffer;
		struct PBRUBOValue {
			float metallic = 0.0f;
			float roughness = 0.5f;
			float ao = 1.0f;
			float padding;   // ∂‘∆Î”√
		} values;
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
		std::vector<void*>										_skyDataBuffersMapped;
		std::vector<void*>										_gridDataBuffersMapped;
		std::vector<void*>										_pbrDataBuffersMapped;
		
		std::vector<ShaderData>									_uniformData;
		std::vector<SkyData>									_skyData;
		std::vector<GridData>									_gridData;
		std::vector<PBRData>									_pbrData;
		uint32_t												_currentFrame = 0;
		
		
		VkDescriptorSetLayout descriptorSetLayoutImage = VK_NULL_HANDLE;
		VkDescriptorSetLayout descriptorSetLayoutUbo = VK_NULL_HANDLE;
		VkMemoryPropertyFlags memoryPropertyFlags = 0;
		std::unique_ptr<VulkanGraphicResourceManager>			_GpuResouce;
		PostProcedurePass 										_debugPipeline;
		PostProcedurePass										_skyPass;
		glm::vec3												_sunDir;


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
		void UpdateSkyBuffer(uint32_t currentImage);
		void UpdateGridBuffer(uint32_t currentImage);
		void UpdatePBR(uint32_t currentImage);
		bool IsMinimized() const;
		void PrepareRenderObject();
		void BuildCommandBuffer();
		void CreateDebugPipeline();
		void CreateSkyPipeline();
	


	};
}

