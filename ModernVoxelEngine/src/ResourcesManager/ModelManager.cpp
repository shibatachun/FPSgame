#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "ModelManager.h"


asset::ModelManager::ModelManager()
{
	loadAllModel();
	loadAllImage();
	createDummyTexture();
	//loadImage("test","res/textures/text_texture.jpg");
	//loadImage("viking", "res/textures/viking_room.png");
	//loadTestExample();
	//loadobj("res/models/viking_room.obj");
	//loadgltf("res/models/Zzz/zzz_alice/alice.pmx");
	//loadgltf("res/models/sponza/sponza.gltf");
	//loadgltf_test("Sponza","res/models/sponza/sponza.gltf");
	//loadgltf_test("sky","res/models/sphere.gltf");
	//loadImage("sky", "res/textures/skysphere_rgba.ktx", true);
	//CollectTexturePaths("res/models/sponza/sponza.gltf");
	//loadFBX("res/models/main_sponza/NewSponza_Main_Yup_003.fbx");
	loadFBX("res/models/generator_LP.fbx");
}

asset::ModelManager::~ModelManager()
{
}



void asset::ModelManager::processNode(aiNode* node, const aiScene* scene)
{
}

void asset::ModelManager::loadAllImage()
{
	std::vector<FileInfo> allTextureFiles;
	IterateAllDirectories("res/textures", { "png","jpg","ktx" }, allTextureFiles);
	for (auto& x : allTextureFiles) {
		_imageFilesInfo.emplace(x.GetFileNameWithExtension(), x);
	}
	
}

void asset::ModelManager::loadAllModel()
{
	for (auto& x : IterateDirectory("res/models", { "obj","fbx","gltf"})) {
		_modelFilesInfo.emplace(x.name, x);
	}
}

void asset::ModelManager::loadImage(std::string filename, std::string path, bool isktx)
{

	Image image;

	if (isktx) {
		ktxTexture* kt;
		ktxResult result = KTX_SUCCESS;
		result = ktxTexture_CreateFromNamedFile(path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kt);
		assert(result == KTX_SUCCESS);
		image.texWidth = kt->baseWidth;
		image.texHeight = kt->baseHeight;
		image.mipLevels = kt->numLevels;
		image.subresource.resize(image.mipLevels);
		image.pixel = ktxTexture_GetData(kt);
		
		image.size = ktxTexture_GetDataSize(kt);
		image.isKtx = true;
		image.name = filename;
		image.format = FromVk(ktxTexture_GetVkFormat(kt));

		for (int i = 0; i < image.mipLevels; i++) {
			ktx_size_t offset;
			KTX_error_code result = ktxTexture_GetImageOffset(kt, i, 0, 0, &offset);
			assert(result == KTX_SUCCESS);
			SubResource& subResouces = image.subresource[i];
			subResouces.offset = offset;
			subResouces.depth = 1;
			subResouces.width = std::max(1u, kt->baseWidth >> i);
			subResouces.height = std::max(1u, kt->baseHeight >> i);
			subResouces.layer = 0;
		}

		_imageFileIdMapping.emplace(filename,_imageCount);
		_imageFile.push_back(image);
		_imageCount++;
	
		
	}
	else{

		image.pixel = stbi_load(path.c_str(), &image.texWidth, &image.texHeight, &image.texChannels, STBI_rgb_alpha);
		_imageFileIdMapping.emplace(filename, _imageCount);
		_imageFile.push_back(image);
		_imageCount++;
	}
	
}

const void asset::ModelManager::freeImage(unsigned char* pixel)
{
	stbi_image_free(pixel);
}

