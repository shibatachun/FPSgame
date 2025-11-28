#include "VulkanRenderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
///////////////////////////////////////////////////////////////Common Renderer Function////////////////////////////////////////////////////////////////////
bool vulkan::VulkanRenderer::Init()
{
	
	InitVulkan();
	
	return true;
	 
}

void vulkan::VulkanRenderer::DrawFrame()
{
	vkWaitForFences(_devices->Handle(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);
	
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(_devices->Handle(), _swapchain->Handle(), UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		BuildCommandBuffer();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	vkResetFences(_devices->Handle(), 1, &_inFlightFences[_currentFrame]);
	//vkResetCommandBuffer(_commandBuffers[_currentFrame], 0);
	UpdateUniformBuffer(_currentFrame);
	UpdateGridBuffer(_currentFrame);
	UpdateSkyBuffer(_currentFrame);
	UpdatePBR(_currentFrame);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[_currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandBuffers[imageIndex];
	
	
	VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[_currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	vkQueueSubmit(_devices->GraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]);
	//Check(vkQueueSubmit(_devices->GraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]), "Submit to graphics queue family");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { _swapchain->Handle() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;
	result = vkQueuePresentKHR(_devices->PresentQueue(), &presentInfo);
	// vkDeviceWaitIdle(_devices->Handle());
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized){
		_framebufferResized = false;
		RecreateSwapChain();
		BuildCommandBuffer();
	}

	else if (result != VK_SUCCESS){
		throw std::runtime_error("failed to present swap chain Image!");
	}

	_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	
}

void vulkan::VulkanRenderer::Cleanup()
{
	
	_devices->WaitIdle();
	_swapchain->CleanUpSwapChain();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(_devices->Handle(), _uniformData[i].buffer.buffer, nullptr);
		vkFreeMemory(_devices->Handle(), _uniformData[i].buffer.memory, nullptr);
	}
	_descriptorPools.reset();
	_descriptorLayouts.reset();

	_resouceManager.reset();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(_devices->Handle(), _renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(_devices->Handle(), _imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(_devices->Handle(), _inFlightFences[i], nullptr);
	}
	_renderPass->Destroy();
	_graphicsPipline.reset();
	_commandPools.reset();
	_devices.reset();
	_surface.reset();
#ifdef _DEBUG
	_debugMessenger.reset();
#endif // _DEBUG
	_instance.reset();
}


//////////////////////////////////////////////////////////////Vulkan Exclusive/////////////////////////////////////////////////////////////////////////////
bool vulkan::VulkanRenderer::InitVulkan()
{
	
	_instance.reset(new vulkan::Instance(_window));
	_surface.reset(new vulkan::Surface(*_instance));
	_debugMessenger.reset(new vulkan::DebugUtilsMessenger(*_instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT));
	SetPhysicalDevices();
	SetUpCommandPools();
	SetUpBufferManager();
	SetSwapChain();
	SetUpDescriptorLayoutManager();
	SetUpDescriptorPoolsManager();
	SetUpGraphicPipelineManager();
	SetUpVulkanResouceManager();
	
	_GpuResouce.reset(new vulkan::VulkanGraphicResourceManager(*_instance,*_devices,*_swapchain,*_descriptorPools,*_descriptorLayouts,*_graphicsPipline));
	_GpuResouce->Init(); 
	BufferCreation bc;
	TextureCreation tc;
	ModelData model = _assetManager.getModelDataByName("Sponza");
	VkBufferUsageFlags flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bc.set(flags, ResourceUsageType::Immutable, (sizeof(Vertex) * model.vertexSize)).set_persistent(true).set_name("Test object");
	const Image& image = _assetManager.getImageDataByName(model.images[0]);
	tc.set_name(image.name.c_str()).set_size(image.texWidth, image.texHeight, image.texDepth).set_data(image.pixel).set_mips(image.mipLevels).set_format_type(FromFormat(image.format), TextureType::Texture2D);
	BufferHandle br = _GpuResouce->CreateBufferResouce(bc);
	TextureHandle tr = _GpuResouce->CreateTextureResource(tc);
	VulkanTexture* vk_tx = _GpuResouce->AccessTexture(tr);
	
	//_GpuResouce->CreateBufferResouce(bc);
	//VulkanBuffer* vertex_buffer = _GpuResouce->AccessBuffer(br);
	CreateFrameBuffer();
	CreateCommandBuffer(QueueFamily::GRAPHIC);
	CreateUniformBuffers();
	CreateSyncObjects();
	//CreateUniformBuffers();

	PrepareRenderObject();
	CreateDebugPipeline();
	CreateSkyPipeline();
	BuildCommandBuffer();
	
	return true;
}

//Set up Devices
void vulkan::VulkanRenderer::SetPhysicalDevices()
{
	if (_devices)
	{
		throw std::logic_error("physical devices has already been set");
		return;
	}
	std::vector<const char*> requeredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	auto isDeviceSuitable = [&](VkPhysicalDevice device)->bool {
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceFeatures features{};
		vkGetPhysicalDeviceProperties(device, &properties);
		vkGetPhysicalDeviceFeatures(device, &features);

		return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && features.geometryShader;
		};
	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
	
	for (VkPhysicalDevice device : _instance->PhysicalDevices())
	{
		if (isDeviceSuitable(device))
		{
			_physicalDevice = device;
			
		}
	}
	VkPhysicalDeviceProperties props{};
	vkGetPhysicalDeviceProperties(_physicalDevice, &props);
	std::cout << "Using physical device : " << props.deviceName << std::endl;
	_devices.reset(new vulkan::Device(_physicalDevice, *_surface, requeredExtensions, nullptr));



}

//Set up SwapChain
void vulkan::VulkanRenderer::SetSwapChain()
{
	while (IsMinimized())
	{
		
	}

	_swapchain.reset(new vulkan::SwapChain(*_devices,*_bufferManager, _presentMode));


}

//Set up DescriptorLayoutManager
void vulkan::VulkanRenderer::SetUpDescriptorLayoutManager()
{
	_descriptorLayouts.reset(new DescriptorLayoutManager(*_devices));
}

//Set up DescriptorPoolsManger
void vulkan::VulkanRenderer::SetUpDescriptorPoolsManager()
{
	_descriptorPools.reset(new DescriptorPoolManager(*_devices));
}

//Setup Pipeline manager
void vulkan::VulkanRenderer::SetUpGraphicPipelineManager()
{
	_renderPass.reset(new vulkan::RenderPass(*_swapchain));
	_graphicsPipline.reset(new vulkan::GraphicPipelineManager(_assetManager.getShaderAssets(), _devices->Handle(),*_swapchain,*_renderPass));
	LayoutConfig config;
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	config.bindings.push_back(uboLayoutBinding);
	config.bindings.push_back(samplerLayoutBinding);
	config.UpdateAllArray();

	_descriptorLayouts->CreateDescriptorSetLayout(config);

	//_graphicsPipline->createPipelineLayout("default", _descriptorLayouts->GetDescriptorSetLayout(config));

	//_graphicsPipline->CreateGraphicsPipeline("test_triangle_vulkan","default",_assetManager.getShaderByName("Rectangle_Vulkan"),_renderPass->GetRenderPass());
	

	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,_descriptorLayouts->GetDescriptorSetLayout(config));
	_descriptorPools->CreatePreFrameDescriptorSets(layouts);
}


//Set up Command Pool
void vulkan::VulkanRenderer::SetUpCommandPools()
{
	if (!_commandPools)
	{
		_commandPools.reset(new vulkan::CommandPoolManager(*_devices));
	}
}

//Set up Buffer Manager
void vulkan::VulkanRenderer::SetUpBufferManager()
{
	_bufferManager.reset(new BufferManager(*_devices, *_commandPools));
}

//Set up Vulkan Resouce Manager
void vulkan::VulkanRenderer::SetUpVulkanResouceManager()
{
	_resouceManager.reset(new vulkan::VulkanResouceManager(*_bufferManager,*_descriptorPools,*_descriptorLayouts,*_graphicsPipline, *_devices,*_instance, _assetManager));

}


//Set up Sync Objects
void vulkan::VulkanRenderer::CreateSyncObjects()
{
	_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		Check(vkCreateSemaphore(_devices->Handle(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]), "Create Image avaiable Semaphore");
		Check(vkCreateSemaphore(_devices->Handle(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]), "Create render finish Semaphore");
		Check(vkCreateFence(_devices->Handle(), &fenceInfo, nullptr, &_inFlightFences[i]), "Create fence");
	}

}



