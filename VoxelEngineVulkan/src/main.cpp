#include "utils/GlfwHelper.h"
#include "GraphicAPI/RenderFactory.h"


int main() {
	uint32_t width = 1280;
	uint32_t height = 800;
	GLFWwindow* window = initWindow("GLFW example", width, height);
	API api = API::VULKAN;
	auto renderer = CreateRenderer(api, window);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