void asset::ModelManager::loadTestExample()
{
	ModelData data;
	data.meshCount = 1;
	MeshData meshData{};
	meshData.meshes.resize(data.meshCount);
	Mesh& mesh = meshData.meshes[0];
	std::vector<Vertex> test_vertices = {
	{.pos = {-0.5f, -0.5f, 0.0f},	.color = {1.0f,0.0f,0.0f,1.0f}, .uv = {1.0f, 0.0f}},
	{.pos = {0.5f, -0.5f, 0.0f},	.color = {0.0f,1.0f,0.0f,0.0f}, .uv = {0.0f,0.0f}},
	{.pos = {0.5f, 0.5f, 0.0f},		.color = {0.0f,0.0f,1.0f,1.0f}, .uv = {0.0f,1.0f}},
	{.pos = {-0.5f, 0.5f, 0.0f},	.color = {1.0f,1.0f,1.0f,1.0f},	.uv = {1.0f, 1.0f}},

	{.pos = {-0.5f, -0.5f, -0.5f},	.color = {1.0f,0.0f,0.0f,1.0f}, .uv = {1.0f, 0.0f}},
	{.pos = {0.5f, -0.5f, -0.5f},	.color = {0.0f,1.0f,0.0f,0.0f}, .uv = {0.0f,0.0f}},
	{.pos = {0.5f, 0.5f, -0.5f},		.color = {0.0f,0.0f,1.0f,1.0f}, .uv = {0.0f,1.0f}},
	{.pos = {-0.5f, 0.5f, -0.5f},	.color = {1.0f,1.0f,1.0f,1.0f},	.uv = {1.0f, 1.0f}},
	};
	std::vector<uint16_t> indices = {
		0,1,2,2,3,0,4,5,6,6,7,4
	};
	//push back用于完整的对象，如果容器内的构造的
	mesh.vertexCount = static_cast<uint32_t>(test_vertices.size());
	mesh.indiceCount = static_cast<uint32_t>(indices.size());
	//什么时候用reserve，在后面要用push_back加入元素可以减去多次预分配内存使用时间，用reserve事先获得需要的内存，但是reserve也不混直接[i]复制，因为元素的数量没变，需要用resize
	//为什么用auto &, 因为直接拿引用避免一次拷贝，不拿的话会再auto x阶段又进行一次构造拷贝，push back又进行一次，会增加开销
	for (const auto& x : test_vertices) {
		data.vertices.push_back(x);
	}
	for (auto x : indices) {
		data.indices.push_back(x);
	}
	data.meshdatas.push_back(meshData);
	

	_modelsIdMapping.emplace("test_data", _modelsCount);
	_models.push_back(data);
	_modelsCount++;

}

void asset::ModelManager::loadobj(std::string filePath)
{
	ModelData data;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, 
		aiProcess_Triangulate |            // 和 tinyobj 的自动三角化等价
		aiProcess_JoinIdenticalVertices |  // 合并完全相同的顶点
		aiProcess_GenSmoothNormals |       // 如无法线则生成
		aiProcess_CalcTangentSpace |       // 如需要切线
		aiProcess_ImproveCacheLocality |
		aiProcess_FlipUVs |
		aiProcess_SortByPType);
	if (!scene) {
		std::cout << "LoadAndExportCustomFormat() failed to loaded model" << filePath << "\n";
		std::cerr << "Assimp Error: " << importer.GetErrorString() << "\n";
		return;
	}
	data.meshCount = scene->mNumMeshes;
	MeshData meshdata{};
	meshdata.meshes.resize(scene->mNumMeshes);
	
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		Mesh& mesh = meshdata.meshes[i];
		mesh.vertexCount = scene->mMeshes[i]->mNumVertices;
		mesh.indiceCount = scene->mMeshes[i]->mNumFaces * 3;
		mesh.name = scene->mMeshes[i]->mName.C_Str();

		data.vertexSize += mesh.vertexCount;
		data.indiceSize += mesh.indiceCount;
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		uint32_t vertex_start = static_cast<uint32_t>(data.vertices.size());
		uint32_t indice_start = static_cast<uint32_t>(data.indices.size());
		Mesh& meshOffset = meshdata.meshes[i];
		const aiMesh* mesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
			Vertex vert{};
			vert.pos = glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
			vert.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			vert.uv = mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y) : glm::vec2(0.0f, 0.0f);
			data.vertices.push_back(vert);
		}
		for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
			const aiFace& face = mesh->mFaces[j];
		
			for (unsigned i = 0; i != mesh->mFaces[j].mNumIndices; i++) {
				data.indices.push_back(mesh->mFaces[j].mIndices[i]);
			}
		}
		meshOffset.vertexOffset = vertex_start;
		meshOffset.indiceCount = indice_start;
	}
	assert(data.indices.size() == data.indiceSize && data.vertices.size() == data.vertexSize);
	_modelsIdMapping.emplace("viking_room", _modelsCount);
	_models.push_back( data);
	_modelsCount++;



}
//TODO
void asset::ModelManager::loadgltf(std::string filename)
{                     
	

}

