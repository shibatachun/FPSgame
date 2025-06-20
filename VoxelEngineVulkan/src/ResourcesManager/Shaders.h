#pragma once
#include "AssetCommonUtils.h"
namespace asset
{
	struct shader{
		std::vector<char> vertexShader;
		std::vector<char> fragmentShader;
	};


	class ShadersManager{
	public:
		ShadersManager(std::string dir);
		~ShadersManager();
		shader loadVulkanShaderFromFiles(const std::string& filename);
		shader loadD3DShaderFromFiles(const std::string& filename);

	private:
	
	private:
		std::vector<FileInfo> _vulkan_shaders;
		std::vector<FileInfo>_d3d_shaders;

	};

}

