#include "VulkanClasses.h"


#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
vulkan::Instance::Instance(void* window) : _window(window)
{
	
	CheckVulkanMinimumVersion(_vulkanVersion);

	uint32_t sdlExtensionCount = 0;
	SDL_Vulkan_GetInstanceExtensions((SDL_Window*)_window, &sdlExtensionCount, nullptr);

	std::vector<const char*> extensions(sdlExtensionCount);
	SDL_Vulkan_GetInstanceExtensions((SDL_Window*)_window, &sdlExtensionCount, extensions.data());

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else 
	const bool enableValidationLayers = true;
#endif
	if (enableValidationLayers)
	{
		validationLayers_.push_back("VK_LAYER_KHRONOS_validation");
	}
	CheckValidationLayerSupport(validationLayers_);
	if (!validationLayers_.empty())
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Homemade_Vulkan";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "VulkanEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = _vulkanVersion;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers_.size());
	createInfo.ppEnabledLayerNames = validationLayers_.data();

	Check(vkCreateInstance(&createInfo, nullptr, &instance_), "create instance");
	GetVulkanPhysicalDevices();

}

vulkan::Instance::~Instance()
{
	if (instance_ != nullptr)
	{
		vkDestroyInstance(instance_, nullptr);
		instance_ = nullptr;
	}
}

void* vulkan::Instance::getWindow() const
{
	return _window;
}

void vulkan::Instance::GetVulkanExtensions()
{

}

void vulkan::Instance::GetVulkanLayers()
{
}

void vulkan::Instance::GetVulkanPhysicalDevices()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
	deviceCount == 0 ? throw std::runtime_error("failed to find GPUs with Vulkan support!")  : physicalDevices_.resize(deviceCount);
	vkEnumeratePhysicalDevices(instance_, &deviceCount, physicalDevices_.data());

}

void vulkan::Instance::CheckVulkanMinimumVersion(const uint32_t minVersion)
{
	uint32_t version;
	Check(vkEnumerateInstanceVersion(&version), "query instance version");
	if (minVersion > version)
	{
		
		throw std::runtime_error("minimum required version not found");
	}
}

void vulkan::Instance::CheckValidationLayerSupport(const std::vector<const char*>& validationLayers)
{


	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


	for (const char* layerName : validationLayers)
	{
#ifdef _DEBUG
		std::cout << layerName << std::endl;
#endif // DEBUG
		bool layerFound = false;
		for (const auto& LayerProperties : availableLayers)
		{
#ifdef _DEBUG
			std::cout << LayerProperties.layerName << std::endl;
#endif // DEBUG
			if (strcmp(layerName, LayerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			throw std::runtime_error("cound not find the requested validation layer");
		}
	}
}

vulkan::Surface::Surface(const Instance& instance) : instance_(instance)
{
	
	SDL_Window* window = (SDL_Window*)instance_.getWindow();
	if (SDL_Vulkan_CreateSurface(window, instance_.Handle(), &surface_) == SDL_FALSE) {
		throw std::runtime_error(SDL_GetError());
	}
}

vulkan::Surface::~Surface()
{
	if (surface_ != nullptr)
	{
		vkDestroySurfaceKHR(instance_.Handle(), surface_, nullptr);
	}
}

namespace
{

	const char* ObjectTypeToString(const VkObjectType objectType)
	{
		switch (objectType)
		{
#define STR(e) case VK_OBJECT_TYPE_ ## e: return # e
			STR(UNKNOWN);
			STR(INSTANCE);
			STR(PHYSICAL_DEVICE);
			STR(DEVICE);
			STR(QUEUE);
			STR(SEMAPHORE);
			STR(COMMAND_BUFFER);
			STR(FENCE);
			STR(DEVICE_MEMORY);
			STR(BUFFER);
			STR(IMAGE);
			STR(EVENT);
			STR(QUERY_POOL);
			STR(BUFFER_VIEW);
			STR(IMAGE_VIEW);
			STR(SHADER_MODULE);
			STR(PIPELINE_CACHE);
			STR(PIPELINE_LAYOUT);
			STR(RENDER_PASS);
			STR(PIPELINE);
			STR(DESCRIPTOR_SET_LAYOUT);
			STR(SAMPLER);
			STR(DESCRIPTOR_POOL);
			STR(DESCRIPTOR_SET);
			STR(FRAMEBUFFER);
			STR(COMMAND_POOL);
			STR(SAMPLER_YCBCR_CONVERSION);
			STR(DESCRIPTOR_UPDATE_TEMPLATE);
			STR(SURFACE_KHR);
			STR(SWAPCHAIN_KHR);
			STR(DISPLAY_KHR);
			STR(DISPLAY_MODE_KHR);
			STR(DEBUG_REPORT_CALLBACK_EXT);
			STR(DEBUG_UTILS_MESSENGER_EXT);
			STR(ACCELERATION_STRUCTURE_KHR);
			STR(VALIDATION_CACHE_EXT);
			STR(PERFORMANCE_CONFIGURATION_INTEL);
			STR(DEFERRED_OPERATION_KHR);
			STR(INDIRECT_COMMANDS_LAYOUT_NV);
#undef STR
		default: return "unknown";
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		const VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* const pCallbackData,
		void* const pUserData)
	{
		(void)pUserData;

		

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			std::cerr << "VERBOSE: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			std::cerr << "INFO: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			std::cerr << "WARNING: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			std::cerr << "ERROR: ";
			break;
		default:;
			std::cerr << "UNKNOWN: ";
		}

		switch (messageType)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			std::cerr << "GENERAL: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			std::cerr << "VALIDATION: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			std::cerr << "PERFORMANCE: ";
			break;
		default:
			std::cerr << "UNKNOWN: ";
		}

		std::cerr << pCallbackData->pMessage;

		if (pCallbackData->objectCount > 0 && messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		{
			std::cerr << "\n\n  Objects (" << pCallbackData->objectCount << "):\n";

			for (uint32_t i = 0; i != pCallbackData->objectCount; ++i)
			{
				const auto object = pCallbackData->pObjects[i];
				std::cerr
					<< "  - Object[" << i << "]: "
					<< "Type: " << ObjectTypeToString(object.objectType) << ", "
					<< "Handle: " << reinterpret_cast<void*>(object.objectHandle) << ", "
					<< "Name: '" << (object.pObjectName ? object.pObjectName : "") << "'"
					<< "\n";
			}
		}

		std::cerr << std::endl;

		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback)
	{
		const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
		return func != nullptr
			? func(instance, pCreateInfo, pAllocator, pCallback)
			: VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator)
	{
		const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
		if (func != nullptr) {
			func(instance, callback, pAllocator);
		}
	}
}

vulkan::DebugUtilsMessenger::DebugUtilsMessenger(const Instance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold)
	: instance_(instance), threshold_(threshold)
{
	if (instance.GetValidationLayers().empty())
	{
		return;
	}

	VkDebugUtilsMessageSeverityFlagsEXT severity = 0;


	switch (threshold)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		break;
	default:
		throw (std::invalid_argument("invalid threshold"));
	}


	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = severity;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = VulkanDebugCallback;
	createInfo.pUserData = nullptr;

	Check(CreateDebugUtilsMessengerEXT(instance_.Handle(), &createInfo, nullptr, &messenger_),
		"set up Vulkan debug callback");


}

vulkan::DebugUtilsMessenger::~DebugUtilsMessenger()
{
	if (messenger_ != nullptr)
	{
		DestroyDebugUtilsMessengerEXT(instance_.Handle(), messenger_, nullptr);
		messenger_ = nullptr;
	}
}

vulkan::DebugUtils::DebugUtils(VkInstance instance) :
	vkSetDebugUtilsObjectNameEXT_(reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT")))
{
#ifndef NDEBUG
	if (vkSetDebugUtilsObjectNameEXT_ == nullptr)
	{
		throw (std::runtime_error("failed to get address of 'vkSetDebugUtilsObjectNameEXT'"));
	}
#endif
}

namespace
{
	std::vector<VkQueueFamilyProperties>::const_iterator FindQueue(
		const std::vector<VkQueueFamilyProperties>& queueFamilies,
		const std::string& name,
		const VkQueueFlags requiredBits,
		const VkQueueFlags excludedBits)
	{
		const auto family = std::find_if(queueFamilies.begin(), queueFamilies.end(), [requiredBits, excludedBits](const VkQueueFamilyProperties& queueFamily)
			{
				return
					queueFamily.queueCount > 0 &&
					queueFamily.queueFlags & requiredBits &&
					!(queueFamily.queueFlags & excludedBits);
			});

		if (family == queueFamilies.end())
		{
			throw (std::runtime_error("found no matching " + name + " queue"));
		}

		return family;
	}
}
vulkan::Device::Device(
	VkPhysicalDevice device, 
	const Surface& surface, 
	const std::vector<const char*>& requiredExtensions, 
	const void* nextDeviceFeatures):
	_physicalDevice(device),
	_surface(surface),
	_debugUtils(surface.getInstance().Handle())
{
	CheckRequiredExtensions(_physicalDevice, requiredExtensions);

	uint32_t QueueFamilyuCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &QueueFamilyuCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamily_;
	if (QueueFamilyuCount) 
	{
		queueFamily_.resize(QueueFamilyuCount);
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &QueueFamilyuCount, queueFamily_.data());
	}
	else
	{
		throw std::logic_error("Unable find Device Qeueue Family Propertie");

	}
	const auto graphicsFamily = FindQueue(queueFamily_, "graphics", VK_QUEUE_GRAPHICS_BIT, 0);
	const auto computeFamily = FindQueue(queueFamily_, "compute", VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);
	const auto transferFamily = FindQueue(queueFamily_, "transfer", VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT);

	const auto presentFamily = std::find_if(queueFamily_.begin(), queueFamily_.end(), [&](const VkQueueFamilyProperties& queueFamily) {
		VkBool32 presentSupport = false;
		const uint32_t i = static_cast<uint32_t>(queueFamily_.data() - &queueFamily);
		//const uint32_t i = static_cast<uint32_t>(std::distance(queueFamily_.data(), &queueFamily));
		vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, surface.Handle(), &presentSupport);
		return queueFamily.queueCount > 0 && presentSupport;
		});
	if (presentFamily == queueFamily_.end())
	{
		throw (std::runtime_error("found no presentation queue"));
	}
	_graphicsFamilyIndex = static_cast<uint32_t>(graphicsFamily - queueFamily_.begin());
	_presentFamilyIndex = static_cast<uint32_t>(presentFamily - queueFamily_.begin());
	_transferFamilyIndex = static_cast<uint32_t>(transferFamily - queueFamily_.begin());

	//获取各个Queuefamilies
	const std::set<uint32_t> uniqueQueueFamilies = {
		_graphicsFamilyIndex,
		_presentFamilyIndex,
		_transferFamilyIndex,
	};
	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	//启用各个features
	VkPhysicalDeviceFeatures deviceFeatures10 = {
	 .geometryShader = vkFeatures10_.features.geometryShader, // enable if supported
	 .tessellationShader = vkFeatures10_.features.tessellationShader, // enable if supported
	 .sampleRateShading = VK_TRUE,
	 .multiDrawIndirect = VK_TRUE,
	 .drawIndirectFirstInstance = VK_TRUE,
	 .depthBiasClamp = VK_TRUE,
	 .fillModeNonSolid = vkFeatures10_.features.fillModeNonSolid, // enable if supported
	 .samplerAnisotropy = VK_TRUE,
	 .textureCompressionBC = vkFeatures10_.features.textureCompressionBC, // enable if supported
	 .vertexPipelineStoresAndAtomics = vkFeatures10_.features.vertexPipelineStoresAndAtomics, // enable if supported
	 .fragmentStoresAndAtomics = VK_TRUE,
	 .shaderImageGatherExtended = VK_TRUE,
	 .shaderInt64 = vkFeatures10_.features.shaderInt64, // enable if supported
	};
	VkPhysicalDeviceVulkan11Features deviceFeatures11 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
		.pNext = &deviceFeatures10,
		.storageBuffer16BitAccess = VK_TRUE,
		.samplerYcbcrConversion = vkFeatures11_.samplerYcbcrConversion, // enable if supported
		.shaderDrawParameters = VK_TRUE,
	};
	VkPhysicalDeviceVulkan12Features deviceFeatures12 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
		.pNext = &deviceFeatures11,
		.drawIndirectCount = vkFeatures12_.drawIndirectCount, // enable if supported
		.storageBuffer8BitAccess = vkFeatures12_.storageBuffer8BitAccess, // enable if supported
		.uniformAndStorageBuffer8BitAccess = vkFeatures12_.uniformAndStorageBuffer8BitAccess, // enable if supported
		.shaderFloat16 = vkFeatures12_.shaderFloat16, // enable if supported
		.descriptorIndexing = VK_TRUE,
		.shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
		.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
		.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE,
		.descriptorBindingUpdateUnusedWhilePending = VK_TRUE,
		.descriptorBindingPartiallyBound = VK_TRUE,
		.descriptorBindingVariableDescriptorCount = VK_TRUE,
		.runtimeDescriptorArray = VK_TRUE,
		.scalarBlockLayout = VK_TRUE,
		.uniformBufferStandardLayout = VK_TRUE,
		.hostQueryReset = vkFeatures12_.hostQueryReset, // enable if supported
		.timelineSemaphore = VK_TRUE,
		.bufferDeviceAddress = VK_TRUE,
		.vulkanMemoryModel = vkFeatures12_.vulkanMemoryModel, // enable if supported
		.vulkanMemoryModelDeviceScope = vkFeatures12_.vulkanMemoryModelDeviceScope, // enable if supported
	};
	VkPhysicalDeviceVulkan13Features deviceFeatures13 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
		.pNext = &deviceFeatures12,
		.subgroupSizeControl = VK_TRUE,
		.synchronization2 = VK_TRUE,
		.dynamicRendering = VK_TRUE,
		.maintenance4 = VK_TRUE,
	};
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = nextDeviceFeatures;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures10;
	createInfo.enabledLayerCount = static_cast<uint32_t>(_surface.getInstance().GetValidationLayers().size());
	createInfo.ppEnabledLayerNames = _surface.getInstance().GetValidationLayers().data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	Check(vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device), "create logical device");
	if (deviceFeatures12.descriptorBindingPartiallyBound && deviceFeatures12.runtimeDescriptorArray) {
		isBindless = true;
	}

	_debugUtils.SetDevice(_device);
	          
	vkGetDeviceQueue(_device, _graphicsFamilyIndex, 0, &_graphicsQueue);
	vkGetDeviceQueue(_device, _presentFamilyIndex, 0, &_presentQueue);
	vkGetDeviceQueue(_device, _transferFamilyIndex, 0, &_transferQueue);
	
	_physicalDeviceDriverProerties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
	_physicalDeviceDriverProerties.pNext = nullptr;
	_physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	_physicalDeviceProperties2.pNext = &_physicalDeviceDriverProerties;

	
	vkGetPhysicalDeviceProperties2(_physicalDevice, &_physicalDeviceProperties2);
	vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &_memProperties);
	
	const uint32_t apiVersion = _physicalDeviceProperties2.properties.apiVersion;
	LLOGL("Vulakn physical device: %s\n", _physicalDeviceProperties2.properties.deviceName);
	LLOGL("                 API version : %i.%i.%i.%i\n", 
		VK_API_VERSION_MAJOR(apiVersion),							
		VK_API_VERSION_MINOR(apiVersion),							
		VK_API_VERSION_PATCH(apiVersion),							
		VK_API_VERSION_VARIANT(apiVersion));
	LLOGL("                 Driver info : %s %s\n", _physicalDeviceDriverProerties.driverName, _physicalDeviceDriverProerties.driverInfo);

}

