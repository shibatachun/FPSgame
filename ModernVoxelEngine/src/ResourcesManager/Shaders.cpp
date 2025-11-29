#include "Shaders.h"



asset::ShadersManager::ShadersManager(std::string dir) :
	_vulkan_shaders(asset::IterateDirectory(dir, { "spv" })),
	_d3d_shaders(asset::IterateDirectory(dir, { "hlsl" })){
	ConstructShader("Triangle_Vulkan", "triangle_vert", "triangle_frag");
	ConstructShader("Rectangle_Vulkan", "rectangle_vert", "triangle_frag");
	ConstructShader("SkySphere", "skysphere_vert", "skysphere_frag");
	ConstructShader("scene", "scene_vert", "scene_frag");
	ConstructShader("grid", "grid_vert", "grid_frag");
	ConstructShader("sky", "triangle_vert", "sky_frag");
	ConstructShader("pbr", "pbr_vert", "pbr_frag");
	ConstructShader("debug", "debug_vert", "debug_frag");
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

//For vertex name and fragment name, must match the exact file name, you can exclude the extension.
void asset::ShadersManager::ConstructShader(std::string name, std::string vertex, std::string fragment)
{
	shader TargetShader;
	for (auto entry : _vulkan_shaders) {
		if (entry.name == fragment) {
			TargetShader.fragmentShader = asset::readFile(entry.path);
		}
		else if (entry.name == vertex)
		{
			TargetShader.vertexShader = asset::readFile(entry.path);
		}
	}
	if (!TargetShader.fragmentShader.empty() && !TargetShader.vertexShader.empty())
	{
		_shaderAssets.emplace(name, TargetShader);
	}
}



