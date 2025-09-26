#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "../Shader.h"

class IGameObject
{
public:
	IGameObject(glm::vec3 position, Shader* shader, float size = 1.0f, glm::vec4 color = glm::vec4(1.0f))
		:m_position(position), m_shader(shader), m_size(size), m_color(color), m_model(glm::mat4(1.0f)) {}
	// this function updates the projection and view matrix in the shaders,
	// therefore all the other uniform variables 
	// need to be updated before calling this function
	// the model matrix is a private member of this class, its managed internally
	virtual void draw(glm::mat4& projection, glm::mat4& view) = 0;
	// the angle is meant to be in degrees, NOT radians
	virtual void rotate(float angle, glm::vec3& axis) {
		m_model = glm::rotate(m_model, glm::radians(angle), axis);
	}
protected:
	Shader* m_shader;
	glm::vec3 m_position;
	glm::mat4 m_model;
	glm::vec4 m_color;
	float m_size;

	virtual void setGlContext() = 0;
};