/// <summary>
/// Rendered Core
/// </summary>

void vulkan::VulkanRenderer::CreateFrameBuffer()
{
	_swapchain->CreateFrameBuffer(_renderPass->GetRenderPass());
	
}

void vulkan::VulkanRenderer::CreateCommandBuffer(QueueFamily family)
{
	_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo allocaInfo{};
	allocaInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocaInfo.commandPool = _commandPools->GetCommandPool(family);
	allocaInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocaInfo.commandBufferCount = (uint32_t)_commandBuffers.size();
	Check(vkAllocateCommandBuffers(_devices->Handle(), &allocaInfo, _commandBuffers.data()), "Allocate Command buffer!");
}

void vulkan::VulkanRenderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VulkanRenderScene object)
{
	
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	//Check(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Start record commandbuffer");


	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = _renderPass->GetRenderPass();
	renderPassInfo.framebuffer =_swapchain->GetSwapChainBuffer()[imageIndex];
	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = _swapchain->GetSwapchainExtent();
	std::vector<VkClearValue> clearValues{};

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f,1.0f}} };
	VkClearValue depthStencil{};
	depthStencil.depthStencil = { 1.0f, 0 };
	clearValues.push_back(clearColor);
	clearValues.push_back(depthStencil);
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();


	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(_swapchain->GetSwapchainExtent().width);
	viewport.height = static_cast<float>(_swapchain->GetSwapchainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = _swapchain->GetSwapchainExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _skyPass._pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _skyPass._pipelineLayout, 0, 1, &_skyPass._sets[imageIndex], 0, nullptr);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _debugPipeline._pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _debugPipeline._pipelineLayout, 0, 1, &_debugPipeline._sets[imageIndex], 0, nullptr);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
	
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, object.Pipelinelayout, 0, 1, &object.descriptorSets[imageIndex], 0, nullptr);
	object.Draw(commandBuffer, object.Pipelinelayout);


	vkCmdEndRenderPass(commandBuffer);

	vkEndCommandBuffer(commandBuffer);
	//Check(vkEndCommandBuffer(commandBuffer), "Record command buffer!");

}

