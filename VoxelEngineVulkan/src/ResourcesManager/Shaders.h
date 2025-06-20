#pragma once
#include "AssetCommonUtils.h"
namespace asset
{
	struct shader
	{
		std::vector<char> vertexShader;
		std::vector<char> fragmentShader;
	};


	class Shaders
	{
	public:
		Shaders(std::string & filepath);
		~Shaders();
		const shader VulkanShaders() { return _vulkanShaders; };
		const shader D3DShaders() { return _d3dShaders; };
	private:
		void loadShaderFromFiles(const std::string& filename);
		shader _vulkanShaders;
		shader _d3dShaders;
	};

}

