#pragma once
#include "AssetCommonUtils.h"


namespace asset {



class ModelManager
{

public:
	ModelManager();
	~ModelManager();
	const std::unordered_map<std::string, int32_t>& GetModeDatas() { return _modelsIdMapping; };
	const ModelData& GetModelDataByName(std::string modelname) { return _models[utils::findInMap(_modelsIdMapping, modelname, {})]; }
	const std::vector<ModelData>& GetAllModelDatas() { return _models; }
	const std::vector<Image>& GetAllImagesDatas() { return _imageFile; }
	const std::unordered_map<std::string, int32_t>& GetImageDatas() { return _imageFileIdMapping; };
	const Image& GetImageDataByName(std::string imagename) { return _imageFile[utils::findInMap(_imageFileIdMapping, imagename, {})]; };
	const void freeImage(unsigned char* pixel);
private:
	void processNode(aiNode* node, const aiScene* scene);
	void loadAllImage();
	void loadAllModel();
	void loadImage(std::string filename, std::string path, bool isktx = false);
	void loadTestExample();
	void loadobj(std::string filePath);
	void loadgltf(std::string filename);
	void loadNode(aiNode* scene, std::vector<Vertex>& vertexbuffer, std::vector<uint32_t>& indicebuffer);
	void loadFBX(std::string model_name, std::string filepath);
	void processFBXNode(const aiNode* node, int depth);
	void createDummyTexture();
	//test function
	void loadgltf_test(std::string filename, std::string path);
	void loadImage_test(tinygltf::Model& gltfModel, ModelData& model);
	void loadMaterial_test(tinygltf::Model& gltfMode, ModelData& model);
	void loadNode_test(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& tingymodel, ModelData& model, float globalscale);
	bool loadImageDataFunc(tinygltf::Image* image, const int imageIndex, std::string* error, std::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData);
	bool loadImageDataFuncEmpty(tinygltf::Image* image, const int imageIndex, std::string* error, std::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData);


private:
	uint32_t														_modelsCount;
	uint32_t														_imageCount;
	std::vector<ModelData>											_models;
	std::unordered_map<std::string, int32_t>						_modelsIdMapping;
	std::vector<Image>												_imageFile;
	std::unordered_map<std::string, int32_t>						_imageFileIdMapping;

	std::unordered_map<std::string, FileInfo>						_modelFilesInfo;
	std::unordered_map<std::string, FileInfo>						_imageFilesInfo;
	/*std::unordered_map<FileInfo, */

};
}