void vulkan::VulkanRenderer::RecreateSwapChain()
{
	int width = 0, height = 0;
	SDL_Window* sdl_window = (SDL_Window*)_window;
	SDL_Vulkan_GetDrawableSize(sdl_window, &width, &height);
	SDL_Event event;
	while (width == 0 || height == 0) {
		SDL_WaitEvent(&event);
		SDL_Vulkan_GetDrawableSize(sdl_window, &width, &height);
	}

	_devices->WaitIdle();
	_swapchain->CleanUpSwapChain();
	_swapchain->CreateSwapChain(_presentMode);
	_swapchain->CreateDepthResources();
	_swapchain->CreateFrameBuffer(_renderPass->GetRenderPass());
	BuildCommandBuffer();
	_devices->WaitIdle();
	
}

void vulkan::VulkanRenderer::CreateUniformBuffers()
{
	
	_uniformData.resize(MAX_FRAMES_IN_FLIGHT);
	_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	_skyData.resize(MAX_FRAMES_IN_FLIGHT);
	_skyDataBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
	
	_gridData.resize(MAX_FRAMES_IN_FLIGHT);
	_gridDataBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	_pbrData.resize(MAX_FRAMES_IN_FLIGHT);
	_pbrDataBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		//Scenc UBO
		_bufferManager->createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(_uniformData[i].values),
			&_uniformData[i].buffer.buffer, &_uniformData[i].buffer.memory);
		_uniformData[i].buffer.setupDescriptor();
		_uniformData[i].buffer.map(_devices->Handle());
		//Sky UBO
		_bufferManager->createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(_skyData[i].values),
			&_skyData[i].buffer.buffer, &_skyData[i].buffer.memory);
		_skyData[i].buffer.setupDescriptor();
		_skyData[i].buffer.map(_devices->Handle());

		//Grid UBO
		_bufferManager->createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(_gridData[i].values),
			&_gridData[i].buffer.buffer, &_gridData[i].buffer.memory);
		_gridData[i].buffer.setupDescriptor();
		_gridData[i].buffer.map(_devices->Handle());
		
		//pbr UBO
		_bufferManager->createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(_pbrData[i].values),
			&_pbrData[i].buffer.buffer, &_pbrData[i].buffer.memory);
		_pbrData[i].buffer.setupDescriptor();
		_pbrData[i].buffer.map(_devices->Handle());
	}
}



