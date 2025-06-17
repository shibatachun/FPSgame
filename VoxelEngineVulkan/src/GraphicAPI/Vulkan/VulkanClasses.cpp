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

vulkan::Device::Device(
	VkPhysicalDevice device, 
	const Surface& surface, 
	const std::vector<const char*>& requiredExtension, 
	const VkPhysicalDeviceFeatures& deviceFeatures, 
	const void* nextDeviceFeatures):
	_physicalDevice(device),
	_surface(surface),
	_debugUtils(surface.getInstance().Handle())
{
	CheckRequiredExtensions(_physicalDevice, requiredExtension);

}

vulkan::Device::~Device()
{
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
