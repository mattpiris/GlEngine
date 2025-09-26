#include "BatchRenderer.h"

BatchRenderer::BatchRenderer(std::shared_ptr<Model> model, std::vector<glm::mat4>& positions)
	:m_positions(positions), m_model(model)
{
	// sets up vao for the model
    unsigned int pos_vbo;
    glGenBuffers(1, &pos_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_positions.size()
        * sizeof(glm::mat4), m_positions.data(), GL_DYNAMIC_DRAW);

    for (unsigned int i = 0; i < model->m_meshes.size(); i++) {
        unsigned int vao = model->m_meshes[i].m_vao;
        glBindVertexArray(vao);

        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 4, (void*)0);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 4, (void*)(sizeof(glm::vec4) * 1));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 4, (void*)(sizeof(glm::vec4) * 2));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 4, (void*)(sizeof(glm::vec4) * 3));
        glEnableVertexAttribArray(6);

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
}

void BatchRenderer::render(Shader& shader, glm::mat4& projection, glm::mat4& view)
{
	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

	// bind all the relevant textures
	for (unsigned int i = 0; i < m_model->m_meshes.size(); i++) {
		unsigned int n_diffuse = 1;
		unsigned int n_specular = 1;
		for (unsigned int j = 0; j < m_model->m_meshes[i].m_textures.size(); j++) {
			glActiveTexture(GL_TEXTURE0 + j);

			std::string texNumber;
			std::string textureName = m_model->m_meshes[i].m_textures[j].type;

			if (textureName == "texture_diffuse") {
				texNumber = std::to_string(n_diffuse++);
			}
			else if (textureName == "texture_specular") {
				texNumber = std::to_string(n_specular++);
			}

			shader.setInt(("material." + textureName + texNumber).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, m_model->m_meshes[i].m_textures[j].id);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(m_model->m_meshes[i].m_vao);
		glDrawElementsInstanced(GL_TRIANGLES, m_model->m_meshes[i].m_indices.size(), GL_UNSIGNED_INT, 0, m_positions.size());
		glBindVertexArray(0);
	}
}
