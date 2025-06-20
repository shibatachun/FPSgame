#include "utils/GlfwHelper.h"
#include "GraphicAPI/RenderFactory.h"
#include "ResourcesManager/AssetManager.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h> 


int main() {
	uint32_t width = 1280;
	uint32_t height = 800;
	GLFWwindow* window = initWindow("GLFW example", width, height);
	API api = API::VULKAN;
	auto renderer = CreateRenderer(api, window, VK_PRESENT_MODE_FIFO_RELAXED_KHR);
	asset::AssetManager assetMananger;
	if (!assetMananger.Init())
	{
		std::cerr << "Failed to initialize assetManager" << std::endl;
		return -1;
	}
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		//renderer->DrawFrame();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