void asset::ModelManager::loadNode(aiNode* scene, std::vector<Vertex>& vertexbuffer, std::vector<uint32_t>& indicebuffer)
{
	
}


void PrintTexture(const aiMaterial* mat, aiTextureType type, const char* typeName)
{
	unsigned int texCount = mat->GetTextureCount(type);
	if (texCount == 0) return;

	std::cout << "  " << typeName << " (" << texCount << "):\n";

	for (unsigned int i = 0; i < texCount; i++) {
		aiString path;
		if (mat->GetTexture(type, i, &path) == AI_SUCCESS) {
			std::cout << "     [" << i << "] " << path.C_Str() << "\n";
		}
	}
}

void PrintMaterialInfo(const aiScene* scene)
{
	if (!scene) {
		std::cout << "Scene is null.\n";
		return;
	}

	std::cout << "================ MATERIAL INFO ================\n";
	std::cout << "Material Count: " << scene->mNumMaterials << "\n\n";

	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* mat = scene->mMaterials[i];

		aiString matName;
		mat->Get(AI_MATKEY_NAME, matName);

		std::cout << "------------------------------------------------\n";
		std::cout << "Material #" << i << ": " << matName.C_Str() << "\n";

		// ----- Print basic colors -----
		aiColor4D baseColor;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_BASE_COLOR, baseColor))
			std::cout << "  BaseColorFactor: (" << baseColor.r << ", " << baseColor.g << ", " << baseColor.b << ", " << baseColor.a << ")\n";

		aiColor3D diffuse;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
			std::cout << "  DiffuseColor: (" << diffuse.r << ", " << diffuse.g << ", " << diffuse.b << ")\n";

		aiColor3D specular;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, specular))
			std::cout << "  SpecularColor: (" << specular.r << ", " << specular.g << ", " << specular.b << ")\n";

		aiColor3D emissive;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive))
			std::cout << "  EmissiveColor: (" << emissive.r << ", " << emissive.g << ", " << emissive.b << ")\n";

		float opacity;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_OPACITY, opacity))
			std::cout << "  Opacity: " << opacity << "\n";

		float metallic;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_METALLIC_FACTOR, metallic))
			std::cout << "  MetallicFactor: " << metallic << "\n";

		float roughness;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness))
			std::cout << "  RoughnessFactor: " << roughness << "\n";

		float glossiness;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_GLOSSINESS_FACTOR, glossiness))
			std::cout << "  GlossinessFactor: " << glossiness << "\n";

		// ----- Print all texture types -----

		PrintTexture(mat, aiTextureType_DIFFUSE, "DiffuseTexture");
		PrintTexture(mat, aiTextureType_BASE_COLOR, "BaseColorTexture");
		PrintTexture(mat, aiTextureType_NORMALS, "NormalTexture");
		PrintTexture(mat, aiTextureType_METALNESS, "MetallicTexture");
		PrintTexture(mat, aiTextureType_DIFFUSE_ROUGHNESS, "RoughnessTexture");
		PrintTexture(mat, aiTextureType_AMBIENT_OCCLUSION, "OcclusionTexture");
		PrintTexture(mat, aiTextureType_EMISSIVE, "EmissiveTexture");
		PrintTexture(mat, aiTextureType_OPACITY, "OpacityTexture");

		// PBR MetallicRoughness (glTF / FBX)
		PrintTexture(mat, aiTextureType_UNKNOWN, "UNKNOWNTexture (MetalRough / Extras)");

		// SpecularGlossiness workflow
		PrintTexture(mat, aiTextureType_SPECULAR, "SpecularTexture");
		PrintTexture(mat, aiTextureType_SHININESS, "GlossinessTexture");

		std::cout << "\n";
	}

	std::cout << "================================================\n";
}

