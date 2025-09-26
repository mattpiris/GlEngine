#pragma once
#include <glm/glm.hpp>
#include <vector>

class ComputeShaderEx
{
public:
	ComputeShaderEx(const char* computePath, glm::uvec3 workSize);
	
	// launches the size work groups
	void dispatch();
	void use();

	// blocks the program until the compute has finished processing the data
	void process();
	void setData(std::vector<float> newData);
	std::vector<float> getData();
private:
	glm::uvec3 m_workSize;
	unsigned int m_programId;
	unsigned int m_textureId;
};