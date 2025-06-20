#include "Shaders.h"



asset::Shaders::Shaders(std::string& filepath)
{
	loadShaderFromFiles(filepath);
}

asset::Shaders::~Shaders()
{
}

void asset::Shaders::loadShaderFromFiles(const std::string& filename)
{
	auto shaderfile = asset::IterateDirectory(filename, {"spv"});
	_vulkanShaders.fragmentShader = readFile(shaderfile[0].path);
	_vulkanShaders.vertexShader = readFile(shaderfile[1].path);
	//TODO:read the hlsl shaders
	
}