void asset::ModelManager::loadFBX(std::string filepath)
{
	ModelData modelData;
	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true);
	const aiScene* scene = importer.ReadFile(filepath,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FlipUVs);
	if (!scene) {
		std::cout << "LoadAndExportCustomFormat() failed to loaded model " << filepath << "\n";
		std::cerr << "Assimp Error: " << importer.GetErrorString() << "\n";
		return;
	}
	modelData.name = GetFileName(filepath);
	modelData.meshCount = scene->mNumMeshes;
	modelData.timestamp = GetLastModifiedTime(filepath);
	modelData.materials.resize(scene->mNumMaterials);
	modelData.meshdatas.resize(scene->mNumMeshes);
	for (int i = 0; i < modelData.meshCount; i++) {
		MeshData& meshData = modelData.meshdatas[i];
		//TODO: Determine the lods
		Mesh mesh;
		mesh.vertexCount = scene->mMeshes[i]->mNumVertices;
		mesh.indiceCount = scene->mMeshes[i]->mNumFaces * 3;
		mesh.materialID = scene->mMeshes[i]->mMaterialIndex;
		mesh.name = scene->mMeshes[i]->mName.C_Str();
		mesh.vertexOffset += modelData.vertexSize;
		mesh.indexOffset += modelData.indiceSize;
		modelData.vertexSize += mesh.vertexCount;
		modelData.indiceSize += mesh.indiceCount;
		meshData.meshes.push_back(mesh);
	}
	modelData.vertices.resize(modelData.vertexSize);
	modelData.indices.resize(modelData.indiceSize);
	
	for (int i = 0; i < modelData.meshCount; i++) {
		MeshData& meshData = modelData.meshdatas[i];
		uint32_t vertice_offset = meshData.meshes[0].vertexOffset;
		uint32_t indices_offset = meshData.meshes[0].indexOffset;
		const aiMesh* assimpMesh = scene->mMeshes[i];

		for (unsigned int j = 0; j < assimpMesh->mNumVertices; j++) {
			Vertex& ver = modelData.vertices[vertice_offset+j];
			ver.pos = glm::vec3(assimpMesh->mVertices[j].x, assimpMesh->mVertices[j].y, assimpMesh->mVertices[j].z);
			ver.normal = glm::vec3(assimpMesh->mNormals[j].x, assimpMesh->mNormals[j].y, assimpMesh->mNormals[j].z);
			ver.uv = assimpMesh->HasTextureCoords(0) ? glm::vec2(assimpMesh->mTextureCoords[0][j].x, assimpMesh->mTextureCoords[0][j].y) : glm::vec2(0.0f, 0.0f);
			ver.color = assimpMesh->HasVertexColors(0) ? glm::vec4(assimpMesh->mColors[0]->r, assimpMesh->mColors[0]->g, assimpMesh->mColors[0]->b, assimpMesh->mColors[0]->a) : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			ver.tangent = assimpMesh->HasTangentsAndBitangents() ? glm::vec4(assimpMesh->mTangents[j].x, assimpMesh->mTangents[j].y, assimpMesh->mTangents[j].z, 0.0f) : glm::vec4(0.0f);
			ver.normal = glm::normalize(ver.normal);
			meshData.aabbMin = utils::math::VecMin(ver.pos, meshData.aabbMin);
			meshData.aabbMax = utils::math::VecMax(ver.pos, meshData.aabbMax);
		}
		for (unsigned int j = 0; j < assimpMesh->mNumFaces; j++) {
			const aiFace& face = assimpMesh->mFaces[j];
			unsigned int baseIndex = j * 3;
			modelData.indices[indices_offset + baseIndex] = face.mIndices[0];
			modelData.indices[indices_offset + baseIndex + 1] = face.mIndices[1];
			modelData.indices[indices_offset + baseIndex + 2] = face.mIndices[2];
		}
		for (int i = 0; i < meshData.meshes[0].indiceCount; i +=3) {
			Vertex& vert0 = modelData.vertices[vertice_offset + modelData.indices[indices_offset + i]];
			Vertex& vert1 = modelData.vertices[vertice_offset + modelData.indices[indices_offset + i+1]];
			Vertex& vert2 = modelData.vertices[vertice_offset + modelData.indices[indices_offset + i+2]];
			glm::vec3 deltaPos1 = vert1.pos - vert0.pos;
			glm::vec3 deltaPos2 = vert2.pos - vert0.pos;
			glm::vec2 deltaUV1 = vert1.uv - vert0.uv;
			glm::vec2 deltaUV2 = vert2.uv - vert0.uv;
			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
			vert0.tangent = glm::vec4(tangent, 0.0f);
			vert1.tangent = glm::vec4(tangent, 0.0f);
			vert2.tangent = glm::vec4(tangent, 0.0f);
		}
		modelData.aabbMin = utils::math::VecMin(modelData.aabbMin, meshData.aabbMin);
		modelData.aabbMax = utils::math::VecMax(modelData.aabbMax, meshData.aabbMax);

	}


	_models.push_back(modelData);
	_modelsCount++;

}

