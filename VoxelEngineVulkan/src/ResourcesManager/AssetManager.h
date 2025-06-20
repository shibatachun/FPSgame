#pragma once
#include "AssetCommonUtils.h"
#include "Shaders.h"
namespace asset
{
	class AssetManager final
	{
	public:
		AssetManager();
		~AssetManager();
		const std::unordered_map<std::string, Shaders> getShaderAssets() { return _shaderAssets; };
		bool Init();

	private:
		void loadShaderAssets(std::string filepath);
	private:
		std::unordered_map<std::string, Shaders> _shaderAssets;
	};
}


