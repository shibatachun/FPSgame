#include "VulkanCommon.h"

VkVertexInputBindingDescription vulkan::inputBindingDescription(uint32_t binding)
{
	return VkVertexInputBindingDescription({ binding, sizeof(Vertex),VK_VERTEX_INPUT_RATE_VERTEX });
}

VkVertexInputAttributeDescription vulkan::inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component)
{
	switch (component) {
	case VertexComponent::Position:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) });
	case VertexComponent::Normal:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
	case VertexComponent::UV:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });
	case VertexComponent::Color:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color) });
	case VertexComponent::Tangent:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, tangent) });
	case VertexComponent::Joint0:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, joint0) });
	case VertexComponent::Weight0:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, weight0) });
	default:
		return VkVertexInputAttributeDescription({});
	}
}

std::vector<VkVertexInputAttributeDescription> vulkan::inputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent> components)
{
	std::vector<VkVertexInputAttributeDescription> result;
	uint32_t location = 0;
	for (VertexComponent component : components) {
		result.push_back(inputAttributeDescription(binding, location, component));
		location++;
	}
	return result;
}

void vulkan::getPipelineVertexInputState(const std::vector<VertexComponent> components, graphicsPipelineCreateInfoPack& createinfo)
{

	createinfo.vertexInputAttributes = inputAttributeDescriptions(0, components);
	createinfo.vertexInputBindings = { inputBindingDescription(0) };
	createinfo.UpdateAllArrays();
}

uint32_t vulkan::VulkanResource::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice device)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
	//´ý»á³ò³ò¿´
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
	
}

//Vulkan Resouces namespace
void vulkan::VulkanResource::createImage(uint32_t width, uint32_t height, VkFormat format,
	VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
	VkImage& image, VkDeviceMemory& imageMemory, VkDevice& device, VkPhysicalDevice phy_device)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, phy_device);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device, image, imageMemory, 0);

}


void vulkan::VulkanResource::createTextureImage()
{
	/*int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device, stagingBufferMemory);

    stbi_image_free(pixels);

    createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);*/
}

void vulkan::Vulkan_Texture::updateDescriptor()
{
	descriptor.sampler = sampler;
	descriptor.imageView = view;
	descriptor.imageLayout = imageLayout;
}

VkResult vulkan::Buffer::map(VkDevice device, VkDeviceSize size, VkDeviceSize offset)
{
	return vkMapMemory(device, memory, offset, size, 0, &mapped);
}

void vulkan::Buffer::unmap()
{
	if (mapped)
	{
		vkUnmapMemory(_device, memory);
		mapped = nullptr;
	}
}

VkResult vulkan::Buffer::bind(VkDeviceSize offset)
{
	return vkBindBufferMemory(_device, buffer, memory, offset);	

}

void vulkan::Buffer::setupDescriptor(VkDeviceSize size, VkDeviceSize offset)
{
	descriptor.offset = offset;
	descriptor.buffer = buffer;
	descriptor.range = size;
}

void vulkan::Buffer::copyTo(void* data, VkDeviceSize size)
{
	assert(mapped);
	memcpy(mapped, data, size);
}

VkResult vulkan::Buffer::flush(VkDeviceSize size, VkDeviceSize offset)
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	return vkFlushMappedMemoryRanges(_device, 1, &mappedRange);
}

VkResult vulkan::Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	return vkInvalidateMappedMemoryRanges(_device, 1, &mappedRange);
}

void vulkan::Buffer::destroy()
{
	if (buffer)
	{
		vkDestroyBuffer(_device, buffer, nullptr);
	}
	if (memory)
	{
		vkFreeMemory(_device, memory, nullptr);
	}
}

void vulkan::VulkanRenderScene::Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) {
	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indiceBuffer, 0, VK_INDEX_TYPE_UINT32);

	for (auto& node : sceneGraph) {
		drawNode(commandBuffer, pipelineLayout, node);
	}

}

void vulkan::VulkanRenderScene::drawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, SceneNode* node)
{
	for (auto& mesh : node->mesh) {
		if (mesh->offset.size() > 0) {
			glm::mat4 nodeMatrix = node->matrix;
			SceneNode* currentParent = node->parent;
			while (currentParent) {
				nodeMatrix = currentParent->matrix * nodeMatrix;
				currentParent = currentParent->parent;
			}
			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &nodeMatrix);
			for (Mesh& offset : mesh->offset) {
				if (offset.indiceCount > 0) {
					Vulkan_Material& material = materials[offset.materialID];
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &material.descriptorSet, 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, offset.indiceCount, 1, offset.indexOffset,0,0);
				}
			}
		}
	}
	for (auto& child : node->children) {
		drawNode(commandBuffer, pipelineLayout, child);
	}
}

