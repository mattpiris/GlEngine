#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	// loading from files
	std::string vertexCode;
	std::string fragmentCode;

	std::ifstream vertexStream;
	std::ifstream fragmentStream;

	vertexStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		std::stringstream vertexStringStream, fragmentStringStream;
		vertexStream.open(vertexPath);
		fragmentStream.open(fragmentPath);

		vertexStringStream << vertexStream.rdbuf();
		fragmentStringStream << fragmentStream.rdbuf();

		vertexCode = vertexStringStream.str();
		fragmentCode = fragmentStringStream.str();
	}
	catch (const std::ifstream::ios_base::failure& e) {
		std::cout << "ERROR::SHADER::IO_FAILIURE" << std::endl << e.what() << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// creation of shaders and compilation errors
	int success;
	char infoLog[512];

	m_vertexId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_vertexId, 1, &vShaderCode, NULL);
	glCompileShader(m_vertexId);

	glGetShaderiv(m_vertexId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(m_vertexId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	m_fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_fragmentId, 1, &fShaderCode, NULL);
	glCompileShader(m_fragmentId);
	glGetShaderiv(m_fragmentId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(m_fragmentId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// creation of the program
	m_programId = glCreateProgram();
	glAttachShader(m_programId, m_vertexId);
	glAttachShader(m_programId, m_fragmentId);
	glLinkProgram(m_programId);

	glGetProgramiv(m_programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_programId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// clean-up
	glDeleteShader(m_vertexId);
	glDeleteShader(m_fragmentId);
}



void Shader::use() const {
	glUseProgram(m_programId);
}

unsigned int Shader::getId() const {
	return m_programId;
}

void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(m_programId, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(m_programId, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(m_programId, name.c_str()), value);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const {
	glUniformMatrix2fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
	glUniformMatrix3fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(glGetUniformLocation(m_programId, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const {
	glUniform2f(glGetUniformLocation(m_programId, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(m_programId, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(m_programId, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(glGetUniformLocation(m_programId, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(m_programId, name.c_str()), x, y, z, w);
}