vulkan::Device::~Device()
{
	if (_device != nullptr) {
		vkDestroyDevice(_device, nullptr);
		_device = nullptr;
	}
}

void vulkan::Device::WaitIdle() const
{
	Check(vkDeviceWaitIdle(_device), "wait for device idle");
}

void vulkan::Device::CommandBufferSubmit(QueueFamily queue_, uint32_t submit_count_, const VkSubmitInfo* submit_info_, VkFence fence)
{
	switch (queue_)
	{
	case vulkan::QueueFamily::GRAPHIC:
		Check(vkQueueSubmit(_graphicsQueue, 1, submit_info_, VK_NULL_HANDLE),"Command buffer submit");
		vkQueueWaitIdle(_graphicsQueue);
		break;
	case vulkan::QueueFamily::COMPUTE:
		break;
	case vulkan::QueueFamily::TRANSFER:
		break;
	default:
		break;
	}
	
}

void vulkan::Device::CommandBufferSubmit2(QueueFamily queue_, uint32_t submit_count, const VkSubmitInfo2KHR* submit_info_, VkFence fence)
{
	switch (queue_)
	{
	case vulkan::QueueFamily::GRAPHIC:
		Check(vkQueueSubmit2KHR(_graphicsQueue, submit_count, submit_info_, VK_NULL_HANDLE), "Command buffer extension submit");
		break;
	case vulkan::QueueFamily::COMPUTE:
		break;
	case vulkan::QueueFamily::TRANSFER:
		break;
	default:
		break;
	}
	
}

void vulkan::Device::CheckRequiredExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions) 
{
	uint32_t extensionsCount = 0;
	vkEnumerateDeviceExtensionProperties(_physicalDevice, static_cast<const char*>(nullptr), &extensionsCount, nullptr);
	if (extensionsCount)
	{
		_availableExtensions.resize(extensionsCount);
	}
	vkEnumerateDeviceExtensionProperties(_physicalDevice, static_cast<const char*>(nullptr), &extensionsCount, _availableExtensions.data());
	std::set<std::string> required(requiredExtensions.begin(), requiredExtensions.end());

	for (const auto& extension : _availableExtensions)
	{
		required.erase(extension.extensionName);
	}
	if (!required.empty())
	{
		bool first = true;
		std::string extensions;
		for (const auto& extension : required)
		{
			if (!first)
			{
				extensions += ", ";
			}
			extensions += extension;
			first = false;
		}
		throw (std::runtime_error("missing required extensions: " + extensions));
	}
}