void vulkan::VulkanRenderer::ConfigureDescriptorSet(VulkanRenderScene& object)
{
	if (object.descriptorSets.size() == MAX_FRAMES_IN_FLIGHT) {
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			utils::vector<VkDescriptorBufferInfo> descriptors = { _uniformData[i].buffer.descriptor ,_pbrData[i].buffer.descriptor };
			_descriptorPools->AllocateDescriptorSet(object.descriptorSetLayouts.matrices, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, object.descriptorSets[i], 0, descriptors, i);
	
		}
	}
	
	for (auto& x : object.sceneGraph) {
		_descriptorPools->PrepareNodeDescriptor(x, object.descriptorSetLayouts.matrices);
	}

	for (auto& material : object.materials) {
		_descriptorPools->AllocateImageDescriptorSet(material, object.textures, object.descriptorSetLayouts.textures);
	}

	std::cout << "stop" << std::endl;
	
	
}

void vulkan::VulkanRenderer::ConfigureDescriptorSet(VulkanRenderObject & object)
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		utils::vector<VkDescriptorBufferInfo> descriptors = { _uniformData[i].buffer.descriptor };
		_descriptorPools->AllocateDescriptorSet(object.descriptorSetLayouts.matrices, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, object.descriptorSets[i], 0, descriptors, i);
	}
	for (auto& material : object.materials) {
		_descriptorPools->AllocateImageDescriptorSet(material, object.textures, object.descriptorSetLayouts.textures);
	}
	
}

void vulkan::VulkanRenderer::ConfigurePipeline(VulkanRenderScene& object)
{
	std::vector<VkDescriptorSetLayout> setLayouts = { object.descriptorSetLayouts.matrices, object.descriptorSetLayouts.textures };
	VkPipelineLayoutCreateInfo pipelineLayoutCI = initializers::pipelineLayoutCreateInfo(setLayouts.data(), static_cast<uint32_t>(setLayouts.size()));
	VkPushConstantRange pushConstantRange = initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), 0);
	pipelineLayoutCI.pushConstantRangeCount = 1;
	pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;
	Check(vkCreatePipelineLayout(_devices->Handle(), &pipelineLayoutCI, nullptr, &object.Pipelinelayout), "Create render object pipeline layout");

	graphicsPipelineCreateInfoPack GPCI;
	GPCI.inputAssemblyStateCi = initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
	GPCI.rasterizationStateCi = initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
	GPCI.colorBlendAttachmentStates.push_back(initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE));
	GPCI.depthStencilStateCi = initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
	GPCI.viewportStateCi = initializers::pipelineViewportStateCreateInfo(1, 1, 0);
	GPCI.multisampleStateCi = initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
	GPCI.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	GPCI.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
	getPipelineVertexInputState({ VertexComponent::Position, VertexComponent::Normal, VertexComponent::UV, VertexComponent::Color, VertexComponent::Tangent }, GPCI);
	GPCI.createInfo.layout = object.Pipelinelayout;
	GPCI.createInfo.renderPass = _renderPass->GetRenderPass();
	GPCI.createInfo.stageCount = 2;
	GPCI.shaderStages.resize(2);
	const auto& shaders = _assetManager.getShaderByName("pbr");
	for (auto& material : object.materials) {
		struct MaterialSpecializationData {
			VkBool32 alphaMask;
			float alphaMaskCutoff;
		} materialSpecializationData;

		materialSpecializationData.alphaMask = material.alphaMode == ALPHAMODE_MASK;
		materialSpecializationData.alphaMaskCutoff = material.alphaCutoff;

		std::vector<VkSpecializationMapEntry> specializationMapEntries = {
			initializers::specializationMapEntry(0, offsetof(MaterialSpecializationData, alphaMask), sizeof(MaterialSpecializationData::alphaMask)),
			initializers::specializationMapEntry(1, offsetof(MaterialSpecializationData, alphaMaskCutoff), sizeof(MaterialSpecializationData::alphaMaskCutoff))
		};
		VkSpecializationInfo specializationInfo = initializers::specializationInfo(specializationMapEntries, sizeof(materialSpecializationData), &materialSpecializationData);
		GPCI.shaderStages[1].pSpecializationInfo = &specializationInfo;

		material.pipeline = _graphicsPipline->CreateGraphicsPipeline(object.name, GPCI, shaders);
	}
	
	std::cout << "stop" << std::endl;
	
}

