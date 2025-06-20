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
	std::cout << "AssetManager当前工作目录" << cwd.string() << std::endl;
	_shadersManager.reset(new asset::ShadersManager("res/shaders"));
	loadShaderAssets();
	
	return true;
}

void asset::AssetManager::loadShaderAssets()
{
	_shaderAssets.emplace("Triangle_Vulkan", _shadersManager->loadVulkanShaderFromFiles("triangle"));
}
