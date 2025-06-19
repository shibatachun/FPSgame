#include "VulkanClasses.h"

vulkan::Instance::Instance(GLFWwindow* window) : _window(window)
{
	
	CheckVulkanMinimumVersion(_vulkanVersion);

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	auto extensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

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

GLFWwindow* vulkan::Instance::getWindow() const
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
	instance_.getWindow();
	Check(glfwCreateWindowSurface(instance_.Handle(), instance_.getWindow(), nullptr, &surface_), "create window surface");
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
	const VkPhysicalDeviceFeatures& deviceFeatures, 
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

	const auto presentFamily = std::find_if(queueFamily_.begin(), queueFamily_.end(), [&](const VkQueueFamilyProperties& queueFamily) {
		VkBool32 presentSupport = false;
		const uint32_t i = static_cast<uint32_t>(&*queueFamily_.cbegin() - &queueFamily);
		vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, surface.Handle(), &presentSupport);
		return queueFamily.queueCount > 0 && presentSupport;
		});
	if (presentFamily == queueFamily_.end())
	{
		throw (std::runtime_error("found no presentation queue"));
	}
	graphicsFamilyIndex_ = static_cast<uint32_t>(graphicsFamily - queueFamily_.begin());
	presentFamilyIndex_ = static_cast<uint32_t>(presentFamily - queueFamily_.begin());

	const std::set<uint32_t> uniqueQueueFamilies = {
		graphicsFamilyIndex_,
		presentFamilyIndex_,
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
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = nextDeviceFeatures;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledLayerCount = static_cast<uint32_t>(_surface.getInstance().GetValidationLayers().size());
	createInfo.ppEnabledLayerNames = _surface.getInstance().GetValidationLayers().data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	Check(vkCreateDevice(_physicalDevice, &createInfo, nullptr, &device_), "create logical device");

	_debugUtils.SetDevice(device_);
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         (device_, graphicsFamilyIndex_, 0, &graphicsQueue_);
	vkGetDeviceQueue(device_, presentFamilyIndex_, 0, &presentQueue_);


}

vulkan::Device::~Device()
{
	if (device_ != nullptr) {
		vkDestroyDevice(device_, nullptr);
		device_ = nullptr;
	}
}

void vulkan::Device::WaitIdle() const
{
	Check(vkDeviceWaitIdle(device_), "wait for device idle");
}

void vulkan::Device::CheckRequiredExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions) 
{
	uint32_t extensionsCount = 0;
	vkEnumerateDeviceExtensionProperties(_physicalDevice, static_cast<const char*>(nullptr), &extensionsCount, nullptr);
	if (extensionsCount)
	{
		availableExtensions_.resize(extensionsCount);
	}
	vkEnumerateDeviceExtensionProperties(_physicalDevice, static_cast<const char*>(nullptr), &extensionsCount, availableExtensions_.data());
	std::set<std::string> required(requiredExtensions.begin(), requiredExtensions.end());

	for (const auto& extension : availableExtensions_)
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



vulkan::SwapChain::SwapChain(const Device& device, VkPresentModeKHR presentationMode) : _physicalDevice(device.PhysicalDevice()), _device(device)
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
	.minImageCount = imageCount,
	.imageFormat = surfaceFormat.format,
	.imageColorSpace = surfaceFormat.colorSpace,
	.imageExtent = extent,
	.imageArrayLayers = 1,
	.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
	.preTransform = details.Capabilities.currentTransform,
	.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	.presentMode = actualPresentMode,
	.clipped = VK_TRUE,
	.oldSwapchain = nullptr};

	//如果渲染和呈现用的是同一个队列族，用 独占（EXCLUSIVE） 模式，性能最好。
	//如果它们在不同的队列族，就用 并发（CONCURRENT） 模式，并把两条队列族索引都告诉 Vulkan，让它自动帮你管理图像的访问与同步。
	if (_device.GraphicsFamilyIndex() != _device.PresentFamilyIndex())
	{
		uint32_t queueFamilyIndices[] = { _device.GraphicsFamilyIndex(), device.PresentFamilyIndex() };
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
	for (size_t i = 0; i < _imageViews.size(); i ++ )
	{
		VkImageViewCreateInfo createInfo{ };
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
		Check(vkCreateImageView(_device.Handle(), &createInfo, nullptr, &_imageViews[i]), "Create image view");
	}

	const auto& debugUtils = _device.DebugUtils();

	for (size_t i = 0; i != _images.size(); i++)
	{
		debugUtils.SetObjectName(_images[i], ("Swapchain Image #" + std::to_string(i)).c_str());
		debugUtils.SetObjectName(_imageViews[i], ("Swapchain ImageView #" + std::to_string(i)).c_str());
	}

	

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

VkExtent2D vulkan::SwapChain::ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
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