void asset::ModelManager::processFBXNode(const aiNode * node, int depth)
{
	if (!node) return;

	// 缩进打印层级
	for (int i = 0; i < depth; i++)
		printf("  ");

	// 打印 node 名字
	printf("%s\n", node->mName.C_Str());

	// 递归子节点
	for (unsigned i = 0; i < node->mNumChildren; i++)
	{
		processFBXNode(node->mChildren[i], depth + 1);
	}
}

void asset::ModelManager::createDummyTexture()
{
	Image image;
	image.id = _imageCount;
	image.format = TextureFormat::RGBA8_UNORM;
	image.mipLevels = 1;
	uint8_t pixel[4] = { 255,255,255,255 };
	image.size = sizeof(pixel);
	image.pixel = new unsigned char[4];
	memcpy(image.pixel, pixel, sizeof(pixel));
	SubResource sub;
	sub.depth = 1;
	sub.height = 1;
	sub.width = 1;
	sub.mip = 0;
	sub.layer = 0;
	sub.offset = 0;
	image.subresource.push_back(sub);
	
	_imageFileIdMapping.emplace("dummy", _imageCount);
	_imageFile.push_back(image);
	_imageCount++;
	


}

 
//Tutorial test functions
void asset::ModelManager::loadgltf_test(std::string filename, std::string path)
{
	ModelData model;
	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF gltfContext;
	gltfContext.SetImageLoader(tinygltf::LoadImageData, nullptr);
	std::string error, warning;
	bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, path);
	if (fileLoaded) {
		
		loadImage_test(gltfModel, model);
		loadMaterial_test(gltfModel, model);
		const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
			loadNode_test(nullptr, node, scene.nodes[i], gltfModel, model, 1.0f);
			
		}
	}
	

	for (Node* node : model.linearNodeHierarchy) {
		if (node->meshID != -1) {
			const glm::mat4 localMatrix = node->getMatrix();
			MeshData& meshdata = model.meshdatas[node->meshID];
			for (const Mesh& mashoffset : meshdata.meshes) {
				for (uint32_t i = 0; i < mashoffset.vertexCount; i++) {
					Vertex& vertex = model.vertices[mashoffset.vertexOffset+i];
					vertex.pos = glm::vec3(localMatrix * glm::vec4(vertex.pos, 1.0f));
					vertex.normal = glm::normalize(glm::mat3(localMatrix) * vertex.normal);

					vertex.pos.y *= -1.0f;
					vertex.normal.y *= -1.0f;

					//vertex.color = model.materials[mashoffset.materialID].baseColorFactor * vertex.color;
				}
			}
		}
	}
	_modelsIdMapping.emplace(filename,_modelsCount);
	_models.push_back(model);
	_modelsCount++;

}

