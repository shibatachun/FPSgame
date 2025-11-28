#pragma once


#pragma comment(lib, "vulkan-1.lib")

#include <vulkan/vulkan.h>


#define NON_COPIABLE(ClassName) \
	ClassName(const ClassName&) = delete; \
	ClassName(ClassName&&) = delete; \
	ClassName& operator = (const ClassName&) = delete; \
	ClassName& operator = (ClassName&&) = delete;

#define VULKAN_HANDLE(VulkanHandleType, name) \
public: \
	VulkanHandleType Handle() const { return name; } \
private: \
	VulkanHandleType name{};\


#include <chrono>


#include "src/Core/GraphicAPI/RHIResource.h"
#include "src/Core/GraphicAPI/RHIGraphicResourceManager.h"
#include "src/Core/GraphicAPI/RHICommandBuffer.h"

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;

namespace vulkan {

	
	enum class VertexComponent { Position, Normal, UV, Color, Tangent, Joint0, Weight0 };

    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

	struct graphicsPipelineCreateInfoPack {
		VkGraphicsPipelineCreateInfo createInfo =
		{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		//Vertex Input
		VkPipelineVertexInputStateCreateInfo vertexInputStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		std::vector<VkVertexInputBindingDescription> vertexInputBindings;
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
		//Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		//Tessellation
		VkPipelineTessellationStateCreateInfo tessellationStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
		//Viewport
		VkPipelineViewportStateCreateInfo viewportStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		std::vector<VkViewport> viewports;
		std::vector<VkRect2D> scissors;
		uint32_t dynamicViewportCount = 1;//动态视口/剪裁不会用到上述的vector，因此动态视口和剪裁的个数向这俩变量手动指定
		uint32_t dynamicScissorCount = 1;
		//Rasterization
		VkPipelineRasterizationStateCreateInfo rasterizationStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		//Multisample
		VkPipelineMultisampleStateCreateInfo multisampleStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		//Depth & Stencil
		VkPipelineDepthStencilStateCreateInfo depthStencilStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		//Color Blend
		VkPipelineColorBlendStateCreateInfo colorBlendStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
		//Dynamic
		VkPipelineDynamicStateCreateInfo dynamicStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		std::vector<VkDynamicState> dynamicStates;
		//--------------------
		graphicsPipelineCreateInfoPack() {
			SetCreateInfos();
			//若非派生管线，createInfo.basePipelineIndex不得为0，设置为-1
			createInfo.basePipelineIndex = -1;
		}
		//移动构造器，所有指针都要重新赋值
		graphicsPipelineCreateInfoPack(const graphicsPipelineCreateInfoPack& other) noexcept {
			createInfo = other.createInfo;
			SetCreateInfos();

			vertexInputStateCi = other.vertexInputStateCi;
			inputAssemblyStateCi = other.inputAssemblyStateCi;
			tessellationStateCi = other.tessellationStateCi;
			viewportStateCi = other.viewportStateCi;
			rasterizationStateCi = other.rasterizationStateCi;
			multisampleStateCi = other.multisampleStateCi;
			depthStencilStateCi = other.depthStencilStateCi;
			colorBlendStateCi = other.colorBlendStateCi;
			dynamicStateCi = other.dynamicStateCi;

			shaderStages = other.shaderStages;
			vertexInputBindings = other.vertexInputBindings;
			vertexInputAttributes = other.vertexInputAttributes;
			viewports = other.viewports;
			scissors = other.scissors;
			colorBlendAttachmentStates = other.colorBlendAttachmentStates;
			dynamicStates = other.dynamicStates;
			UpdateAllArrayAddresses();
		}
		//Getter，这里我没用const修饰符
		operator const VkGraphicsPipelineCreateInfo& () { return createInfo; }
		//Non-const Function
		//该函数用于将各个vector中数据的地址赋值给各个创建信息中相应成员，并相应改变各个count
		void UpdateAllArrays() {
			createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
			vertexInputStateCi.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
			vertexInputStateCi.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
			viewportStateCi.viewportCount = uint32_t(viewports.size()) ? uint32_t(viewports.size()) : dynamicViewportCount;
			viewportStateCi.scissorCount = uint32_t(scissors.size()) ? uint32_t(scissors.size()) : dynamicScissorCount;
			colorBlendStateCi.attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
			dynamicStateCi.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			UpdateAllArrayAddresses();
		}
	private:
		//该函数用于将创建信息的地址赋值给basePipelineIndex中相应成员
		void SetCreateInfos() {
			createInfo.pVertexInputState = &vertexInputStateCi;
			createInfo.pInputAssemblyState = &inputAssemblyStateCi;
			createInfo.pTessellationState = &tessellationStateCi;
			createInfo.pViewportState = &viewportStateCi;
			createInfo.pRasterizationState = &rasterizationStateCi;
			createInfo.pMultisampleState = &multisampleStateCi;
			createInfo.pDepthStencilState = &depthStencilStateCi;
			createInfo.pColorBlendState = &colorBlendStateCi;
			createInfo.pDynamicState = &dynamicStateCi;
		}
		//该函数用于将各个vector中数据的地址赋值给各个创建信息中相应成员，但不改变各个count
		void UpdateAllArrayAddresses() {
			createInfo.pStages = shaderStages.data();
			vertexInputStateCi.pVertexBindingDescriptions = vertexInputBindings.data();
			vertexInputStateCi.pVertexAttributeDescriptions = vertexInputAttributes.data();
			viewportStateCi.pViewports = viewports.data();
			viewportStateCi.pScissors = scissors.data();
			colorBlendStateCi.pAttachments = colorBlendAttachmentStates.data();
			dynamicStateCi.pDynamicStates = dynamicStates.data();
		}
	};

