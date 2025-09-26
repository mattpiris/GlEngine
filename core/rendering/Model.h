#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include "../../utils/Shader.h"
#include "../../utils/stuff/Log.h"
#include "Mesh.h"

// class that represents the object in opengl
// this doesn't take into consideration all the aspects that a game object
// is expected to have (es. physics), it's just a representation of the mesh
class Model
{
public:
	std::vector<Texture> m_loadedTextures;
	std::vector<Mesh> m_meshes;

	Model(std::string path);
	void render(Shader& shader, glm::mat4& projection, glm::mat4& view, glm::mat4& model);
private:
	std::string m_directory;


	void loadModel(std::string path);

	// each node processes the mesh and then calls recursively this function
	// in child nodes
	void processNode(aiNode* node, const aiScene* scene);
	
	// translates the assimp mesh data structure into the mesh class defined in the code here
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	// loads the textures of the mesh
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		std::string typeName);

    unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
};