void asset::ModelManager::loadImage_test(tinygltf::Model& gltfModel, ModelData& model)
{
	for (tinygltf::Image& image : gltfModel.images) {
		Image texture_image; 
		std::string path = utils::findInMap(_imageFilesInfo, image.uri).path;
		model.images.push_back(image.uri);
		ktxTexture* ktxTexture;
		ktxResult result = KTX_SUCCESS;
		texture_image.name = image.uri;
		if (!FileExists(path)) {
			throw std::runtime_error("Could not load texture from " + path + "\n\nMake sure the assets submodule has been checked out and is up-to-date.");
		}
		result = ktxTexture_CreateFromNamedFile(path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
		assert(result == KTX_SUCCESS);
		texture_image.texWidth = ktxTexture->baseWidth;
		texture_image.texHeight = ktxTexture->baseHeight;
		texture_image.texDepth = ktxTexture->baseDepth;
		texture_image.mipLevels = ktxTexture->numLevels;

		texture_image.pixel = ktxTexture_GetData(ktxTexture);
		
		texture_image.size = ktxTexture_GetDataSize(ktxTexture);
		texture_image.format = FromVk(ktxTexture_GetVkFormat(ktxTexture));
		texture_image.subresource.resize(texture_image.mipLevels);
		for (int i = 0; i < texture_image.mipLevels; i++) {
			ktx_size_t offset;
			KTX_error_code result = ktxTexture_GetImageOffset(ktxTexture, i, 0, 0, &offset);
			assert(result == KTX_SUCCESS);
			SubResource& sub = texture_image.subresource[i];
			sub.mip = i;
			sub.width = std::max(1u, ktxTexture->baseWidth >> i);
			sub.height = std::max(1u, ktxTexture->baseHeight >> i);
			sub.depth = 1;
			sub.offset = offset;
			sub.layer = 0;
		}
		texture_image.id = _imageCount;
		_imageFileIdMapping.emplace(image.uri, _imageCount);
		_imageFile.push_back(texture_image);
		_imageCount++;

	}

}

void asset::ModelManager::loadMaterial_test(tinygltf::Model& gltfMode, ModelData& model)
{

	for (tinygltf::Material& mat : gltfMode.materials) {
		Material material;
		if (mat.values.find("baseColorTexture") != mat.values.end()) {
			uint32_t texId = utils::findInMap(_imageFileIdMapping, gltfMode.images.at(gltfMode.textures[mat.values["baseColorTexture"].TextureIndex()].source).uri, -1);
			material.baseColorTexture = texId == -1 ? -1 : texId;
			model.imageCount++;
		}
		if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
			uint32_t texId = utils::findInMap(_imageFileIdMapping, gltfMode.images.at(gltfMode.textures[mat.values["metallicRoughnessTexture"].TextureIndex()].source).uri, -1);
			material.matallicRoughnessTexture = texId == -1 ? -1 : texId;
		}
		if (mat.values.find("roughnessFactor") != mat.values.end()) {
			material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
		}
		if (mat.values.find("metallicFactor") != mat.values.end()) {
			material.matallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
		}
		if (mat.values.find("baseColorFactor") != mat.values.end()) {
			material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
		}
		if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
			uint32_t texId = utils::findInMap(_imageFileIdMapping, gltfMode.images.at(gltfMode.textures[mat.additionalValues["normalTexture"].TextureIndex()].source).uri, -1);
			material.normalTexture = texId == -1 ? -1 : texId;
		}
		else {
			material.normalTexture = 0;
		}
		if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
			uint32_t texId = utils::findInMap(_imageFileIdMapping, gltfMode.images.at(gltfMode.textures[mat.additionalValues["emissiveTexture"].TextureIndex()].source).uri, -1);
			material.emissiveTexture = texId == -1 ? -1 : texId;
		}
		if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
			uint32_t texId = utils::findInMap(_imageFileIdMapping, gltfMode.images.at(gltfMode.textures[mat.additionalValues["occlusionTexture"].TextureIndex()].source).uri, -1);
			material.occlusionTexture = texId == -1 ? -1 : texId;
		}
		if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
			tinygltf::Parameter param = mat.additionalValues["alphaMode"];
			if (param.string_value == "BLEND") {
				material.alphaMode = AlphaMode::ALPHAMODE_BLEND;
			}
			if (param.string_value == "MASK") {
				material.alphaMode = AlphaMode::ALPHAMODE_MASK;
			}
		}
		if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
			material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
		}

		model.materials.push_back(material);
		model.materialCount++;
		
	}


}