	struct LayoutConfig {
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		uint32_t bindingCounts;
		// 如果还有 push constant ranges，也可以放在这里：
		// std::vector<VkPushConstantRange> pushConstants;

		// 比较运算符：先比大小，再逐一比较字段
		bool operator==(LayoutConfig const& o) const {
			if (bindings.size() != o.bindings.size()) return false;
			for (size_t i = 0; i < bindings.size(); i++) {
				const auto& a = bindings[i];
				const auto& b = o.bindings[i];
				if (a.binding != b.binding ||
					a.descriptorType != b.descriptorType ||
					a.descriptorCount != b.descriptorCount ||
					a.stageFlags != b.stageFlags ||
					a.pImmutableSamplers != b.pImmutableSamplers   // 通常 nullptr
					) return false;
			}
			// 同理比较 pushConstants…
			return true;
		}
		void UpdateAllArray() {
			bindingCounts = static_cast<uint32_t>((bindings.size()));
		}
	};

	inline void hashCombine(std::size_t& seed, std::size_t v) {
		seed ^= v + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
	}

	struct LayoutConfigHash {
		std::size_t operator()(LayoutConfig const& cfg) const noexcept {
			std::size_t h = 0;
			for (auto const& b : cfg.bindings) {
				// 把每个字段都混合进 seed
				hashCombine(h, std::hash<uint32_t>()(b.binding));
				hashCombine(h, std::hash<uint32_t>()(b.descriptorType));
				hashCombine(h, std::hash<uint32_t>()(b.descriptorCount));
				hashCombine(h, std::hash<uint32_t>()(b.stageFlags));
				// pImmutableSamplers 通常都是 nullptr 才会相等，否则需要根据地址或内容再 hash
				hashCombine(h, std::hash<uintptr_t>()(
					reinterpret_cast<uintptr_t>(b.pImmutableSamplers)));
			}
			// 如果有 pushConstants，再把它们也混合进 h
			// for (auto const &p : cfg.pushConstants) { … }
			return h;
		}
	};

	struct PipelineEntry
	{
		std::string pipeline;
		std::string layout;
	};

