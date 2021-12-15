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
#include <iostream>

#include "shader.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture {
	unsigned int id = 0;
	std::string type;
	aiString path;
};

class Mesh {
public:
	//Mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//Function
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(shader shader);

private:
	//Rendering data
	unsigned int VAO, VBO, EBO;

	//Function
	void setupMesh();
};