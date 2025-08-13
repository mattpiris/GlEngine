#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "../Shader.h"

/// A possible improvement to this class is having a sort of debug geometry manager
/// that uses instanced rendering for every cube drawn, just for performance
class Cube
{
public:
	Cube(glm::vec3 cubePos, Shader* cubeShader, float size = 1.0f, glm::vec4 color = glm::vec4(1.0f));
	// this function updates the projection and view matrix in the shaders,
	// therefore all the other uniform variables 
	// need to be updated before calling this function
	// the model matrix is a private member of this class, its managed internally
	void draw(glm::mat4& projection, glm::mat4& view);
	// the angle is meant to be in degrees, NOT radians
	void rotate(float angle, glm::vec3& axis);
private:
	Shader* m_shader;
	glm::vec3 m_position;
	glm::mat4 m_model;
	glm::vec4 m_color;
	float m_size;

	std::vector<float> m_vertexData;

	unsigned int m_vao;
	unsigned int m_vbo;

	void setGlContext();
};