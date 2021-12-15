#pragma once

#include <glad\glad.h>

#include <GLFW\glfw3.h>
#include <stb\stb_image.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>


#include <string>
#include <vector>


#include "mesh.h"
#include "shader.h"

unsigned int TextureFromFile(const char* path, const std::string& directory);

class Model {
public:
	//Model data
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;

	//Function
	Model(const char* path) { loadModel(path); }
	void Draw(shader shader);

private:

	//Function
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};