void vulkan::VulkanRenderer::UpdateUniformBuffer(uint32_t currentImage)
{
	//static auto startTime = std::chrono::high_resolution_clock::now();
	//
	//auto currentTime = std::chrono::high_resolution_clock::now();
	//float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	
	_uniformData[currentImage].values.view=	gCamera.matrices.view;
	_uniformData[currentImage].values.projection =	gCamera.matrices.perspective;
	_uniformData[currentImage].values.viewInverse = glm::inverse(gCamera.matrices.view);
	_uniformData[currentImage].values.projectionInverse = glm::inverse(gCamera.matrices.perspective);
	_uniformData[currentImage].values.lightPos = glm::vec4(gCamera.sunDir, 1000.0f);
	utils::printVec(_uniformData[currentImage].values.lightPos);
	_uniformData[currentImage].values.viewPos = gCamera.viewPos;
	memcpy(_uniformData[currentImage].buffer.mapped, &_uniformData[currentImage].values, sizeof(_uniformData[currentImage].values));
}
void vulkan::VulkanRenderer::UpdateSkyBuffer(uint32_t currentImage) {
	_skyData[currentImage].values.viewInverse = glm::inverse(gCamera.matrices.view);
	_skyData[currentImage].values.projInverse = glm::inverse(gCamera.matrices.perspective);
	//_skyData[currentImage].values.cameraPos = gCamera.viewPos;
	
	_skyData[currentImage].values.sunDirection = glm::vec4(gCamera.sunDir, 0.0f);
	_skyData[currentImage].values.screenSize = glm::vec2((float)_swapchain->GetSwapchainExtent().width, (float)_swapchain->GetSwapchainExtent().height);
	memcpy(_skyData[currentImage].buffer.mapped, &_skyData[currentImage].values, sizeof(_skyData[currentImage].values));

}
void vulkan::VulkanRenderer::UpdateGridBuffer(uint32_t currentImage) {
	_gridData[currentImage].values.view = gCamera.matrices.view;
	_gridData[currentImage].values.proj = gCamera.matrices.perspective;
	_gridData[currentImage].values.viewInverse = glm::inverse(gCamera.matrices.view);
	_gridData[currentImage].values.projInverse = glm::inverse(gCamera.matrices.perspective);
	memcpy(_gridData[currentImage].buffer.mapped, &_gridData[currentImage].values, sizeof(_gridData[currentImage].values));
}

void vulkan::VulkanRenderer::UpdatePBR(uint32_t currentImage)
{
	memcpy(_pbrData[currentImage].buffer.mapped, &_pbrData[currentImage].values, sizeof(_pbrData[currentImage].values));
}
bool vulkan::VulkanRenderer::IsMinimized() const
{
	int width, height;
	SDL_Window* sdl_window = (SDL_Window*)_window;
	SDL_Vulkan_GetDrawableSize(sdl_window, &width, &height);

	return width==0 && height==0;
}

void vulkan::VulkanRenderer::PrepareRenderScene()
{
	/*_resouceManager->ConstructVulkanRenderObject("Sponza",
		"Sponza");*/
	for (auto& x : _resouceManager->GetRenderScene()) {
		ConfigureDescriptorSet(x);
		ConfigurePipeline(x);
		_renderObjects.push_back(x);
	}
	std::cout << "stop PrepareRenderObject" << std::endl;
}

void vulkan::VulkanRenderer::PrepareRenderObject()
{
	
	//_resouceManager->ConstructVulkanRenderObject("viking",
	//	"viking_room", { "viking" });
	
	_resouceManager->ConstructVulkanRenderObject("generator", "generator");
	for (auto& x : _resouceManager->GetRenderObjects()) {
		ConfigureDescriptorSet(x);

	}

	

}

void vulkan::VulkanRenderer::BuildCommandBuffer()
{
	for (const auto& x : _renderObjects) {

		for (uint32_t i = 0; i < _commandBuffers.size(); i++)
		{
			RecordCommandBuffer(_commandBuffers[i], i, x);

		}
	}

}

