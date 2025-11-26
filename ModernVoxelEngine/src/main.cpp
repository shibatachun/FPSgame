#include "ModernEngine/ModernEngineApplication.h"
#include "VoxelEngine/VoxelApplication.h"



static void input_os_messages_callback(void* os_event, void* user_data) {
	InputSystem* input = (InputSystem*)user_data;
	input->OnEvent(os_event);
}
static void test_input(InputSystem* input) {
	float original_speed = gCamera.movementSpeed;

	
	if (input->IsKeyDown(KEY_A)) {
		
		gCamera.keys.left = true;
	}
	else
	{
		gCamera.keys.left = false;
	}

	if (input->IsKeyDown(KEY_D))
	{
		gCamera.keys.right = true;
	}
	else
	{
		gCamera.keys.right = false;
	}

	if (input->IsKeyDown(KEY_W)) {
	
		gCamera.keys.up = true;
	}
	else
	{
		gCamera.keys.up = false;
	}

	if (input->IsKeyDown(KEY_S)) {
		gCamera.keys.down = true;
	}
	else
	{
		gCamera.keys.down = false;
	}

	if (input->IsMouseDragging(MOUSE_BUTTONS_RIGHT)) {
		glm::vec2 delta = glm::vec2((input->previous_mouse_postion.x - input->mouse_position.x), (input->previous_mouse_postion.y-input->mouse_position.y));
		gCamera.rotate(glm::vec3(delta.y,-delta.x , 0.0f));
	}
}
static float sunAngle = 0.0f;

static void UpdateSundir(float deltatime) {
	sunAngle += deltatime * 0.1f;
	gCamera.sunDir.x = -0.3f;
	gCamera.sunDir.y = sin(sunAngle);
	gCamera.sunDir.z = cos(sunAngle);
	gCamera.sunDir = glm::normalize(gCamera.sunDir);
}
int main (int argc, char* argv[]) {
	uint32_t width = 1920;
	uint32_t height = 1080;
	//GLFWwindow* window = initWindow("GLFW example", width, height);
	API api = API::VULKAN;
	WindowConfiguration wconf{ 1920, 1080, api, "Window creation" };
	Window window;
	window.Init(&wconf);

	InputSystem input;
	input.init();
	input.has_focus = true;
	window.RegisterOSMessagesCallback(input_os_messages_callback, &input);
	auto& app = ModernEngineApplication::GetInstance(window.platform_handle,api);
	app.Init();
	app.SetWindowUserPointer();


	gCamera.type = Camera::CameraType::firstperson;
	gCamera.flipY = false;
	gCamera.setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
	gCamera.setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
	gCamera.setPerspective(60.0f, (float)width / (float)height, 0.1f, 256.0f);

	float frameCounter = 0.0f;
	float frameTimer = 0.0f;
	
	while (!window.requested_exit)
	{
		window.HandleOSMessages();
		
		input.NewFrame();
	

		auto tStart = std::chrono::high_resolution_clock::now();
		if (window.resized) {
			app.Resize();
			window.resized = false;
		}
		app.Draw();
		frameCounter++;
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		frameTimer = (float)tDiff / 1000.0f;
		test_input(&input);
		input.Update(frameTimer);
		UpdateSundir(frameTimer);
		gCamera.update(frameTimer);
		window.UpdateFPS();
		//TitleFps("lihai", window);
	}
	app.Finish();
	//glfwDestroyWindow(window);
	//glfwTerminate();
	window.Shutdown();
	return 0;
}

