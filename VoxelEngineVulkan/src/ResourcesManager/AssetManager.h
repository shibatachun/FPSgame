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
		const std::unordered_map<std::string, asset::shader> getShaderAssets() { return _shaderAssets; };
		bool Init();

	private:
		void loadShaderAssets();
		
	private:
		std::unique_ptr<asset::ShadersManager> _shadersManager;
		std::unordered_map<std::string, asset::shader> _shaderAssets;
	};
}