void asset::ModelManager::loadNode_test(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& tingymodel, ModelData& model, float globalscale)
{
	Node* newNode = new Node{};
	newNode->index = nodeIndex;
	newNode->parent = parent;
	newNode->name = node.name;
	newNode->skinIndex = node.skin;
	newNode->matrix = glm::mat4(1.0f);

	glm::vec3 translation = glm::vec3(0.f);
	if (node.translation.size() == 3) {
		translation = glm::make_vec3(node.translation.data());
		newNode->translation = translation;
	}
	glm::mat4 rotation = glm::mat4(1.0f);
	if (node.rotation.size() == 4) {
		glm::quat q = glm::make_quat(node.rotation.data());
		newNode->rotation = glm::mat4(q);
	}
	glm::vec3 scale = glm::vec3(1.0f);
	if (node.scale.size() == 3) {
		scale = glm::make_vec3(node.scale.data());
		newNode->scale = scale;
	}
	if (node.matrix.size() == 16) {
		newNode->matrix = glm::make_mat4x4(node.matrix.data());
		if (globalscale != 1.0f) {
			newNode->matrix = glm::scale(newNode->matrix, glm::vec3(globalscale));
		}
	}
	if (node.children.size() > 0) {
		for (auto i = 0; i < node.children.size(); i++) {
			loadNode_test(newNode, tingymodel.nodes[node.children[i]], node.children[i], tingymodel, model, globalscale);
		}
	}

	//1.有不同的node,node有对应的mesh
	//2.一个mesh有多个primitive
	//3.一个primitive有很多vertex和indice
	if (node.mesh > -1) {
		const tinygltf::Mesh mesh = tingymodel.meshes[node.mesh];
		MeshData meshdata{};
		meshdata.localTransform = newNode->getMatrix();
		meshdata.name = mesh.name;
		for (size_t j = 0; j < mesh.primitives.size(); j++) {
			const tinygltf::Primitive& primitive = mesh.primitives[j];
			if (primitive.indices < 0) {
				continue;
			}
			uint32_t indexStart = static_cast<uint32_t>(model.indices.size());
			uint32_t vertexStart = static_cast<uint32_t>(model.vertices.size());
			uint32_t indexCount = 0;
			uint32_t vertexCount = 0;
			glm::vec3 posMin{};
			glm::vec3 posMax{};
			bool hasSkin = false;
			//Vertex
			{
				const float* bufferPos = nullptr;
				const float* bufferNormals = nullptr;
				const float* bufferTexCoords = nullptr;
				const float* bufferColors = nullptr;
				const float* bufferTangents = nullptr;
				uint32_t numColorComponents;
				const uint16_t* bufferJoints = nullptr;
				const float* bufferWeights = nullptr;

				assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

				const tinygltf::Accessor& posAccessor = tingymodel.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& posView = tingymodel.bufferViews[posAccessor.bufferView];
				bufferPos = reinterpret_cast<const float*>(&(tingymodel.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
				posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
				posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
				meshdata.aabbMax = utils::math::VecMax(posMax, meshdata.aabbMax);
				meshdata.aabbMin = utils::math::VecMin(posMin, meshdata.aabbMin);

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
					const tinygltf::Accessor &normAccessor = tingymodel.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView &normView = tingymodel.bufferViews[normAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float *>(&(tingymodel.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
				}

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
					const tinygltf::Accessor &uvAccessor = tingymodel.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView &uvView = tingymodel.bufferViews[uvAccessor.bufferView];
					bufferTexCoords = reinterpret_cast<const float *>(&(tingymodel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				}

				if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor& colorAccessor = tingymodel.accessors[primitive.attributes.find("COLOR_0")->second];
					const tinygltf::BufferView& colorView = tingymodel.bufferViews[colorAccessor.bufferView];
					// Color buffer are either of type vec3 or vec4
					numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
					bufferColors = reinterpret_cast<const float*>(&(tingymodel.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
				}

				if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
				{
					const tinygltf::Accessor &tangentAccessor = tingymodel.accessors[primitive.attributes.find("TANGENT")->second];
					const tinygltf::BufferView &tangentView = tingymodel.bufferViews[tangentAccessor.bufferView];
					bufferTangents = reinterpret_cast<const float *>(&(tingymodel.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
				}

				// Skinning
				// Joints
				if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor &jointAccessor = tingymodel.accessors[primitive.attributes.find("JOINTS_0")->second];
					const tinygltf::BufferView &jointView = tingymodel.bufferViews[jointAccessor.bufferView];
					bufferJoints = reinterpret_cast<const uint16_t *>(&(tingymodel.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]));
				}

				if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor &uvAccessor = tingymodel.accessors[primitive.attributes.find("WEIGHTS_0")->second];
					const tinygltf::BufferView &uvView = tingymodel.bufferViews[uvAccessor.bufferView];
					bufferWeights = reinterpret_cast<const float *>(&(tingymodel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				}

				hasSkin = (bufferJoints && bufferWeights);

				vertexCount = static_cast<uint32_t>(posAccessor.count);

				for (size_t v = 0; v < posAccessor.count; v++) {
					Vertex vert{};
					vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
					vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
					vert.uv = bufferTexCoords ? glm::make_vec2(&bufferTexCoords[v * 2]) : glm::vec3(0.0f);
					if (bufferColors) {
						switch (numColorComponents) {
						case 3:
							vert.color = glm::vec4(glm::make_vec3(&bufferColors[v * 3]), 1.0f);
						case 4:
							vert.color = glm::make_vec4(&bufferColors[v * 4]);
						}
					}
					else
					{
						vert.color = glm::vec4(1.0f);
					}
					vert.tangent = bufferTangents ? glm::vec4(glm::make_vec4(&bufferTangents[v * 4])) : glm::vec4(0.0f);
					vert.joint0 = hasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * 4])) : glm::vec4(0.0f);
					vert.weight0 = hasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : glm::vec4(0.0f);
					model.vertices.push_back(vert);
					model.vertexSize++;
				}
			}
			//Indice
			{
				const tinygltf::Accessor& accessor = tingymodel.accessors[primitive.indices];
				const tinygltf::BufferView& bufferView = tingymodel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = tingymodel.buffers[bufferView.buffer];

				indexCount = static_cast<uint32_t>(accessor.count);

				switch (accessor.componentType)
				{
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
					uint32_t* buf = new uint32_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
					for (size_t index = 0; index < accessor.count; index++) {
						model.indices.push_back(buf[index] + vertexStart);
					}
					delete[] buf;
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
					uint16_t* buf = new uint16_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
					for (size_t index = 0; index < accessor.count; index++) {
						model.indices.push_back(buf[index] + vertexStart);
					}
					delete[] buf;
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
					uint8_t* buf = new uint8_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
					for (size_t index = 0; index < accessor.count; index++) {
						model.indices.push_back(buf[index] + vertexStart);
					}
					delete[] buf;
					break;
				}
				default:
					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
					return;
				}
				model.indiceSize += indexCount;
				
			}
			Mesh meshoffset{};
			meshoffset.indexOffset = indexStart;
			meshoffset.indiceCount = indexCount;
			meshoffset.vertexOffset = vertexStart;
			meshoffset.vertexCount = vertexCount;
			meshoffset.materialID = primitive.material > -1 ? primitive.material : model.materials.size()-1;
			meshdata.meshes.push_back(meshoffset);

			
		}
		model.meshdatas.push_back(meshdata);
		model.aabbMax = utils::math::VecMax(meshdata.aabbMax, model.aabbMax);
		model.aabbMin = utils::math::VecMin(meshdata.aabbMin, model.aabbMin);
		model.meshCount++;
		newNode->meshID = model.meshdatas.size() - 1;
		
		if (parent != nullptr) {
			parent->children.push_back(newNode);
		}
		else
		{
			model.nodes.push_back(newNode);
		}
		model.linearNodeHierarchy.push_back(newNode);
		
	}
}

bool asset::ModelManager::loadImageDataFunc(tinygltf::Image* image, const int imageIndex, std::string* error, std::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
	// KTX files will be handled by our own code
	if (image->uri.find_last_of(".") != std::string::npos) {
		if (image->uri.substr(image->uri.find_last_of(".") + 1) == "ktx") {
			return true;
		}
	}
	return tinygltf::LoadImageData(image, imageIndex, error, warning, req_width, req_height, bytes, size, userData);
	
}

bool asset::ModelManager::loadImageDataFuncEmpty(tinygltf::Image* image, const int imageIndex, std::string* error, std::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
	return true;
}


