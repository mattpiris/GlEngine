#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../utils/gameObj/Cube.h"
#include "../utils/Shader.h"

class Chunk
{
public:
	Chunk(int chunkSize, float cellSize);
	void enableDebug(bool debug);
	void render(Shader& terrainShader, glm::mat4& projection, glm::mat4& view);
private:
	int m_size;
	float m_cellSize;
	std::vector<int> m_vertexVector; // stores the information about if an edge is solid or not
	std::vector<Cube> m_debugCubes;
	std::vector<float> m_positionBuffer; // for positionvbo, contains all the data position for the triangles
	std::vector<float> m_normalBuffer;

	Shader m_shader;

	unsigned int m_vao;
	unsigned int m_positionvbo;
	unsigned int m_normalvbo;

	bool m_debug;
	int getIndex(int x, int y, int z) const;
	void createMesh();
	// this method fills the vertex vector
	void fillSolidMap();
	// fills the positionBuffer with all the triangles position data
	void fillPositionBuffer();
	void fillNormalBuffer();
	void setGlContext(); // self-explanatory
	// gets the index of the grid cells at the specified position for the look up table
	// NOTE: the vector MUST be in unit cubes, hence its components should be integers
	int getGridCellIndex(glm::ivec3 position);
};