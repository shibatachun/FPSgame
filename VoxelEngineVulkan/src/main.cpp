#include "utils/GlfwHelper.h"

int main() {
	uint32_t width = 1280;
	uint32_t height = 800;
	GLFWwindow* window = initWindow("GLFW example", width, height);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
