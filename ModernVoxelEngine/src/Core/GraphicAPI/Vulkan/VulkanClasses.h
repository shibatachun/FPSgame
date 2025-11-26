#pragma once

#include "VulkanInitalizer.h"
#include "VulkanResource.h"

namespace vulkan {


	struct GpuThreadFramePools {

		VkCommandPool                   vulkan_command_pool = nullptr;
		VkQueryPool                     vulkan_timestamp_query_pool = nullptr;
		VkQueryPool                     vulkan_pipeline_stats_query_pool = nullptr;
		//
		//TODO::debug tools
		//GpuTimeQueryTree* time_queries = nullptr;

	}; // struct GpuThreadFramePools
	struct GpuDescriptorPoolCreation {

		uint16_t                             samplers = 256;
		uint16_t                             combined_image_samplers = 256;
		uint16_t                             sampled_image = 256;
		uint16_t                             storage_image = 256;
		uint16_t                             uniform_texel_buffers = 256;
		uint16_t                             storage_texel_buffers = 256;
		uint16_t                             uniform_buffer = 256;
		uint16_t                             storage_buffer = 256;
		uint16_t                             uniform_buffer_dynamic = 256;
		uint16_t                             storage_buffer_dynamic = 256;
		uint16_t                             input_attachments = 256;

	}; // struct GpuDescriptorPoolCreation
	class Instance final {
	public:
		NON_COPIABLE(Instance);
		Instance(void* window);
		~Instance();
		void* getWindow() const;

		const std::vector<const char*>& GetValidationLayers() const { return validationLayers_; }
		const std::vector<VkPhysicalDevice> PhysicalDevices() const { return physicalDevices_; }

	private:
		void* _window;
		uint32_t _vulkanVersion = VK_API_VERSION_1_3;
		std::vector<const char*>					validationLayers_;
		VULKAN_HANDLE(VkInstance, instance_)
		std::vector<VkExtensionProperties>			extension_;
		std::vector<VkLayerProperties>				layers_;
		std::vector<VkPhysicalDevice>				physicalDevices_;
	



	private:
		void GetVulkanExtensions();
		void GetVulkanLayers();
		void GetVulkanPhysicalDevices();

		void CheckVulkanMinimumVersion(const uint32_t minVersion);
		void CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

		

	};

	class Surface final {
	public:
		NON_COPIABLE(Surface)
		explicit Surface(const Instance& instance);
		~Surface();
		const class Instance& getInstance() const {
			return  instance_;
		}
	private:
		const Instance& instance_;
		VULKAN_HANDLE(VkSurfaceKHR, surface_);
	};

	class DebugUtilsMessenger final {
	public:
		NON_COPIABLE(DebugUtilsMessenger)
		DebugUtilsMessenger(const Instance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold);
		~DebugUtilsMessenger();

		VkDebugUtilsMessageSeverityFlagBitsEXT Threshold() const { return threshold_; }
	private:
		const Instance& instance_;
		const VkDebugUtilsMessageSeverityFlagBitsEXT threshold_;

		VULKAN_HANDLE(VkDebugUtilsMessengerEXT, messenger_)
	};

	class DebugUtils final {
	public:

		NON_COPIABLE(DebugUtils)

			explicit DebugUtils(VkInstance instance);
		~DebugUtils() = default;


		void SetDevice(VkDevice device)
		{
			device_ = device;
		}

