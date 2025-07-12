#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <vector>

#include "../Shader.h"

class Cube
{
public:
	Cube(glm::vec3 cubePos, Shader* cubeShader);
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
	
	std::vector<float> m_vertexData;

	unsigned int m_vao;
	unsigned int m_vbo;

	void setGlContext();
};