vulkan::SwapChain::SwapChain(const Device& device, BufferManager& buffermanager, VkPresentModeKHR presentationMode) : _physicalDevice(device.PhysicalDevice()), _bufferManager(buffermanager),_device(device)
{
	CreateSwapChain(presentationMode);
	CreateDepthResources();
}

void vulkan::SwapChain::CreateSwapChain(VkPresentModeKHR presentationMode)
{
	const auto details = QuerySwapChainSupport(_physicalDevice, _device.VulkanSurface().Handle());
	if (details.Formats.empty() || details.PresentModes.empty())
	{
		throw std::runtime_error("empty swap chain support");
	}

	const auto& surface = _device.VulkanSurface();
	const auto& window = surface.getInstance().getWindow();

	const auto surfaceFormat = ChooseSwapSurfaceFormat(details.Formats);
	const auto actualPresentMode = ChooseSwapPresentMode(details.PresentModes, presentationMode);
	const auto extent = ChooseSwapExtent(window, details.Capabilities);
	const auto imageCount = ChooseImageCount(details.Capabilities);

	VkSwapchainCreateInfoKHR createInfo = {
	.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
	.surface = surface.Handle(),
	.minImageCount = imageCount+1,
	.imageFormat = surfaceFormat.format,
	.imageColorSpace = surfaceFormat.colorSpace,
	.imageExtent = extent,
	.imageArrayLayers = 1,
	.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
	.preTransform = details.Capabilities.currentTransform,
	.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	.presentMode = actualPresentMode,
	.clipped = VK_TRUE,
	.oldSwapchain = nullptr };

	//如果渲染和呈现用的是同一个队列族，用 独占（EXCLUSIVE） 模式，性能最好。
	//如果它们在不同的队列族，就用 并发（CONCURRENT） 模式，并把两条队列族索引都告诉 Vulkan，让它自动帮你管理图像的访问与同步。
	if (_device.GraphicsFamilyIndex() != _device.PresentFamilyIndex())
	{
		uint32_t queueFamilyIndices[] = { _device.GraphicsFamilyIndex(), _device.PresentFamilyIndex() };
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	Check(vkCreateSwapchainKHR(_device.Handle(), &createInfo, nullptr, &_swapChain), "create swap chain!");

	uint32_t vkimageCount;
	vkGetSwapchainImagesKHR(_device.Handle(), _swapChain, &vkimageCount, nullptr);
	if (vkimageCount)
	{
		_images.resize(vkimageCount);
		vkGetSwapchainImagesKHR(_device.Handle(), _swapChain, &vkimageCount, _images.data());
	}
	else
	{
		throw std::runtime_error("unable find vk image");
	}
	_imageViews.resize(_images.size());
	for (size_t i = 0; i < _imageViews.size(); i++)
	{
		/*VkImageViewCreateInfo createInfo{ };
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			createInfo.image = _images[i],
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D,
			createInfo.format = surfaceFormat.format,
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		Check(vkCreateImageView(_device.Handle(), &createInfo, nullptr, &_imageViews[i]), "Create image view");*/
		_bufferManager.CreateImageView(_imageViews[i],_images[i],surfaceFormat.format,VK_IMAGE_ASPECT_COLOR_BIT,1);
	}

	const auto& debugUtils = _device.DebugUtils();

	for (size_t i = 0; i != _images.size(); i++)
	{
		debugUtils.SetObjectName(_images[i], ("Swapchain Image #" + std::to_string(i)).c_str());
		debugUtils.SetObjectName(_imageViews[i], ("Swapchain ImageView #" + std::to_string(i)).c_str());
	}

	_colorFormat = surfaceFormat.format;
	_extent = extent;
}

void vulkan::SwapChain::CreateFrameBuffer(VkRenderPass renderPass)
{
	_swapChainFramebuffers.resize(_imageViews.size());
	auto ImageViewSize = _imageViews.size();
	for (size_t i = 0; i < ImageViewSize; i++)
	{
		std::array<VkImageView, 2> attachments = {
		_imageViews[i],
		_depthImageView
		};
		
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount =static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = _extent.width;
		framebufferInfo.height = _extent.height;
		framebufferInfo.layers = 1;
		Check(vkCreateFramebuffer(_device.Handle(), &framebufferInfo, nullptr, &_swapChainFramebuffers[i]), "Create framebuffer!");

	}
}

void vulkan::SwapChain::CleanUpSwapChain()
{
	vkDestroyImageView(_device.Handle(), _depthImageView, nullptr);
	vkDestroyImage(_device.Handle(), _depthImage, nullptr);
	vkFreeMemory(_device.Handle(), _depthImageMemory, nullptr);
	for (size_t i = 0; i < _swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(_device.Handle(), _swapChainFramebuffers[i], nullptr);
	}

	for (size_t i = 0; i < _imageViews.size(); i++) {
		vkDestroyImageView(_device.Handle(), _imageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(_device.Handle(), _swapChain, nullptr);
	_swapChain = nullptr;
}

vulkan::SwapChain::~SwapChain()
{

	if (_swapChain != nullptr)
	{
		vkDestroySwapchainKHR(_device.Handle(), _swapChain, nullptr);
		_swapChain = nullptr;
	}
}

vulkan::SwapChain::SupportDetails vulkan::SwapChain::QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	SupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.Capabilities);
	uint32_t surfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
	if (surfaceFormatCount)
	{
		details.Formats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, details.Formats.data());
	}
	uint32_t surfacePresentModesCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModesCount, nullptr);
	if (surfacePresentModesCount)
	{
		details.PresentModes.resize(surfacePresentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModesCount, details.PresentModes.data());
	}
	
	return details;
}

VkSurfaceFormatKHR vulkan::SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& format : formats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}
	throw std::runtime_error("found no suitable surface format");
}

VkPresentModeKHR vulkan::SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes, VkPresentModeKHR presentMode)
{
	switch (presentMode)
	{
	case VK_PRESENT_MODE_IMMEDIATE_KHR:
		if (std::find(presentModes.begin(), presentModes.end(), presentMode) != presentModes.end())
		{
			return presentMode;
		}
		break;
	case VK_PRESENT_MODE_MAILBOX_KHR:
		break;
	case VK_PRESENT_MODE_FIFO_KHR:
		break;
	case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
		if (std::find(presentModes.begin(), presentModes.end(), presentMode) != presentModes.end())
		{
			return presentMode;
		}
		break;
	case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
		break;
	case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
		break;
	case VK_PRESENT_MODE_FIFO_LATEST_READY_EXT:
		break;
	case VK_PRESENT_MODE_MAX_ENUM_KHR:
		break;
	default:
		throw std::out_of_range("unknow present mode");
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D vulkan::SwapChain::ChooseSwapExtent(void* window, const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	int width, height;
	SDL_Window* sdl_window = (SDL_Window*)window;
	SDL_GetWindowSize(sdl_window, &width, &height);
	VkExtent2D actualExtent = VkExtent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
	actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
	actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

	return actualExtent;
}

uint32_t vulkan::SwapChain::ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
{
	uint32_t imageCount = std::max(2u, capabilities.minImageCount);
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
	{
		imageCount = capabilities.maxImageCount;
	}
	return imageCount;
}

void vulkan::SwapChain::CreateDepthResources()
{
	auto findDepthFormat = [this]()->VkFormat {
		return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		};
	VkFormat depthFormat = findDepthFormat();
	_depthFormat = depthFormat;
	_bufferManager.createImageBuffer(_extent.width, _extent.height,1, depthFormat, 
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		&_depthImage, &_depthImageMemory);
	 _bufferManager.CreateImageView(_depthImageView,_depthImage, depthFormat,VK_IMAGE_ASPECT_DEPTH_BIT,1);
	_bufferManager.transitionImageLayout(_depthImage, depthFormat,1, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, QueueFamily::GRAPHIC);
	
}

VkFormat vulkan::SwapChain::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &props);
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}



vulkan::RenderPass::RenderPass(const SwapChain& swapchain) : _swapChain(swapchain)
{
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = _swapChain.GetDepthStencilFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = _swapChain.GetSwaphchainImageFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//关于stencil data的操作 深度缓冲等等
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	//在swapchian 里面的image需要被转换成适合的layouts
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//创建subpass，一个render pass有一个或多个subp ass，用于存储渲染操作
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	//这里指向我们的subpass，从外部进来，转换到我们的subpass的layout
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	//开始创建renderPass
	std::vector<VkAttachmentDescription> attachments;
	attachments.push_back(colorAttachment);
	attachments.push_back(depthAttachment);
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	Check(vkCreateRenderPass(_swapChain.GetDevice().Handle(), &renderPassInfo, nullptr, &_renderPass), "Create Render Pass");
}

vulkan::RenderPass::~RenderPass()
{

}

void vulkan::RenderPass::Destroy()
{
	vkDestroyRenderPass(_swapChain.GetDevice().Handle(), _renderPass, nullptr);
}


/////////////////////////////////////////////////////////////////////////GRAPHIC PIPELINE//////////////////////////////////////////////////////////////

