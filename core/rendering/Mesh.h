#pragma once
#include <glm/glm.hpp>
#include "../../utils/Shader.h"
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

struct Texture
{
	unsigned int id;
	std::string type;
	aiString path;
};

class Mesh
{
public:
	std::vector<Vertex> m_vertices;
	std::vector<Texture> m_textures;
	std::vector<unsigned int> m_indices;
	unsigned int m_vao, m_vbo, m_ebo;

	Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices);
	void render(Shader& shader);

private:
	void createMesh();
};