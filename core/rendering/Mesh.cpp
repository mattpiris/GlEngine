#include "Mesh.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices)
	:m_vertices(vertices), m_textures(textures), m_indices(indices)
{
	createMesh();
}

void Mesh::createMesh() {
	// setting up the gl context
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::render(Shader& shader)
{
	unsigned int n_diffuse = 1;
	unsigned int n_specular = 1;


	for (int i = 0; i < m_textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
	
		// in the shader there's a naming convention for the textures
		// we have some uniforms defined as follows
		// uniform texture2D texture_diffuse1;
		// uniform texture2D texture_diffuse2;
		// ...
		// uniform texture2D texture_diffuseN;

		// same thing with texture_specular
		// also we organize the texture data into a struct called material in the shaders
		// this way the shader programming is independent from the cpu-side programming
		std::string texNumber;
		std::string textureName = m_textures[i].type;
		
		if (textureName == "texture_diffuse") {
			texNumber = std::to_string(n_diffuse++);
		}
		else if (textureName == "texture_specular") {
			texNumber = std::to_string(n_specular++);
		}

		shader.setInt(("material." + textureName + texNumber).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}

	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}