vulkan::GraphicPipelineManager::GraphicPipelineManager(const std::unordered_map<std::string, asset::shader>& shaders, VkDevice device, const SwapChain& swapchain, const RenderPass& renderpass)
	: _shaders(shaders), _device(device), _swapChain(swapchain),_renderPass(renderpass)
{
	createPipelineCache();
}

void vulkan::GraphicPipelineManager::Destroy(std::string pipelineName)
{

	
}

VkPipeline vulkan::GraphicPipelineManager::GetGraphicsPipeline(std::string pipelineName)
{
	return utils::findInMap(_pipelines, pipelineName);
}



VkPipelineLayout vulkan::GraphicPipelineManager::GetGraphicsPipelineLayout(std::string piplineLayoutName)
{
	return utils::findInMap(_pipelineLayouts, piplineLayoutName);
}

void vulkan::GraphicPipelineManager::CreateGraphicsPipeline(std::string pipelineName, VkPipelineLayout layout, const asset::shader& shaders, VkRenderPass renderPass)
{
	graphicsPipelineCreateInfoPack createinfo;
	VkPipeline pipeline;

	PipelineEntry entry;
	//创建Shader Module
	VkShaderModule vertShaderModule = CreateShaderModule(shaders.vertexShader);
	VkShaderModule fragShaderModule = CreateShaderModule(shaders.fragmentShader);
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	createinfo.shaderStages.emplace_back(vertShaderStageInfo);
	createinfo.shaderStages.emplace_back(fragShaderStageInfo);

	//顶点输入，用来输入到pipeline中的vertex shader的vertex信息
	//主要两个作用， 绑定顶点中的间隔的距离，还有他们属性的类型以及偏移
	//TODO 抽象出来，要弄几个弄几个

	getPipelineVertexInputState({ VertexComponent::Position, VertexComponent::Color, VertexComponent::UV }, createinfo);

	//规定图元类型，如三角形，线，点等
	createinfo.inputAssemblyStateCi.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	createinfo.inputAssemblyStateCi.primitiveRestartEnable = VK_FALSE;


	//创建动态状态
	createinfo.dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
	createinfo.dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);


	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)_swapChain.GetSwapchainExtent().width;
	viewport.height = (float)_swapChain.GetSwapchainExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	createinfo.viewports.push_back(viewport);

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = _swapChain.GetSwapchainExtent();

	createinfo.scissors.push_back(scissor);

	//光栅器
	//The rasterizer takes the geometry that is shaped by the vertices from the vertex shader and turns it into fragments to be colored by the fragment shader. 
	//It also performs depth testing, face culling and the scissor test,
	//and it can be configured to output fragments that fill entire polygons or just the edges (wireframe rendering). All this is
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	createinfo.rasterizationStateCi = rasterizer;


	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	createinfo.multisampleStateCi = multisampling;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	createinfo.colorBlendAttachmentStates.push_back(colorBlendAttachment);

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	createinfo.colorBlendStateCi = colorBlending;
		
	createinfo.createInfo.layout = layout;
	createinfo.createInfo.renderPass = renderPass;
	createinfo.createInfo.stageCount = 2;

	std::string pipelineNameInfo = "Create pipeline for ";
	pipelineNameInfo += pipelineName;

	createinfo.depthStencilStateCi.depthTestEnable = VK_TRUE;
	createinfo.depthStencilStateCi.depthWriteEnable = VK_TRUE;
	createinfo.depthStencilStateCi.depthCompareOp = VK_COMPARE_OP_LESS;
	createinfo.depthStencilStateCi.depthBoundsTestEnable = VK_FALSE;
	createinfo.depthStencilStateCi.minDepthBounds = 0.0f;
	createinfo.depthStencilStateCi.maxDepthBounds = 1.0f;
	createinfo.depthStencilStateCi.stencilTestEnable = VK_FALSE;
	createinfo.depthStencilStateCi.front = {};
	createinfo.depthStencilStateCi.back = {};

	createinfo.UpdateAllArrays();
	Check(vkCreateGraphicsPipelines(_device, _pipelineCache, 1, &createinfo.createInfo, nullptr, &pipeline), pipelineNameInfo.c_str());
	_pipelines.emplace(pipelineName, pipeline);
	entry.pipeline = pipelineName;
	_pipelineEntrys[pipelineName] = entry;

	vkDestroyShaderModule(_device, fragShaderModule, nullptr);
	vkDestroyShaderModule(_device, vertShaderModule, nullptr);

}

vulkan::GraphicPipelineManager::~GraphicPipelineManager()
{
	for (auto& x : _pipelines) {
		vkDestroyPipeline(_device, x.second, nullptr);
	}
	for (auto& x : _pipelineLayouts) {
		vkDestroyPipelineLayout(_device, x.second, nullptr);
	}
	vkDestroyPipelineCache(_device, _pipelineCache, nullptr);
	
	
}

VkShaderModule vulkan::GraphicPipelineManager::CreateShaderModule(const std::vector<char>& code)
{
	
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule shaderModule;
	Check(vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule), "Create shader Module");
	return shaderModule;
	
}

void vulkan::GraphicPipelineManager::createPipelineCache()
{
	VkPipelineCacheCreateInfo cacheInfo{};
	cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	cacheInfo.initialDataSize = 0;
	cacheInfo.pInitialData = nullptr;
	Check(vkCreatePipelineCache(_device, &cacheInfo, nullptr, &_pipelineCache), "Create Pipeline Cache!");
}

VkPipeline vulkan::GraphicPipelineManager::CreateGraphicsPipeline(std::string pipelineName, graphicsPipelineCreateInfoPack& pack,  const asset::shader& shaders)
{
	
	VkPipeline pipeline;
	
	PipelineEntry entry;
	//创建Shader Module
	VkShaderModule vertShaderModule = CreateShaderModule(shaders.vertexShader);
	VkShaderModule fragShaderModule = CreateShaderModule(shaders.fragmentShader);
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	pack.shaderStages[0] = vertShaderStageInfo;
	pack.shaderStages[1] = fragShaderStageInfo;

	std::string pipelineNameInfo = "Create pipeline for ";
	pipelineNameInfo += pipelineName;
	
	
	pack.UpdateAllArrays();
	Check(vkCreateGraphicsPipelines(_device, _pipelineCache, 1,&pack.createInfo, nullptr, &pipeline), pipelineNameInfo.c_str());
	_pipelines.emplace(pipelineName, pipeline);
	entry.pipeline = pipelineName;
	_pipelineEntrys[pipelineName] = entry;

	return pipeline;
	vkDestroyShaderModule(_device, fragShaderModule, nullptr);
	vkDestroyShaderModule(_device, vertShaderModule, nullptr);

}

VkPipeline vulkan::GraphicPipelineManager::DebugPipelineTest(const asset::shader & shaders, VkPipelineLayout layout)
{
	VkPipeline pipeline;
	VkShaderModule vertShaderModule = CreateShaderModule(shaders.vertexShader);
	VkShaderModule fragShaderModule = CreateShaderModule(shaders.fragmentShader);

	VkPipelineShaderStageCreateInfo shaderStages[2]{};

	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = vertShaderModule;
	shaderStages[0].pName = "main";

	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = fragShaderModule;
	shaderStages[1].pName = "main";

	// ---------------------------
	VkPipelineVertexInputStateCreateInfo vertexInput{};
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInput.vertexBindingDescriptionCount = 0;
	vertexInput.vertexAttributeDescriptionCount = 0;

	// ---------------------------
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_NONE;     // critical for fullscreen tri
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.lineWidth = 1.0f;
	// ---------------------------
	VkPipelineMultisampleStateCreateInfo multisample{};
	multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// ---------------------------
	 // ---------------------------
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;  // must enable all
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlend{};
	colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlend.attachmentCount = 1;
	colorBlend.pAttachments = &colorBlendAttachment;

	VkPipelineDepthStencilStateCreateInfo depth{};
	depth.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth.depthTestEnable = VK_FALSE;
	depth.depthWriteEnable = VK_FALSE;

	VkDynamicState dynamicStates[] = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic{};
	dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic.dynamicStateCount = 2;
	dynamic.pDynamicStates = dynamicStates;
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInput;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisample;
	pipelineInfo.pColorBlendState = &colorBlend;
	pipelineInfo.pDepthStencilState = &depth;
	pipelineInfo.pDynamicState = &dynamic;

	pipelineInfo.layout = layout;
	pipelineInfo.renderPass = _renderPass.GetRenderPass();
	pipelineInfo.subpass = 0;
	vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
	return pipeline;
}





/////////////////////////////////////////////////////////////////////////COMMAND POOL//////////////////////////////////////////////////////////////

vulkan::CommandPoolManager::CommandPoolManager(const Device& device) : _device(device)
{
	Init(4);
}

vulkan::CommandPoolManager::~CommandPoolManager()
{
	for (auto& pool : _commandPools)
	{
		vkDestroyCommandPool(_device.Handle(), pool.second, nullptr);
	}
}