    enum class QueueFamily
    {
        GRAPHIC,
        COMPUTE,
        TRANSFER
    };

    inline const char* ToString(const VkResult result)
    {
        switch (result)
        {
#define STR(r) case VK_ ##r: return #r
            STR(SUCCESS);
            STR(NOT_READY);
            STR(TIMEOUT);
            STR(EVENT_SET);
            STR(EVENT_RESET);
            STR(INCOMPLETE);
            STR(ERROR_OUT_OF_HOST_MEMORY);
            STR(ERROR_OUT_OF_DEVICE_MEMORY);
            STR(ERROR_INITIALIZATION_FAILED);
            STR(ERROR_DEVICE_LOST);
            STR(ERROR_MEMORY_MAP_FAILED);
            STR(ERROR_LAYER_NOT_PRESENT);
            STR(ERROR_EXTENSION_NOT_PRESENT);
            STR(ERROR_FEATURE_NOT_PRESENT);
            STR(ERROR_INCOMPATIBLE_DRIVER);
            STR(ERROR_TOO_MANY_OBJECTS);
            STR(ERROR_FORMAT_NOT_SUPPORTED);
            STR(ERROR_FRAGMENTED_POOL);
            STR(ERROR_UNKNOWN);
            STR(ERROR_OUT_OF_POOL_MEMORY);
            STR(ERROR_INVALID_EXTERNAL_HANDLE);
            STR(ERROR_FRAGMENTATION);
            STR(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
            STR(ERROR_SURFACE_LOST_KHR);
            STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            STR(SUBOPTIMAL_KHR);
            STR(ERROR_OUT_OF_DATE_KHR);
            STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            STR(ERROR_VALIDATION_FAILED_EXT);
            STR(ERROR_INVALID_SHADER_NV);
            STR(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
            STR(ERROR_NOT_PERMITTED_EXT);
            STR(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
            STR(THREAD_IDLE_KHR);
            STR(THREAD_DONE_KHR);
            STR(OPERATION_DEFERRED_KHR);
            STR(OPERATION_NOT_DEFERRED_KHR);
            STR(PIPELINE_COMPILE_REQUIRED_EXT);
#undef STR
        default:
            return "UNKNOWN_ERROR";
        }
    }

    inline void Check(const VkResult result, const std::string& operation)
    {
        if (result != VK_SUCCESS)
        {
            std::cerr << "[VULKAN]::Error: failed to " << operation
                << " (" << ToString(result) << ")\n";
            throw std::runtime_error(
                std::string("failed to ") + operation +
                " (" + ToString(result) + ")"
            );
        }
        else
        {
            std::cout << "[VULKAN]::Success: do " << operation << " success!" << std::endl;
        }
    }

	inline VkFormat FromFormat(TextureFormat format) {
		switch (format) {
		case TextureFormat::RGBA8_UNORM:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case TextureFormat::RGBA8_SRGB:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case TextureFormat::BGRA8_UNORM:
			return VK_FORMAT_B8G8R8A8_UNORM;
		case TextureFormat::BGRA8_SRGB:
			return VK_FORMAT_B8G8R8A8_SRGB;
		case TextureFormat::RGB8_UNORM:
			return VK_FORMAT_R8G8B8_UNORM;
		case TextureFormat::R8_UNORM:
			return VK_FORMAT_R8_UNORM;
		case TextureFormat::RGBA16F:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case TextureFormat::RGBA32F:
			return VK_FORMAT_R32G32B32A32_SFLOAT;

			// Block compressed (BCn, aka DXT formats)
		case TextureFormat::BC1_RGB_UNORM:
			return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
		case TextureFormat::BC1_RGB_SRGB:
			return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
		case TextureFormat::BC3_RGBA_UNORM:
			return VK_FORMAT_BC3_UNORM_BLOCK;
		case TextureFormat::BC3_RGBA_SRGB:
			return VK_FORMAT_BC3_SRGB_BLOCK;
		case TextureFormat::BC4_R_UNORM:
			return VK_FORMAT_BC4_UNORM_BLOCK;
		case TextureFormat::BC5_RG_UNORM:
			return VK_FORMAT_BC5_UNORM_BLOCK;
		case TextureFormat::BC7_RGBA_UNORM:
			return VK_FORMAT_BC7_UNORM_BLOCK;
		case TextureFormat::BC7_RGBA_SRGB:
			return VK_FORMAT_BC7_SRGB_BLOCK;

			// ETC2
		case TextureFormat::ETC2_RGBA8_UNORM:
			return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
		case TextureFormat::ETC2_RGBA8_SRGB:
			return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;

			// ASTC
		case TextureFormat::ASTC_4x4_UNORM:
			return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
		case TextureFormat::ASTC_4x4_SRGB:
			return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;

		default:
			return VK_FORMAT_UNDEFINED;
		}
	}
    inline VkVertexInputBindingDescription getBindingDescription(){
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
	inline VkPresentModeKHR ToVkPresentMode(PRESENTMODE mode) {
		switch (mode){
		   case PRESENTMODE::VSYNC:
			   return VK_PRESENT_MODE_FIFO_KHR;                // Guaranteed supported

		   case PRESENTMODE::MAILBOX:
			   return VK_PRESENT_MODE_MAILBOX_KHR;             // Ultra smooth, low latency

		   case PRESENTMODE::IMMEDIATE:
			   return VK_PRESENT_MODE_IMMEDIATE_KHR;           // No vsync, can tear

		   case PRESENTMODE::ADAPTIVE:
			   return VK_PRESENT_MODE_FIFO_RELAXED_KHR;        // Adaptive VSync

		   default:
			   return VK_PRESENT_MODE_FIFO_KHR;
	}
	}

	VkVertexInputBindingDescription inputBindingDescription(uint32_t binding);
	
	VkVertexInputAttributeDescription inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component);
	
	std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent> components);

	void getPipelineVertexInputState(const std::vector<VertexComponent> components,  graphicsPipelineCreateInfoPack& createinfo);


	inline VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(
		VkDescriptorType type,
		VkShaderStageFlags stageFlags,
		uint32_t binding,
		uint32_t descriptorCount = 1)
	{
		VkDescriptorSetLayoutBinding setLayoutBinding{};
		setLayoutBinding.descriptorType = type;
		setLayoutBinding.stageFlags = stageFlags;
		setLayoutBinding.binding = binding;
		setLayoutBinding.descriptorCount = descriptorCount;
		return setLayoutBinding;
	}

	struct Vulkan_SkinnedMesh {
		std::vector<Mesh> offset;
		std::string name;
		struct UniformBuffer {
			VkBuffer buffer;
			VkDeviceMemory memory;
			VkDescriptorBufferInfo descriptor;
			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
			void* mapped;
		} uniformBuffer;

		struct UniformBlock {
			glm::mat4 matrix;
			glm::mat4 jointMatrix[64]{};
			float jointcount{ 0 };
		} uniformBlock;
	};

	struct Vulkan_Mesh {
		std::vector<Mesh> offset;
		std::string name;
	};

	struct Vulkan_Texture {
		std::string				name;
		VkSampler				sampler{ VK_NULL_HANDLE };
		VkImage					image{ VK_NULL_HANDLE };
		VkImageLayout			imageLayout;
		VkDeviceMemory			deviceMemory{ VK_NULL_HANDLE };
		VkImageView				view{ VK_NULL_HANDLE };
		VkDescriptorImageInfo	descriptor;
		uint32_t				width{ 0 };
		uint32_t				height{ 0 };
		uint32_t				mipLevels{ 0 };
		int						index = -1;

		void      updateDescriptor();
	};

	struct Buffer
	{
		VkDevice _device;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkDescriptorBufferInfo descriptor;
		VkDeviceSize size = 0;
		VkDeviceSize alignment = 0;
		void* mapped = nullptr;
		/** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
		VkBufferUsageFlags usageFlags;
		/** @brief Memory property flags to be filled by external source at buffer creation (to query at some later point) */
		VkMemoryPropertyFlags memoryPropertyFlags;
		VkResult map(VkDevice device, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void unmap();
		VkResult bind(VkDeviceSize offset = 0);
		void setupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void copyTo(void* data, VkDeviceSize size);
		VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void destroy();

	
	};
	struct ShaderData {
		Buffer buffer;
		struct Values {
			glm::mat4 projection;
			glm::mat4 view;
			glm::mat4 projectionInverse;
			glm::mat4 viewInverse;
			glm::vec4 lightPos;
			glm::vec4 viewPos;

		} values;
	};

	struct SceneNode {
		SceneNode* parent = nullptr;
		std::string name;
		uint32_t index;
		glm::mat4 matrix;
		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f };
		glm::quat rotation{};
		Skin* skin;
		int32_t skinIndex = -1;
		std::vector<SceneNode*>children;
		std::vector<Vulkan_SkinnedMesh*> mesh;

	};
	struct Vulkan_Material {
		AlphaMode alphaMode;
		float alphaCutoff = 1.0f;
		float metallicFactor = 1.0f;
		float roughnessFactor = 1.0f;
		glm::vec4 baseColorFactor = glm::vec4(1.0f);
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;
		uint32_t baseColorTexture = -1;
		uint32_t matallicRoughnessTexture = -1;
		uint32_t normalTexture = -1;
		uint32_t occlusionTexture = -1;
		uint32_t emissiveTexture = -1;
		uint32_t specularGlossinessTexture = -1;
		uint32_t diffuseTexture =	-1;
	};
	struct VulkanRenderObject {

		bool isVisiable = false;
		std::string name;
		VkDescriptorSet descriptorSets[MAX_FRAMES_IN_FLIGHT];
		struct DescriptorSetLayouts {
			VkDescriptorSetLayout matrices{ VK_NULL_HANDLE };
			VkDescriptorSetLayout textures{ VK_NULL_HANDLE };
		} descriptorSetLayouts;
		std::vector<Vulkan_Mesh> meshes;
		std::vector<Vulkan_Texture> textures;
		std::vector<Vulkan_Material> materials;
		std::unordered_map<int, int> textureIdMapping;

		VkBuffer		vertexBuffer;
		VkDeviceMemory	vertexmemory;

		VkBuffer		indiceBuffer;
		VkDeviceMemory	indicememory;
	};
	struct VulkanRenderScene {
		std::string name;
		VkPipeline pipeline;
		VkPipelineLayout Pipelinelayout;
		utils::vector<VkDescriptorSet> descriptorSets;

		struct DescriptorSetLayouts {
			VkDescriptorSetLayout matrices{ VK_NULL_HANDLE };
			VkDescriptorSetLayout textures{ VK_NULL_HANDLE };
		} descriptorSetLayouts;
		std::vector<SceneNode*> sceneGraph;
		std::vector<Vulkan_SkinnedMesh*> meshes;
		std::vector<Vulkan_Texture> textures;
		std::vector<Vulkan_Material> materials;
		uint32_t textureCount = 0;
		std::unordered_map<int, int> textureIdMapping;

		VkBuffer		vertexBuffer;
		VkDeviceMemory	vertexmemory;

		VkBuffer		indiceBuffer;
		VkDeviceMemory	indicememory;

		std::vector<uint32_t>		indiceCounts;

		void Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
		void drawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, SceneNode* node);
	};
	
	/////////////////////////////////ResouceManager//////////////////////////////////////////


	namespace VulkanResource {
	

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice device);

		void createImage(uint32_t width, uint32_t height, VkFormat format,
			VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
			VkImage& image, VkDeviceMemory& imageMemory, VkDevice& device, VkPhysicalDevice phy_device);

		void createTextureImage();

	

	}
	



}