		void SetObjectName(const VkAccelerationStructureKHR& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR); }
		void SetObjectName(const VkBuffer& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_BUFFER); }
		void SetObjectName(const VkCommandBuffer& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_COMMAND_BUFFER); }
		void SetObjectName(const VkDescriptorSet& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET); }
		void SetObjectName(const VkDescriptorSetLayout& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT); }
		void SetObjectName(const VkDeviceMemory& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_DEVICE_MEMORY); }
		void SetObjectName(const VkFramebuffer& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_FRAMEBUFFER); }
		void SetObjectName(const VkImage& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_IMAGE); }
		void SetObjectName(const VkImageView& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_IMAGE_VIEW); }
		void SetObjectName(const VkPipeline& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_PIPELINE); }
		void SetObjectName(const VkQueue& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_QUEUE); }
		void SetObjectName(const VkRenderPass& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_RENDER_PASS); }
		void SetObjectName(const VkSemaphore& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_SEMAPHORE); }
		void SetObjectName(const VkShaderModule& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_SHADER_MODULE); }
		void SetObjectName(const VkSwapchainKHR& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_SWAPCHAIN_KHR); }

	private:

		template <typename T>
		void SetObjectName(const T& object, const char* name, VkObjectType type) const
		{
#ifndef NDEBUG
			VkDebugUtilsObjectNameInfoEXT info = {};
			info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			info.pNext = nullptr;
			info.objectHandle = reinterpret_cast<const uint64_t&>(object);
			info.objectType = type;
			info.pObjectName = name;

			Check(vkSetDebugUtilsObjectNameEXT_(device_, &info), "set object name");
#endif
		}

		const PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT_;

		VkDevice device_{};
	};

	class Device final {
	public:
		NON_COPIABLE(Device);
		Device(VkPhysicalDevice device,const Surface&, const std::vector<const char*>& requiredExtension, const void* nextDeviceFeatures);
		~Device();

		uint32_t GraphicsFamilyIndex() const { return _graphicsFamilyIndex; }
		uint32_t PresentFamilyIndex() const { return _presentFamilyIndex; }
		uint32_t TransferFamilyIndex() const { return _transferFamilyIndex; }
		VkPhysicalDevice PhysicalDevice() const { return _physicalDevice; }
		const Surface& VulkanSurface() const { return _surface; }
		VkQueue GraphicsQueue() const { return _graphicsQueue; }
		VkQueue PresentQueue() const { return _presentQueue; }
		VkQueue TransferQueue() const { return _transferQueue; }
		const DebugUtils& DebugUtils() const { return _debugUtils; }
		void WaitIdle() const;
		VkPhysicalDeviceMemoryProperties GetPhyDeviceMemProperty() const { return _memProperties; };
		VkPhysicalDeviceProperties2 GetPhyDeviceProperty() const { return _physicalDeviceProperties2; };

		void CommandBufferSubmit(QueueFamily queue_, uint32_t submit_count_, const VkSubmitInfo* submit_info_, VkFence fence);
		void CommandBufferSubmit2(QueueFamily queue_, uint32_t submit_count, const VkSubmitInfo2KHR* submit_info_, VkFence fence);
		bool isBindless = false;
		bool synchronization2_extension_present = false;

	
	private:
		VULKAN_HANDLE(VkDevice, _device)
		const VkPhysicalDevice							_physicalDevice;
		const Surface&									_surface;
		std::vector<VkExtensionProperties>				_availableExtensions;
		class DebugUtils								_debugUtils;
		uint32_t										_graphicsFamilyIndex{};
		uint32_t										_presentFamilyIndex{};
		uint32_t										_transferFamilyIndex{};

		VkQueue											_graphicsQueue{};
		VkQueue											_presentQueue{};
		VkQueue											_transferQueue{};

		VkPhysicalDeviceProperties2						_physicalDeviceProperties2;
		VkPhysicalDeviceDriverProperties				_physicalDeviceDriverProerties;
		VkPhysicalDeviceMemoryProperties				_memProperties;
		//Features
		VkPhysicalDeviceVulkan13Features vkFeatures13_ = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
		VkPhysicalDeviceVulkan12Features vkFeatures12_ = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
														  .pNext = &vkFeatures13_ };
		VkPhysicalDeviceVulkan11Features vkFeatures11_ = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
														  .pNext = &vkFeatures12_ };
		VkPhysicalDeviceFeatures2 vkFeatures10_ = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &vkFeatures11_ };

		//Extension Method
		PFN_vkCmdBeginRenderingKHR							vkCmdBeginRenderingKHR;
		PFN_vkCmdEndRenderingKHR							vkCmdEndRenderingKHR;
		PFN_vkQueueSubmit2KHR								vkQueueSubmit2KHR;
		
	private:
		void CheckRequiredExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions);
	};
	/////////////////////////////////////////////////CommandBuffer and Pool//////////////////////////////////////////////////////////////
	struct CommandBuffer {
		void Init(Device& device);
		void Shutdown();
		void Begin();
		void BeginSecondary(VulkanRenderPass* currentRenderPass, VulkanFramebuffer* currentFramebuffer );
		void End();
		void EndCurrentRenderPass();
	};

	class CommandPoolManager final {
	public:
		NON_COPIABLE(CommandPoolManager)
		CommandPoolManager(const Device& device);
		~CommandPoolManager();
		void CreateCommandPool(QueueFamily);
		VkCommandPool GetCommandPool(QueueFamily family) { return _commandPools[family]; };
		void FreeCommandBuffer(QueueFamily, uint32_t, const VkCommandBuffer&);
	private:
		void Init(uint32_t num_threads);
		std::unordered_map<QueueFamily, VkCommandPool>				_commandPools;
		const Device& _device;


	};

	class BufferManager final {

	public:
		BufferManager(const Device& deivce, CommandPoolManager& commandPools);
		~BufferManager();

		void createBuffer(VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryPropertyFlags,
			VkDeviceSize size,
			VkBuffer* buffer,
			VkDeviceMemory* memory,
			void* data = nullptr);
	
		void createImageBuffer(uint32_t width, uint32_t height,  uint32_t mipLevels,
			VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkImage* image,
			VkDeviceMemory* imageMemory);
		const VkDevice& GetVkDevice() { return device.Handle(); };
		void CreateIndexBuffer1(std::vector<uint32_t>& indiceData, VkBuffer& buffer, VkDeviceMemory& memory);
		void CreateVertexBuffer1(std::vector<Vertex>& vertexData, VkBuffer& buffer, VkDeviceMemory& memory);
		void CreateVulkanImageBuffer(const Image& image_data, VkImageLayout& image_layout, VkImage& image, VkDeviceMemory& memory);

		void transitionImageLayout(VkImage image, VkFormat format, uint32_t miplevel, VkImageLayout oldLayout, VkImageLayout newLayout, QueueFamily family);
		void setImageLayout(VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkImageSubresourceRange subresourceRange,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask);
		bool hasStencilComponent(VkFormat format);

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, QueueFamily family);
		void copyBufferToImage(VkBuffer buffer, VkImage image, std::vector<SubResource>,uint32_t width, uint32_t height, QueueFamily family);

		VkCommandBuffer createInstantCommandBuffer(VkCommandBufferLevel level, VkCommandPool pool, uint32_t bufferCount, bool begin = false);
		void flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool, bool free);

		uint32_t findMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr) const;
		void CreateTextureSampler(VkSampler& sampler, uint32_t mipLevels);

		void CreateImageView(VkImageView& view, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

		void DestroyBuffer(VkBuffer buffer, VkDeviceMemory memory);
		void DestroyVkImage(VkImage image, VkDeviceMemory memory);
		void DestroyVkImageView(VkImageView imageview);
		void DestroySampler(VkSampler sampler);

		////test functions;
		/*void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void copyBuffer1(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void transitionImageLayout1(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage1(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);*/
	private:
		const Device& device;
		CommandPoolManager& commandPools;


	};

	class SwapChain final {
	
	public:
		NON_COPIABLE(SwapChain)
			SwapChain(const Device& device, BufferManager& buffermanager,VkPresentModeKHR presentationMode);
		const VkExtent2D GetSwapchainExtent() const { return _extent; };
		const VkFormat GetSwaphchainImageFormat() const { return _colorFormat; };
		const VkFormat GetDepthStencilFormat() const { return _depthFormat; };
		std::vector<VkFramebuffer>& GetSwapChainBuffer() { return _swapChainFramebuffers; };
		std::vector<VkImageView>& GetImageViews() { return _imageViews; };
		const Device& GetDevice() const { return _device; };
		void CreateSwapChain(VkPresentModeKHR presentationMode);
		void CreateFrameBuffer(VkRenderPass renderPass);
		void CleanUpSwapChain();
		void CreateDepthResources();
		
		~SwapChain();
	private:
		struct SupportDetails
		{
			VkSurfaceCapabilitiesKHR Capabilities{};
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;
		};

		SupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes, VkPresentModeKHR presentMode);
		VkExtent2D ChooseSwapExtent(void* window, const VkSurfaceCapabilitiesKHR& capabilities);
		uint32_t ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities);
		
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		
	

	private:
		VULKAN_HANDLE(VkSwapchainKHR, _swapChain)
		const VkPhysicalDevice										_physicalDevice;
		const class Device&											_device;
		BufferManager&												_bufferManager;
		uint32_t													_minImageCount;
		VkPresentModeKHR											_presentMode;
		VkFormat													_colorFormat;
		VkFormat													_depthFormat;
		VkExtent2D													_extent;
		std::vector<VkImage>										_images;
		std::vector<VkImageView>									_imageViews;
		std::vector<VkFramebuffer>									_swapChainFramebuffers;

		VkImage														_depthImage;
		VkDeviceMemory												_depthImageMemory;
		VkImageView													_depthImageView;
	};

	class RenderPass final {
	public:

		NON_COPIABLE(RenderPass)
		RenderPass(const SwapChain& swapchain);
		~RenderPass();
		const VkRenderPass GetRenderPass() const { return _renderPass; }
		void Destroy();

	private:
		const SwapChain&											_swapChain;
		VkRenderPass												_renderPass;

	};

	class GraphicPipelineManager final {
	
	public:
		NON_COPIABLE(GraphicPipelineManager)
		GraphicPipelineManager(
			const std::unordered_map<std::string, asset::shader>& shaders, 
			VkDevice device,
			const  SwapChain& swapchain, 
			const RenderPass& renderpass);
		void Destroy(std::string pipelineName);
		VkPipeline GetGraphicsPipeline(std::string pipelineName);
		VkPipelineLayout GetGraphicsPipelineLayout(std::string piplineLayoutName);
		void CreateGraphicsPipeline(std::string pipelineName, VkPipelineLayout layout, const asset::shader& shaders, VkRenderPass renderPass);
		VkPipeline CreateGraphicsPipeline(std::string pipelineName, graphicsPipelineCreateInfoPack& pack, const asset::shader& shaders);
		VkPipeline DebugPipelineTest(const asset::shader& shaders, VkPipelineLayout layout);

		~GraphicPipelineManager();
	private:
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
		void createPipelineCache();
		
	private:
		
		std::unordered_map<std::string, PipelineEntry>				_pipelineEntrys;
		std::unordered_map<std::string, VkPipeline>					_pipelines;
		std::unordered_map<std::string, VkPipelineLayout>			_pipelineLayouts;

		VkPipelineCache												_pipelineCache;
		//TODO Remove this 
		const std::unordered_map<std::string, asset::shader>&		_shaders;
		VkDevice													_device;
		const SwapChain&											_swapChain;
		const RenderPass&											_renderPass;
	};

	class DescriptorLayoutManager final {
	public:
		NON_COPIABLE(DescriptorLayoutManager)
		DescriptorLayoutManager(const Device& deivce);
		VkDescriptorSetLayout GetDescriptorSetLayout(LayoutConfig config) { return _LayoutCache[config]; };
		~DescriptorLayoutManager();
		VkDescriptorSetLayout CreateDescriptorSetLayout(LayoutConfig config);
	private:
		std::unordered_map<LayoutConfig, VkDescriptorSetLayout, LayoutConfigHash>				_LayoutCache;
		const Device&																			_device;
	};

	class DescriptorPoolManager final {
	public:
		NON_COPIABLE(DescriptorPoolManager)
		DescriptorPoolManager(const Device& deivce);
		~DescriptorPoolManager();
		void CreateGlobalDescriptorPool();
		void CreateGlobalDescriptorPool(const GpuDescriptorPoolCreation& creation);
		void CreatePerFrameDescriptorPool();
		void CreatePreFrameDescriptorSets(std::vector<VkDescriptorSetLayout>& layouts);
		void CreatePoolForIndividualObject(uint32_t uboCount, uint32_t imageCount, std::string objectName);
		void AllocateDescriptorSet(VkDescriptorSetLayout& layout, VkDescriptorType type, VkDescriptorSet& desSet, uint32_t binding, std::vector<VkDescriptorBufferInfo>,int index);
		void AllocateImageDescriptorSet(Vulkan_Material& material, std::vector<Vulkan_Texture>& textures, VkDescriptorSetLayout layout);
		void PrepareNodeDescriptor(SceneNode* node, VkDescriptorSetLayout descriptorSetlayout);
		VkDescriptorPool GetIndividualDescriptorPool(std::string poolname)
		{
			return _test_pool[poolname];
		}
		std::vector<VkDescriptorSet> GetHardCodedDescriptorSet() { return _hardCodeDescriptorSet; };

	private:
		VkDescriptorPool														_GlobalPool;
		std::vector<VkDescriptorPool>											_PerFramePool;
		std::vector<VkDescriptorSet>											_hardCodeDescriptorSet;
		std::unordered_map<VkDescriptorSetLayout, std::vector<VkDescriptorSet>> _descriptorSetCache;
		std::unordered_map<std::string, VkDescriptorPool>						_test_pool;
		const Device&															_device;

	};
		
	class VulkanResouceManager final {


		public:
			VulkanResouceManager(
				BufferManager& bufferManager, 
				DescriptorPoolManager& descPoolManager, 
				DescriptorLayoutManager& descLayoutManager, 
				GraphicPipelineManager& graphicPipelineManager,
				Device& device,
				Instance& instance,
				const asset::AssetManager& assetManager);
			~VulkanResouceManager();
			void init();
			void ConvertToVulkanResource();
			void ConstructVulkanRenderObject(std::string name, std::string raw_model_name, std::vector<std::string> textureFiles);
			void ConstructVulkanRenderObject(std::string name, std::string raw_model_name);
			VulkanRenderObject& GetRenderObject(std::string name);
			VkPipelineShaderStageCreateInfo LoadShader(std::string shader_name, VkShaderStageFlagBits);
			std::vector<VulkanRenderObject>& GetRenderObjects() { return _renderObjects; };


		private:
			Device&																				_device;
			Instance&																			_instance;
			std::unordered_map<std::string, uint32_t>											_renderObjectsMapping;
			
			BufferManager&																		_BufferManager;
			DescriptorPoolManager&																_descriptorPoolManager;
			DescriptorLayoutManager&															_descriptorLayoutManager;
			GraphicPipelineManager&																_graphicPipelineManager;
			const asset::AssetManager&															_assetMnanger;

			std::vector<Vulkan_Mesh>															_mesh;
			std::vector<Vulkan_Material>														_material;
			std::vector<Vulkan_Texture>															_Texture;
			std::vector<VulkanRenderObject>														_renderObjects;

			//GPU Resources
			ResourcePool<VulkanBuffer>															_buffers;
			ResourcePool<VulkanTexture>															_textures;
			ResourcePool<VulkanPipeline>														_pipelines;
			ResourcePool<VulkanSampler>															_samplers;
			ResourcePool<VulkanDesciptorSetLayout>												_descriptor_set_layouts;
			ResourcePool<VulkanDesciptorSet>													_descriptor_sets;
			ResourcePool<VulkanRenderPass>														_render_passes;
			ResourcePool<VulkanBuffer>															_command_buffers;
			ResourcePool<VulkanShaderState>														_shaders;

			//RenderObject Resources

		
			ResourcePool<Program>          programs;
			ResourcePool<Material>         materials;


		
		private:
			
			TextureHandle CreateTextureResource (const TextureCreation& creation);
			PipelineHandle CreatePipeline(const PipelineCreation&);
			SamplerHandle CreateSamplerResource (const SamplerCreation& creation);

	
			void ConstructVulkanRenderObject();
			void prepareNodeDescriptor(Node* node, VkDescriptorSetLayout descriptorSetLayout);
			void CreateTextureImageView(VkImageView& imageview, VkImage image);
			void ConstructSceneNode(SceneNode* parent, Node* sourceNode, VulkanRenderObject& object, ModelData& modelData); 

	
	};
}
	