void vulkan::CommandPoolManager::CreateCommandPool(QueueFamily family)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	switch (family)
	{
	case QueueFamily::GRAPHIC:
		poolInfo.queueFamilyIndex = _device.GraphicsFamilyIndex();
		break;
	case QueueFamily::COMPUTE:
		break;
	case QueueFamily::TRANSFER:
		poolInfo.queueFamilyIndex = _device.TransferFamilyIndex();
		break;
	default:
		break;
	}
	Check(vkCreateCommandPool(_device.Handle(), &poolInfo, nullptr, &_commandPools[family]), "Create Command pool");
}

void vulkan::CommandPoolManager::FreeCommandBuffer(QueueFamily family, uint32_t count, const VkCommandBuffer& buffer)
{
	vkFreeCommandBuffers(_device.Handle(), _commandPools[family], count, &buffer);
}



void vulkan::CommandPoolManager::Init(uint32_t num_threads)
{
	const uint32_t totalPool = num_threads * MAX_FRAMES_IN_FLIGHT;

	CreateCommandPool(QueueFamily::GRAPHIC);
	CreateCommandPool(QueueFamily::TRANSFER);
}

vulkan::DescriptorLayoutManager::DescriptorLayoutManager(const Device& deivce) : _device(deivce)
{
	

}

vulkan::DescriptorLayoutManager::~DescriptorLayoutManager()
{
	for (auto& entry : _LayoutCache)
	{
		vkDestroyDescriptorSetLayout(_device.Handle(), entry.second, nullptr);
	}
}

VkDescriptorSetLayout vulkan::DescriptorLayoutManager::CreateDescriptorSetLayout(LayoutConfig config)
{
	auto it = _LayoutCache.find(config);
	if (it == _LayoutCache.end()){
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = config.bindingCounts;
		layoutInfo.pBindings = config.bindings.data();
		Check(vkCreateDescriptorSetLayout(_device.Handle(), &layoutInfo, nullptr, &_LayoutCache[config]),"Create DescriptorSetLayout");
	}
	return _LayoutCache[config];
	
}

vulkan::DescriptorPoolManager::DescriptorPoolManager(const Device& deivce) : _device(deivce)
{
	CreateGlobalDescriptorPool();
	CreatePerFrameDescriptorPool();
}

vulkan::DescriptorPoolManager::~DescriptorPoolManager()
{
	vkDestroyDescriptorPool(_device.Handle(), _GlobalPool, nullptr);
	for (auto& pool: _PerFramePool)
	{
		vkDestroyDescriptorPool(_device.Handle(), pool, nullptr);
	}
	
}

void vulkan::DescriptorPoolManager::CreateGlobalDescriptorPool()
{
	VkDescriptorPoolSize globalPoolSizes[] = {
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 50 }, };
	VkDescriptorPoolCreateInfo globalPoolInfo{};
	globalPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	globalPoolInfo.poolSizeCount = 1;
	globalPoolInfo.pPoolSizes = globalPoolSizes;
	globalPoolInfo.maxSets = 50;

	Check(vkCreateDescriptorPool(_device.Handle(), &globalPoolInfo, nullptr, &_GlobalPool), "Create GlobalDescriptorPool");
}

void vulkan::DescriptorPoolManager::CreateGlobalDescriptorPool(const GpuDescriptorPoolCreation& pool_creation)
{
	VkDescriptorPoolSize pool_sizes[] = {
		{ VK_DESCRIPTOR_TYPE_SAMPLER, pool_creation.samplers },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, pool_creation.combined_image_samplers },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, pool_creation.sampled_image },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, pool_creation.storage_image },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, pool_creation.uniform_texel_buffers },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, pool_creation.storage_texel_buffers },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, pool_creation.uniform_buffer },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, pool_creation.storage_buffer },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, pool_creation.uniform_buffer_dynamic },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, pool_creation.storage_buffer_dynamic },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, pool_creation.input_attachments }
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 4096;
	pool_info.poolSizeCount = (uint32_t)ArraySize(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	Check(vkCreateDescriptorPool(_device.Handle(), &pool_info, nullptr, &_GlobalPool), "create globle pool");
}

void vulkan::DescriptorPoolManager::CreatePerFrameDescriptorPool()
{
	_PerFramePool.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		std::vector<VkDescriptorPoolSize> poolSizes = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000}
		};
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1000;

		Check(vkCreateDescriptorPool(_device.Handle(), &poolInfo, nullptr, &_PerFramePool[i]), "Create per frame descriptor pool");
	}
}

void vulkan::DescriptorPoolManager::CreatePreFrameDescriptorSets(std::vector<VkDescriptorSetLayout>& layouts){
	_hardCodeDescriptorSet.resize(MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _PerFramePool[i];
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();
		Check(vkAllocateDescriptorSets(_device.Handle(), &allocInfo, _hardCodeDescriptorSet.data()), "Allocate Descriptor set");
	}

}

void vulkan::DescriptorPoolManager::CreatePoolForIndividualObject(uint32_t uboCount, uint32_t imageCount, std::string objectName)
{
	std::vector<VkDescriptorPoolSize> poolSize = {
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uboCount},
	};
	//TODO:判断类型
	if (imageCount > 0) {
		poolSize.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, imageCount });
	}
	VkDescriptorPool pool;
	VkDescriptorPoolCreateInfo descriptorPoolCi{};
	descriptorPoolCi.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCi.poolSizeCount = static_cast<uint32_t>(poolSize.size());
	descriptorPoolCi.pPoolSizes = poolSize.data();
	descriptorPoolCi.maxSets = uboCount + imageCount;
	Check(vkCreateDescriptorPool(_device.Handle(), &descriptorPoolCi, nullptr, &pool), "Create individual Pool");
	_test_pool.emplace(objectName, pool);
}

void vulkan::DescriptorPoolManager::AllocateDescriptorSet(VkDescriptorSetLayout& layout,VkDescriptorType type, VkDescriptorSet& desSet, uint32_t binding, std::vector<VkDescriptorBufferInfo> desInfo, int index)
{
	
	VkDescriptorSetAllocateInfo allocInfo = initializers::descriptorSetAllocateInfo(_PerFramePool[index], &layout, 1);
	Check(vkAllocateDescriptorSets(_device.Handle(), &allocInfo, &desSet), "Allocate Descriptor Set");
	std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	writeDescriptorSets.resize(desInfo.size());
	for (uint32_t i = 0; i < desInfo.size(); i++) {

		 writeDescriptorSets[i] = initializers::writeDescriptorSet(desSet, type, i, &desInfo[i]);
	}
	vkUpdateDescriptorSets(_device.Handle(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	

}

void vulkan::DescriptorPoolManager::AllocateImageDescriptorSet(Vulkan_Material& material, std::vector<Vulkan_Texture>& textures,VkDescriptorSetLayout layout)
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
		descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocInfo.descriptorPool = _PerFramePool[i];
		descriptorSetAllocInfo.pSetLayouts = &layout;
		descriptorSetAllocInfo.descriptorSetCount = 1;
		Check(vkAllocateDescriptorSets(_device.Handle(), &descriptorSetAllocInfo, &material.descriptorSet), "Create material DescriptorSet");
		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
			initializers::writeDescriptorSet(material.descriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, &textures[material.baseColorTexture].descriptor),
			initializers::writeDescriptorSet(material.descriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &textures[material.normalTexture].descriptor)
		};
	
		vkUpdateDescriptorSets(_device.Handle(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}

}

void vulkan::DescriptorPoolManager::PrepareNodeDescriptor(SceneNode* node, VkDescriptorSetLayout descriptorSetlayout)
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (!node->mesh.empty()) {
			for (auto& x : node->mesh) {
				VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
				descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				descriptorSetAllocInfo.descriptorPool = _PerFramePool[i];
				descriptorSetAllocInfo.pSetLayouts = &descriptorSetlayout;
				descriptorSetAllocInfo.descriptorSetCount = 1;
				Check(vkAllocateDescriptorSets(_device.Handle(), &descriptorSetAllocInfo, &x->uniformBuffer.descriptorSet), "Allocate descriptor set");

				VkWriteDescriptorSet writeDescriptorSet{};
				writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				writeDescriptorSet.descriptorCount = 1;
				writeDescriptorSet.dstSet = x->uniformBuffer.descriptorSet;
				writeDescriptorSet.dstBinding = 0;
				writeDescriptorSet.pBufferInfo = &x->uniformBuffer.descriptor;

				vkUpdateDescriptorSets(_device.Handle(), 1, &writeDescriptorSet, 0, nullptr);
			}
			
		}
		for (auto& child : node->children) {
			PrepareNodeDescriptor(child, descriptorSetlayout);
		}
	}

}

vulkan::BufferManager::BufferManager(const Device& deivce, CommandPoolManager& commandPools) : device(deivce), commandPools(commandPools)
{

}

vulkan::BufferManager::~BufferManager()
{
}

void vulkan::BufferManager::CreateIndexBuffer1(std::vector<uint32_t>& indiceData, VkBuffer& buffer, VkDeviceMemory& memory)
{
	VkDeviceSize bufferSize = sizeof(indiceData[0]) * indiceData.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		bufferSize,
		&stagingBuffer,
		&stagingBufferMemory,
		indiceData.data());
	createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		&buffer,
		&memory);
	copyBuffer(stagingBuffer, buffer, bufferSize, QueueFamily::TRANSFER);
	vkDestroyBuffer(device.Handle(), stagingBuffer, nullptr);
	vkFreeMemory(device.Handle(), stagingBufferMemory, nullptr);
}