void vulkan::VulkanRenderer::CreateDebugPipeline()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;
	
	vkCreateDescriptorSetLayout(_devices->Handle(), &layoutInfo, nullptr, &_debugPipeline._setLayout);
	
	VkDescriptorSetLayout setLayouts[] = { _debugPipeline._setLayout };
	_debugPipeline._sets.resize(MAX_FRAMES_IN_FLIGHT);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		utils::vector<VkDescriptorBufferInfo> descriptors = { _gridData[i].buffer.descriptor };
		_descriptorPools->AllocateDescriptorSet(_debugPipeline._setLayout, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _debugPipeline._sets[i], 0, descriptors, i);
	}
	
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &_debugPipeline._setLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	vkCreatePipelineLayout(_devices->Handle(), &pipelineLayoutInfo, nullptr, &_debugPipeline._pipelineLayout);

	graphicsPipelineCreateInfoPack GPCI;
	GPCI.inputAssemblyStateCi = initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
	GPCI.rasterizationStateCi = initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
	GPCI.colorBlendAttachmentStates.push_back(initializers::pipelineColorBlendAttachmentState(0xf,VK_FALSE));
	GPCI.depthStencilStateCi = initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
	GPCI.viewportStateCi = initializers::pipelineViewportStateCreateInfo(1, 1, 0);
	GPCI.multisampleStateCi = initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
	VkPipelineVertexInputStateCreateInfo vertexInput{};
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInput.vertexBindingDescriptionCount = 0;
	vertexInput.vertexAttributeDescriptionCount = 0;
	GPCI.vertexInputStateCi = vertexInput;
	GPCI.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	GPCI.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
	GPCI.createInfo.layout = _debugPipeline._pipelineLayout;
	GPCI.createInfo.renderPass = _renderPass->GetRenderPass();
	GPCI.createInfo.stageCount = 2;
	GPCI.shaderStages.resize(2);
	const auto& shader = _assetManager.getShaderByName("grid");
	_debugPipeline._pipeline = _graphicsPipline->CreateGraphicsPipeline("debug_pipeline", GPCI, shader);
	//_debugPipeline._pipeline = _graphicsPipline->DebugPipelineTest(shader, _debugPipeline._pipelineLayout);


	
	
}

void vulkan::VulkanRenderer::CreateSkyPipeline()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;


	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	vkCreateDescriptorSetLayout(_devices->Handle(), &layoutInfo, nullptr, &_skyPass._setLayout);

	VkDescriptorSetLayout setLayouts[] = { _skyPass._setLayout };
	_skyPass._sets.resize(MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		utils::vector<VkDescriptorBufferInfo> descriptors = { _skyData[i].buffer.descriptor };
		_descriptorPools->AllocateDescriptorSet(_skyPass._setLayout, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _skyPass._sets[i], 0, descriptors, i);
	}


	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &_debugPipeline._setLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	vkCreatePipelineLayout(_devices->Handle(), &pipelineLayoutInfo, nullptr, &_skyPass._pipelineLayout);

	graphicsPipelineCreateInfoPack GPCI;
	GPCI.inputAssemblyStateCi = initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
	GPCI.rasterizationStateCi = initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
	GPCI.colorBlendAttachmentStates.push_back(initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE));
	GPCI.depthStencilStateCi = initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
	GPCI.viewportStateCi = initializers::pipelineViewportStateCreateInfo(1, 1, 0);
	GPCI.multisampleStateCi = initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
	VkPipelineVertexInputStateCreateInfo vertexInput{};
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInput.vertexBindingDescriptionCount = 0;
	vertexInput.vertexAttributeDescriptionCount = 0;
	GPCI.vertexInputStateCi = vertexInput;
	GPCI.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	GPCI.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
	GPCI.createInfo.layout = _skyPass._pipelineLayout;
	GPCI.createInfo.renderPass = _renderPass->GetRenderPass();
	GPCI.createInfo.stageCount = 2;
	GPCI.shaderStages.resize(2);
	const auto& shader = _assetManager.getShaderByName("sky");
	_skyPass._pipeline = _graphicsPipline->CreateGraphicsPipeline("sky_pipeline", GPCI, shader);

}





//Constructor
vulkan::VulkanRenderer::VulkanRenderer(void* window, PRESENTMODE presentmode, asset::AssetManager& assetManager) :_window(window), _presentMode(ToVkPresentMode(presentmode)), _assetManager(assetManager)
{
	if (!Init())
	{
		std::cout << "Init vulkan Failed!" << std::endl;
	}
	
}
