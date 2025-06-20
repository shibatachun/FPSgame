#include "Shaders.h"



asset::ShadersManager::ShadersManager(std::string dir) :
	_vulkan_shaders(asset::IterateDirectory(dir, { "spv" })),
	_d3d_shaders(asset::IterateDirectory(dir, { "hlsl" })){
	
	
}

asset::ShadersManager::~ShadersManager(){
}

asset::shader asset::ShadersManager::loadVulkanShaderFromFiles(const std::string& filename){
	shader out_shader;
	std::string frag_file, vert_file;
	frag_file += filename;
	frag_file += "_frag";
	vert_file += filename;
	vert_file += "_vert";

	for (auto entry : _vulkan_shaders){
		if (entry.name == frag_file){
			out_shader.fragmentShader = asset::readFile(entry.path);
		}
		else if (entry.name == vert_file)
		{
			out_shader.vertexShader = asset::readFile(entry.path);
		}
	}
	
	return out_shader;
}

asset::shader asset::ShadersManager::loadD3DShaderFromFiles( const std::string& filename){

	shader out_shader;
	return out_shader;
}