void vulkan::BufferManager::CreateVertexBuffer1(std::vector<Vertex>& vertexData, VkBuffer& buffer, VkDeviceMemory& memory)
{
	VkDeviceSize bufferSize = sizeof(vertexData[0]) * vertexData.size();
	//创建一个临时缓冲区，用于直接传输顶点数据至GPU
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		bufferSize,
		&stagingBuffer,
		&stagingBufferMemory,
		vertexData.data());
	createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		&buffer,
		&memory);
	copyBuffer(stagingBuffer, buffer, bufferSize, QueueFamily::TRANSFER);
	vkDestroyBuffer(device.Handle(), stagingBuffer, nullptr);
	vkFreeMemory(device.Handle(), stagingBufferMemory, nullptr);
}

void vulkan::BufferManager::CreateVulkanImageBuffer(const Image& image_data, VkImageLayout& image_layout, VkImage& image, VkDeviceMemory& memory)
{
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VkDeviceSize image_size = image_data.texWidth * image_data.texHeight * 4;
	VkFormat format = FromFormat(image_data.format);
	createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		image_data.size,
		&stagingBuffer,
		&stagingBufferMemory,
		image_data.pixel);
	createImageBuffer(image_data.texWidth, image_data.texHeight, image_data.mipLevels,
		format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&image,
		&memory);

	image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	transitionImageLayout(image, format,image_data.mipLevels, image_layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, QueueFamily::TRANSFER);
	//transitionImageLayout1(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	image_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	copyBufferToImage(stagingBuffer, image, image_data.subresource,static_cast<uint32_t>(image_data.texWidth), static_cast<uint32_t>(image_data.texHeight), QueueFamily::TRANSFER);
	//copyBufferToImage1(stagingBuffer, image, static_cast<uint32_t>(image_data.texWidth), static_cast<uint32_t>(image_data.texHeight));
	transitionImageLayout(image, format, image_data.mipLevels, image_layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, QueueFamily::GRAPHIC);
	//transitionImageLayout1(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	vkDestroyBuffer(device.Handle(), stagingBuffer, nullptr);
	vkFreeMemory(device.Handle(), stagingBufferMemory, nullptr);

}

void vulkan::BufferManager::createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* memory, void* data)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	bufferInfo.queueFamilyIndexCount = 2;
	uint32_t families[] = { device.GraphicsFamilyIndex(), device.TransferFamilyIndex() };
	bufferInfo.pQueueFamilyIndices = families;
	Check(vkCreateBuffer(device.Handle(), &bufferInfo, nullptr, buffer), "Create buffer!");
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device.Handle(), *buffer, &memRequirements);
	VkMemoryAllocateInfo alloInfo{};
	alloInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloInfo.allocationSize = memRequirements.size;
	alloInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);
	//这玩意光追管线得开，让shader直接访问gpu内存
	VkMemoryAllocateFlagsInfoKHR allocFlagsInfo{};
	if (usageFlags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
		allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
		allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
		alloInfo.pNext = &allocFlagsInfo;
	}
	Check(vkAllocateMemory(device.Handle(), &alloInfo, nullptr, memory), "Allocate memory");

	if (data != nullptr) {
		void* mapped;
		Check(vkMapMemory(device.Handle(), *memory, 0, size, 0, &mapped),"Map memory");
		memcpy(mapped, data, size);
		if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
			VkMappedMemoryRange mappedRange{};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.offset = 0;
			mappedRange.size = size;
			mappedRange.memory = *memory;
			vkFlushMappedMemoryRanges(device.Handle(), 1, &mappedRange);
		}
		vkUnmapMemory(device.Handle(), *memory);
	}
	
	Check(vkBindBufferMemory(device.Handle(), *buffer, *memory, 0), "Bind Memory");
}



void vulkan::BufferManager::createImageBuffer(
	uint32_t width,uint32_t height, uint32_t mipLevels,
	VkFormat format, VkImageTiling tiling, 
	VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	Check(vkCreateImage(device.Handle(), &imageInfo, nullptr,image), "create vk image");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device.Handle(), *image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
	Check(vkAllocateMemory(device.Handle(), &allocInfo, nullptr, imageMemory), "allocate image memory");

	vkBindImageMemory(device.Handle(), *image, *imageMemory, 0);


	

}

void vulkan::BufferManager::transitionImageLayout(VkImage image, VkFormat format, uint32_t miplevel, VkImageLayout oldLayout, VkImageLayout newLayout, QueueFamily family)
{
	VkCommandBuffer cmdBuffer = createInstantCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandPools.GetCommandPool(family), 1, true);
	VkImageMemoryBarrier barrier{};
	VkPipelineStageFlags sourceStage = 65536U;
	VkPipelineStageFlags destinationStage = 65536U;
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.levelCount = miplevel;
	barrier.subresourceRange.layerCount = 1;
	switch (oldLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		// Image layout is undefined (or does not matter)
		// Only valid as initial layout
		// No flags required, listed only for completeness
		barrier.srcAccessMask = 0;
		
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		// Image is preinitialized
		// Only valid as initial layout for linear images, preserves memory contents
		// Make sure host writes have been finished
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image is a color attachment
		// Make sure any writes to the color buffer have been finished
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image is a depth/stencil attachment
		// Make sure any writes to the depth/stencil buffer have been finished
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image is a transfer source
		// Make sure any reads from the image have been finished
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image is a transfer destination
		// Make sure any writes to the image have been finished
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image is read by a shader
		// Make sure any shader reads from the image have been finished
		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}
	switch (newLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image will be used as a transfer destination
		// Make sure any writes to the image have been finished
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image will be used as a transfer source
		// Make sure any reads from the image have been finished
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image will be used as a color attachment
		// Make sure any writes to the color buffer have been finished
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image layout will be used as a depth/stencil attachment
		// Make sure any writes to depth/stencil buffer have been finished
		barrier.dstAccessMask = barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image will be read in a shader (sampler, input attachment)
		// Make sure any writes to the image have been finished
		if (barrier.srcAccessMask == 0)
		{
			barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (hasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	vkCmdPipelineBarrier(
		cmdBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
	if (family == QueueFamily::GRAPHIC) {
		flushCommandBuffer(cmdBuffer, device.GraphicsQueue(), commandPools.GetCommandPool(family), true);
	}
	else
	{
		flushCommandBuffer(cmdBuffer, device.TransferQueue(), commandPools.GetCommandPool(family), true);

	}

}

void vulkan::BufferManager::setImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask)
{
	// Create an image barrier object
	VkImageMemoryBarrier imageMemoryBarrier = initializers::imageMemoryBarrier();
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (oldImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		// Image layout is undefined (or does not matter)
		// Only valid as initial layout
		// No flags required, listed only for completeness
		imageMemoryBarrier.srcAccessMask = 0;
	
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		// Image is preinitialized
		// Only valid as initial layout for linear images, preserves memory contents
		// Make sure host writes have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image is a color attachment
		// Make sure any writes to the color buffer have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image is a depth/stencil attachment
		// Make sure any writes to the depth/stencil buffer have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image is a transfer source
		// Make sure any reads from the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image is a transfer destination
		// Make sure any writes to the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image is read by a shader
		// Make sure any shader reads from the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Target layouts (new)
	// Destination access mask controls the dependency for the new image layout
	switch (newImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image will be used as a transfer destination
		// Make sure any writes to the image have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image will be used as a transfer source
		// Make sure any reads from the image have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image will be used as a color attachment
		// Make sure any writes to the color buffer have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image layout will be used as a depth/stencil attachment
		// Make sure any writes to depth/stencil buffer have been finished
		imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image will be read in a shader (sampler, input attachment)
		// Make sure any writes to the image have been finished
		if (imageMemoryBarrier.srcAccessMask == 0)
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Put barrier inside setup command buffer
	vkCmdPipelineBarrier(
		cmdbuffer,
		srcStageMask,
		dstStageMask,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);
}

bool vulkan::BufferManager::hasStencilComponent(VkFormat format)
{
	
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	
}

void vulkan::BufferManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, QueueFamily family)
{
	VkCommandBuffer cmdBuffer = createInstantCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandPools.GetCommandPool(family), 1, true);
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, & copyRegion);
	flushCommandBuffer(cmdBuffer, device.TransferQueue(), commandPools.GetCommandPool(family), true);

}

void vulkan::BufferManager::copyBufferToImage(VkBuffer buffer, VkImage image, std::vector<SubResource> subResource, uint32_t width, uint32_t height, QueueFamily family)
{
	VkCommandBuffer cmdBuffer = createInstantCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandPools.GetCommandPool(family), 1, true);

	std::vector<VkBufferImageCopy> bufferCopyRegions;
	for (uint32_t i = 0; i < subResource.size(); i++)
	{
		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyRegion.imageSubresource.mipLevel = i;
		bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageExtent.width = std::max(1u, width >> i);
		bufferCopyRegion.imageExtent.height = std::max(1u, height >> i);
		bufferCopyRegion.imageExtent.depth = 1;
		bufferCopyRegion.bufferOffset =	subResource[i].offset;

		bufferCopyRegions.push_back(bufferCopyRegion);
	}

	vkCmdCopyBufferToImage(
		cmdBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		static_cast<uint32_t>(bufferCopyRegions.size()),
		bufferCopyRegions.data()
	);
	flushCommandBuffer(cmdBuffer, device.TransferQueue(), commandPools.GetCommandPool(family), true);
}

