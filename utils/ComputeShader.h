#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

class ComputeShader
{
public:
	ComputeShader(const char* computePath, glm::uvec3 workSize, glm::vec3 localWorkSize);
	// creates an empty ssbo with the specified size
	// returns the id of the buffer created, used for reference for external user
	unsigned int createSSBO(size_t size, unsigned int bindingIndex);
	// creates a ssbo with the data at the specified index 
	// returns the id of the buffer created, used for reference for external user
	unsigned int setSSBOData(std::vector<float> data, unsigned int bindingIndex);
	// overwrites the m_texture3d property of this. 
	// the size should match the data size. it allows the specification in 3d format
	void setTexture3dData(std::vector<float> data, glm::vec3 size, unsigned int bindingIndex);
	// creates an atomic counter at the specified index
	// returns the id of the buffer created, used for reference for external user
	unsigned int createAtomicBuffer(unsigned int bindingIndex);
	void use();
	void dispatch();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

	void setMat2(const std::string& name, const glm::mat2& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;

	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w) const;
private:
	glm::uvec3 m_workSize;
	glm::vec3  m_localWorkSize;
	unsigned int m_programId;
	unsigned int m_texture3D;
	unsigned int atomic;
	unsigned int ssbo;
};