#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "IGameObject.h"
#include "../Shader.h"

class Plane : public IGameObject
{
public:
	Plane(glm::vec3 position, Shader& shader, float size = 1.0f, glm::vec4 color = glm::vec4(1.0f));
	virtual void draw(glm::mat4& projection, glm::mat4& view) override;
private:
	std::vector<float> m_vertexData;
	unsigned int m_vao;
	unsigned int m_vbo;

	virtual void setGlContext() override;
};