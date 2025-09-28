#include "Plane.h"
#include "../stuff/Log.h"

Plane::Plane(glm::vec3 position, Shader& shader, float size, glm::vec4 color)
	:IGameObject(position, shader, size, color), m_vao(0), m_vbo(0)
{
    m_vertexData =
    {
        // positions            // normals        
        -0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,  
        0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,  
        0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,  

        -0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f, 
         0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,  
        -0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,
    };


	setGlContext();
}

void Plane::draw(glm::mat4& projection, glm::mat4& view) {
    m_shader.use();

    // glm::mat4 model = glm::mat4(1.0f);

    m_model = glm::translate(m_model, m_position);
    

    m_shader.setMat4("projection", projection);
    m_shader.setMat4("view", view);
    m_shader.setMat4("model", m_model);
    m_shader.setVec4("color", m_color);
    m_shader.setFloat("size", m_size);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_model = glm::mat4(1.0f);
}

void Plane::setGlContext()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertexData.size(), m_vertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}