VkCommandBuffer vulkan::BufferManager::createInstantCommandBuffer(VkCommandBufferLevel level, VkCommandPool pool, uint32_t bufferCount, bool begin)
{
	
	VkCommandBuffer cmdBuffer;
	VkCommandBufferAllocateInfo cmdBufferAllocateInfo{};
	cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocateInfo.commandPool = pool;
	cmdBufferAllocateInfo.commandBufferCount = bufferCount;
	cmdBufferAllocateInfo.level = level;
	Check(vkAllocateCommandBuffers(device.Handle(), &cmdBufferAllocateInfo, &cmdBuffer),"Allocate CommandBuffer");
	if (begin) {
		VkCommandBufferBeginInfo cmdBeginInfo{};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		Check(vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo), "Begin record buffer");
	}
	return cmdBuffer;

}

uint32_t vulkan::BufferManager::findMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound) const
{
	VkPhysicalDeviceMemoryProperties memProperty= device.GetPhyDeviceMemProperty();
	for (uint32_t i = 0; i < memProperty.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			if ((memProperty.memoryTypes[i].propertyFlags & properties) == properties) {
				if (memTypeFound) {
					*memTypeFound = true;
				}
				return i;
			}
		}
		typeBits >>= 1;
	}
	if (memTypeFound) {
		*memTypeFound = false;
		return 0;
	}
	else
	{
		throw std::runtime_error("Cound not find a matching memory type");
	}

}


void vulkan::BufferManager::CreateTextureSampler(VkSampler& sampler, uint32_t mipLevels)
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerInfo.maxAnisotropy = 1.0;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxLod = (float)mipLevels;
	samplerInfo.maxAnisotropy = 8.0f;
	samplerInfo.anisotropyEnable = VK_TRUE;
	Check(vkCreateSampler(device.Handle(), &samplerInfo, nullptr, &sampler), "Create Sampler");

}

void vulkan::BufferManager::CreateImageView(VkImageView& view, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.layerCount = 1;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.flags = 0;
	Check(vkCreateImageView(device.Handle(), &viewInfo, nullptr, &view), "Create imageView");
	
}



void vulkan::BufferManager::DestroyBuffer(VkBuffer buffer, VkDeviceMemory memory)
{
	vkFreeMemory(device.Handle(), memory, nullptr);
	vkDestroyBuffer(device.Handle(), buffer, nullptr);
}

void vulkan::BufferManager::DestroyVkImage(VkImage image, VkDeviceMemory memory)
{
	vkDestroyImage(device.Handle(), image, nullptr);
	vkFreeMemory(device.Handle(), memory, nullptr);
}

void vulkan::BufferManager::DestroyVkImageView(VkImageView imageview)
{
	vkDestroyImageView(device.Handle(), imageview,nullptr);
}

void vulkan::BufferManager::DestroySampler(VkSampler sampler)
{
	vkDestroySampler(device.Handle(), sampler, nullptr);
}


void vulkan::BufferManager::flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool, bool free)
{
	if (commandBuffer == VK_NULL_HANDLE) {
		return;
	}
	Check(vkEndCommandBuffer(commandBuffer), "end Command buffer");
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = 0;
	VkFence fence;
	Check(vkCreateFence(device.Handle(), &fenceCreateInfo, nullptr, &fence), "Create fence for flush command buffer ");
	Check(vkQueueWaitIdle(queue), "wait queue idle");
	Check(vkQueueSubmit(queue, 1, &submitInfo, fence), "Submit to queue");
	Check(vkWaitForFences(device.Handle(), 1, &fence, VK_TRUE, 100000000000), "Wait for finished excuting commandbuffer");
	vkDestroyFence(device.Handle(), fence, nullptr);
	if (free) {
		
		vkFreeCommandBuffers(device.Handle(), pool, 1, &commandBuffer);
	}


}



vulkan::VulkanResouceManager::VulkanResouceManager(
	BufferManager& bufferManager,
	DescriptorPoolManager& descPoolManager,
	DescriptorLayoutManager& descLayoutManager,
	GraphicPipelineManager& graphicPipelineManager,
	Device& device,
	Instance& instance,
	const asset::AssetManager& assetManager) :
	_BufferManager(bufferManager),
	_descriptorPoolManager(descPoolManager),
	_descriptorLayoutManager(descLayoutManager),
	_graphicPipelineManager(graphicPipelineManager),
	_assetMnanger(assetManager),
	_device(device),
	_instance(instance)
{

	CreateDummyTexture();

	
}

vulkan::VulkanResouceManager::~VulkanResouceManager()
{
	for (auto& x : _renderScenes) {
		_BufferManager.DestroyBuffer(x.indiceBuffer, x.indicememory);
		_BufferManager.DestroyBuffer(x.vertexBuffer, x.vertexmemory);
		for (auto& x : x.textures) {
			_BufferManager.DestroyVkImageView(x.view);
			_BufferManager.DestroyVkImage(x.image, x.deviceMemory);
			_BufferManager.DestroySampler(x.sampler);
		}
	}
}

void vulkan::VulkanResouceManager::init()
{

}

void vulkan::VulkanResouceManager::ConvertToVulkanResource()
{

}

void vulkan::VulkanResouceManager::ConstructVulkanRenderObject(
	std::string name,  
	std::string raw_model_name, 
	std::vector<std::string> textureFiles)
{
	ModelData modeldata = _assetMnanger.getModelDataByName(raw_model_name);
	VulkanRenderScene renderObject;
	
	renderObject.name = name;
	
	for (const auto& x : modeldata.nodes)
	{
		ConstructSceneNode(nullptr, x, renderObject, modeldata);
	}
	for (const auto& x : textureFiles) {
		const Image& image = _assetMnanger.getImageDataByName(x);

		Vulkan_Texture texture;
		texture.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		_BufferManager.CreateVulkanImageBuffer(image, texture.imageLayout, texture.image, texture.deviceMemory);
		CreateTextureImageView(texture.view, texture.image);
		_BufferManager.CreateTextureSampler(texture.sampler, image.mipLevels);
		texture.updateDescriptor();
		renderObject.textures.push_back(texture);
	}


	//上传vertex indice buffer数据
	{
		_BufferManager.CreateVertexBuffer1(modeldata.vertices, renderObject.vertexBuffer, renderObject.vertexmemory);
		_BufferManager.CreateIndexBuffer1 (modeldata.indices, renderObject.indiceBuffer, renderObject.indicememory);
	}
	//生成DescriptorPool
	{
		_descriptorPoolManager.CreatePoolForIndividualObject(1, modeldata.imageCount+1, name);
	}
	//生成vertex shader的关于矩阵变换的layout
	{
		LayoutConfig configVertex{};
		configVertex.bindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0));
		renderObject.descriptorSetLayouts.matrices = _descriptorLayoutManager.CreateDescriptorSetLayout(configVertex);
		
	}
	//生成fragment shader的关于material的layout
	{
		LayoutConfig configFragment{};
		configFragment.bindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0));
		configFragment.bindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1));
		configFragment.UpdateAllArray();
		renderObject.descriptorSetLayouts.textures = _descriptorLayoutManager.CreateDescriptorSetLayout(configFragment);

	}
	

	renderObject.indiceCounts.push_back(static_cast<uint32_t>(modeldata.indices.size()));

	_renderObjectsMapping.emplace(name, static_cast<uint32_t>(_renderScenes.size()));
	_renderScenes.push_back(renderObject);
}

