#include "AssetManager.h"

asset::AssetManager::AssetManager()
{
}

asset::AssetManager::~AssetManager()
{
}

bool asset::AssetManager::Init()
{
	std::filesystem::path cwd = std::filesystem::current_path();
	std::cout << "AssetManager��ǰ����Ŀ¼" << cwd.string() << std::endl;
	_shadersManager.reset(new asset::ShadersManager("res/shaders"));
	loadShaderAssets();
	
	return true;
}

void asset::AssetManager::loadShaderAssets()
{
	_shaderAssets.emplace("Triangle_Vulkan", _shadersManager->loadVulkanShaderFromFiles("triangle"));
}
