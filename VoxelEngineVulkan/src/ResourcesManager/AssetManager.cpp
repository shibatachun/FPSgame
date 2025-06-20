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
	loadShaderAssets("res/shaders");
	return true;
}

void asset::AssetManager::loadShaderAssets(std::string filepath)
{
	_shaderAssets.emplace("Triangle", filepath);
}