void vulkan::VulkanResouceManager::ConstructVulkanRenderObject(std::string name, std::string raw_model_name)
{
	ModelData modeldata = _assetMnanger.getModelDataByName(raw_model_name);
	VulkanRenderScene renderObject;

	renderObject.name = name;
	//生成VKmesh
	for (const auto& x : modeldata.linearNodeHierarchy) {
		
		if (x->meshID != -1) {
			glm::mat4 m = x->getMatrix();
			const auto& meshdata = modeldata.meshdatas[x->meshID];
			Vulkan_SkinnedMesh* vkmesh = new Vulkan_SkinnedMesh{};
			vkmesh->offset = meshdata.meshes;
			vkmesh->uniformBlock.matrix = meshdata.localTransform;
			_BufferManager.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				sizeof(vkmesh->uniformBlock),
				&vkmesh->uniformBuffer.buffer,
				&vkmesh->uniformBuffer.memory,
				&vkmesh->uniformBlock);
			
			Check(vkMapMemory(_device.Handle(), vkmesh->uniformBuffer.memory, 0, sizeof(vkmesh->uniformBlock), 0, &vkmesh->uniformBuffer.mapped), "vkmap memeory uniform buffer");
			vkmesh->uniformBuffer.descriptor = { vkmesh->uniformBuffer.buffer, 0 , sizeof(vkmesh->uniformBlock) };
			vkmesh->name = meshdata.name;
			memcpy(vkmesh->uniformBuffer.mapped, &m, sizeof(glm::mat4));
			renderObject.meshes.push_back(vkmesh);
		}
		
		

	}
	//生成VkSceneNode
	for (const auto& x : modeldata.nodes)
	{
		ConstructSceneNode(nullptr, x, renderObject, modeldata);
	}
	//生成VkImage
	for (const auto& x : modeldata.images) {
		const Image& image = _assetMnanger.getImageDataByName(x);

		Vulkan_Texture vktexture;
		vktexture.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		_BufferManager.CreateVulkanImageBuffer(image, vktexture.imageLayout, vktexture.image, vktexture.deviceMemory);
		_BufferManager.CreateImageView(vktexture.view, vktexture.image, FromFormat(image.format), VK_IMAGE_ASPECT_COLOR_BIT, image.mipLevels);
		_BufferManager.CreateTextureSampler(vktexture.sampler, image.mipLevels);
		vktexture.height = image.texHeight;
		vktexture.width = image.texWidth;
		vktexture.mipLevels = image.mipLevels;
		vktexture.updateDescriptor();
		vktexture.index = image.id;
		renderObject.textureIdMapping.emplace(image.id, renderObject.textureCount);
		renderObject.textures.push_back(vktexture);
		renderObject.textureCount++;

	}
	//生成DescriptorPool
	{
		_descriptorPoolManager.CreatePoolForIndividualObject(1, modeldata.imageCount + 1, name);
	}
	

	//上传vertex indice buffer数据
	{

		_BufferManager.CreateVertexBuffer1(modeldata.vertices, renderObject.vertexBuffer, renderObject.vertexmemory);
		_BufferManager.CreateIndexBuffer1(modeldata.indices, renderObject.indiceBuffer, renderObject.indicememory);
		
		BufferCreation creation{};
		creation.size = static_cast<uint32_t>(modeldata.vertices.size());

	}
	//生成DescriptorPool
	{
		_descriptorPoolManager.CreatePoolForIndividualObject(1, modeldata.imageCount + 1, name);
	}
	//生成vertex shader的关于矩阵变换的layout
	{
		LayoutConfig configVertex{};
		configVertex.bindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, static_cast<uint32_t>(configVertex.bindingCounts)));
		configVertex.UpdateAllArray();
		configVertex.bindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT ,static_cast<uint32_t>(configVertex.bindingCounts)));
		configVertex.UpdateAllArray();
		renderObject.descriptorSetLayouts.matrices = _descriptorLayoutManager.CreateDescriptorSetLayout(configVertex);
		
	}
	//生成fragment shader的关于material的layout
	{
		LayoutConfig configFragment{};
		configFragment.bindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, static_cast<uint32_t>(configFragment.bindingCounts)));
		configFragment.UpdateAllArray();
		configFragment.bindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, static_cast<uint32_t>(configFragment.bindingCounts)));
		configFragment.UpdateAllArray();
		renderObject.descriptorSetLayouts.textures = _descriptorLayoutManager.CreateDescriptorSetLayout(configFragment);

	}
	//生成Material生成
	for (const auto& x : modeldata.materials) {
		if (x.baseColorTexture != -1) {
			Vulkan_Material material;
			material.alphaCutoff = x.alphaCutoff;
			material.alphaMode = x.alphaMode;
			material.metallicFactor = x.matallicFactor;
			material.roughnessFactor = x.roughnessFactor;
			material.baseColorFactor = x.baseColorFactor;
			material.baseColorTexture = renderObject.textureIdMapping[x.baseColorTexture];
			if (x.normalTexture != -1) {
				material.normalTexture = renderObject.textureIdMapping[x.normalTexture];
			}
			renderObject.materials.push_back(material);
		}
		
	}
	renderObject.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	renderObject.indiceCounts.push_back(static_cast<uint32_t>(modeldata.indices.size()));
	_renderObjectsMapping[name] = static_cast<uint32_t>(_renderScenes.size());
	_renderScenes.push_back(renderObject);
}

void vulkan::VulkanResouceManager::CreateDebugRenderObject(std::string name, std::string raw_model_name, std::vector<std::string> textureFiles)
{
	ModelData modelData = _assetMnanger.getModelDataByName(raw_model_name);
	VulkanRenderObject object;
	for (const auto& mesh : modelData.meshdatas) {
		Vulkan_Mesh vk_mesh;
		vk_mesh.name = mesh.name;
		for (const auto& offset : mesh.meshes) {
			vk_mesh.offset.push_back(offset);
		}
	}
	{
		LayoutConfig uboLayout;
		uboLayout.bindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, uboLayout.bindingCounts));
		uboLayout.UpdateAllArray();
		object.descriptorSetLayouts.matrices = _descriptorLayoutManager.CreateDescriptorSetLayout(uboLayout);
	}
	{
		LayoutConfig materialLayout;
		materialLayout.bindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, materialLayout.bindingCounts));
	}
	//TODO::material
	{
		_BufferManager.CreateVertexBuffer1(modelData.vertices, object.vertexBuffer, object.vertexmemory);
		_BufferManager.CreateIndexBuffer1(modelData.indices, object.indiceBuffer, object.indicememory);
	}
	_renderObjects.push_back(object);
	Vulkan_Material material;
	material.baseColorTexture = 0;


	
}

void vulkan::VulkanResouceManager::CreateDummyTexture()
{
	const Image& dummy_image = _assetMnanger.getImageDataByName("dummy");
	
	_dummy_texture.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	_BufferManager.CreateVulkanImageBuffer(dummy_image, _dummy_texture.imageLayout, _dummy_texture.image, _dummy_texture.deviceMemory);
	_BufferManager.CreateImageView(_dummy_texture.view, _dummy_texture.image, FromFormat(dummy_image.format), VK_IMAGE_ASPECT_COLOR_BIT, dummy_image.mipLevels);
	_BufferManager.CreateTextureSampler(_dummy_texture.sampler, dummy_image.mipLevels);
	_dummy_texture.height = dummy_image.texHeight;
	_dummy_texture.width = dummy_image.texWidth;
	_dummy_texture.mipLevels = dummy_image.mipLevels;
	_dummy_texture.updateDescriptor();
	_dummy_texture.index = dummy_image.id;
	
}

vulkan::VulkanRenderScene& vulkan::VulkanResouceManager::GetRenderObject(std::string name)
{			
			
	return _renderScenes[utils::findInMap(_renderObjectsMapping, name)];
}

VkPipelineShaderStageCreateInfo vulkan::VulkanResouceManager::LoadShader(std::string shader_name, VkShaderStageFlagBits stage)
{
	VkPipelineShaderStageCreateInfo shaderStage = {};
	shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStage.stage = stage;

	VkShaderModule shaderModule;
	const auto& shaders = _assetMnanger.getShaderByName(shader_name);
	VkShaderModuleCreateInfo moduleCreateInfo{};
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	assert(shaders.vertexShader.size());
	moduleCreateInfo.codeSize = shaders.vertexShader.size();
	moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaders.vertexShader.data());
	Check(vkCreateShaderModule(_device.Handle(), &moduleCreateInfo, NULL, &shaderModule), "创建shader模组");
	return VkPipelineShaderStageCreateInfo();
}




void vulkan::VulkanResouceManager::ConstructVulkanRenderObject()
{
	
	//1.iterate the raw data sources
	
	for (const auto& x : _assetMnanger.getModelDatas()) {
		VulkanRenderScene object;
		object.name = x.first;

	}
	//2.Use the data in raw datas to create the buffer for the vertex and index
	//3.Get the image datas to create Textures for future use
	//4. animate if any
	//5 skin if any
	//6 store it into read to render object
}

void vulkan::VulkanResouceManager::prepareNodeDescriptor(Node* node, VkDescriptorSetLayout descriptorSetLayout)
{
	if (node->meshID != -1) {

	}
}

void vulkan::VulkanResouceManager::CreateTextureImageView(VkImageView& imageview, VkImage image)
{
	_BufferManager.CreateImageView(imageview,image, VK_FORMAT_R8G8B8A8_SRGB,VK_IMAGE_ASPECT_COLOR_BIT,0);

}

void vulkan::VulkanResouceManager::ConstructSceneNode(SceneNode* parent, Node* sourceNode, VulkanRenderScene& object, ModelData& modelData)
{
	SceneNode* node = new SceneNode{};
	
	node->parent = parent;
	node->translation = sourceNode->translation;
	node->scale = sourceNode->scale;
	node->rotation = sourceNode->rotation;
	node->matrix = sourceNode->matrix;
	for (auto& x : sourceNode->children) {
		ConstructSceneNode(node, x, object, modelData);
	}
	if (sourceNode->meshID != -1) {
		node->mesh.push_back(object.meshes[sourceNode->meshID]);
	}
	if (node->parent) {
		node->parent->children.push_back(node);
	}
	object.sceneGraph.push_back(node);

}



