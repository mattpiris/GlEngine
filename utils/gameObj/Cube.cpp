#include "Cube.h"
#include <gtc/matrix_transform.hpp>

#include "../log/Log.h"
Cube::Cube(glm::vec3 cubePos, Shader* shader)
    :m_position(cubePos),
    m_model(glm::mat4(1.0f)),
    m_shader(shader),
    m_vao(0),
    m_vbo(0)
{
    m_vertexData = {
        // vertices             // normals
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
                             
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

       -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,

       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f
    };

    setGlContext();
}

void Cube::draw(glm::mat4& projection, glm::mat4& view) {
    if (!m_shader) {
        LOG("CUBE::DRAW::ERROR: attached shader doesnt exist");
        return;
    }
    
    m_shader->use();
    
    // glm::mat4 model = glm::mat4(1.0f);
    
    m_model = glm::translate(m_model, m_position);
    m_shader->setMat4("projection", projection);
    m_shader->setMat4("view", view);
    m_shader->setMat4("model", m_model);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_model = glm::mat4(1.0f);
}

void Cube::rotate(float angle, glm::vec3& axis) {
    m_model = glm::rotate(m_model, glm::radians(angle), axis);
}

void Cube::setGlContext()
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