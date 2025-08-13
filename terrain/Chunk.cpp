#include "Chunk.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <stb_perlin.h>
#include "../utils/log/Log.h"
#include "mc_lookupTable.h"

Chunk::Chunk(int chunkSize, float cellSize)
	:m_size(chunkSize), m_cellSize(cellSize), m_shader(Shader("utils/shaders/VertexShader.vert", "utils/shaders/FragmentShader.frag")),
	m_debug(false)
{
	createMesh();
}

void Chunk::enableDebug(bool debug) {
	m_debug = debug;
}

void Chunk::fillSolidMap() {
	m_vertexVector.resize(m_size * m_size * m_size);

	for (int x = 0; x < m_size; x++) {
		for (int y = 0; y < m_size; y++) {
			for (int z = 0; z < m_size; z++) {
				glm::vec3 position = glm::vec3(x, y, z) * m_cellSize;

				// i had to put a scale for perlin noise because if the cellsize
				// is an integer, the diff vector of the perlin noise under the hood is 
				// always zero
				float scale = 0.1;
				glm::vec3 perlinPos = position * scale;
				float perlin = stb_perlin_noise3(perlinPos.x, perlinPos.y, perlinPos.z, 0, 0, 0);
				int is_solid = (perlin > 0.0f) ? 1 : 0;

				// create an array that stores 0 and 1 for the solid terrain
				m_vertexVector[getIndex(x, y, z)] = is_solid;

				// debug stuff
				glm::vec3 color = glm::vec3(1.0f) * (float)!is_solid; // black if solid, white otherwise 
				float debugSize = 0.1f;
				m_debugCubes.push_back(Cube(position, &m_shader, debugSize, glm::vec4(color, 1.0f)));
			}
		}
	}
}

void Chunk::fillPositionBuffer()
{
	// logical operations
	// grid cell index
	// look up table -> index of the edges of the cube
	// from the edges of the cube, exctract the joining vertices position
	// construct the data for the triangle
	
	for (int x = 0; x < m_size - 1; x++) {
		for (int y = 0; y < m_size - 1; y++) {
			for (int z = 0; z < m_size - 1; z++) {
				int index = getGridCellIndex(glm::ivec3(x, y, z));
				std::vector<int> m_edgeIndexes = mc::lookUpTable[index];
				/*
				for (auto& edgeIndex : m_edgeIndexes) {
					// -1 means nothing to draw
					if (edgeIndex == -1) continue;
					std::pair<int, int> vertexPositions = mc::edges[edgeIndex];

					// get the vertex position
					glm::vec3 positionA = mc::vertices[vertexPositions.first];
					glm::vec3 positionB = mc::vertices[vertexPositions.second];
					// compute the mid point between these two positions
					glm::vec3 midPoint = (positionA + positionB) * 0.5f;

					glm::vec3 cubeWorldPosition = glm::vec3(x, y, z);
					glm::vec3 worldPosition = (cubeWorldPosition + midPoint) * m_cellSize;

					// finally pushing the positions to the buffer
					m_positionBuffer.push_back(worldPosition.x);
					m_positionBuffer.push_back(worldPosition.y);
					m_positionBuffer.push_back(worldPosition.z);
				}
				*/

				for (int i = 0; i < 16; i+= 3) 
				{
					if (m_edgeIndexes[i] == -1) break;
					std::vector<glm::vec3> triTable;
					
					for (int j = 0; j < 3; j++) {
						std::pair<int, int> vertexPositions = mc::edges[m_edgeIndexes[i + j]];

						// get the vertex position
						glm::vec3 positionA = mc::vertices[vertexPositions.first];
						glm::vec3 positionB = mc::vertices[vertexPositions.second];
						// compute the mid point between these two positions
						glm::vec3 midPoint = (positionA + positionB) * 0.5f;

						glm::vec3 cubeWorldPosition = glm::vec3(x, y, z);
						glm::vec3 worldPosition = (cubeWorldPosition + midPoint) * m_cellSize;

						triTable.push_back(worldPosition);
					}

					glm::vec3 tan1 = triTable[1] - triTable[0];
					glm::vec3 tan2 = triTable[2] - triTable[0];
					glm::vec3 normal = glm::normalize(glm::cross(tan1, tan2));

					for (int j = 0; j < 3; j++) {
						// finally pushing the positions to the buffer
						m_positionBuffer.push_back(triTable[j].x);
						m_positionBuffer.push_back(triTable[j].y);
						m_positionBuffer.push_back(triTable[j].z);
						
						m_normalBuffer.push_back(normal.x);
						m_normalBuffer.push_back(normal.y);
						m_normalBuffer.push_back(normal.z);
					}
				}
			}
		}
	}
}

void Chunk::fillNormalBuffer()
{
	for (int x = 1; x < m_size - 1; ++x) {
		for (int y = 1; y < m_size - 1; ++y) {
			for (int z = 1; z < m_size - 1; ++z) {
				float dx = stb_perlin_noise3(x + 1, y, z, 0, 0, 0) - stb_perlin_noise3(x - 1, y, z, 0, 0, 0);
				float dy = stb_perlin_noise3(x, y + 1, z, 0, 0, 0) - stb_perlin_noise3(x, y - 1, z, 0, 0, 0);
				float dz = stb_perlin_noise3(x, y, z + 1, 0, 0, 0) - stb_perlin_noise3(x, y, z - 1, 0, 0, 0);
				glm::vec3 normal = glm::normalize(glm::vec3(dx, dy, dz));
				
				m_normalBuffer.push_back(normal.x);
				m_normalBuffer.push_back(normal.y);
				m_normalBuffer.push_back(normal.z);
			}
		}
	}
}

void Chunk::setGlContext() {
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_positionvbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_positionvbo);
	glBufferData(GL_ARRAY_BUFFER, m_positionBuffer.size() * sizeof(float), m_positionBuffer.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &m_normalvbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalvbo);
	glBufferData(GL_ARRAY_BUFFER, m_normalBuffer.size() * sizeof(float), m_normalBuffer.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int Chunk::getIndex(int x, int y, int z) const {
	return x + y * m_size + z * m_size * m_size;
}

void Chunk::createMesh() {
	fillSolidMap();
	fillPositionBuffer();
	// fillNormalBuffer();
	setGlContext();
}

void Chunk::render(Shader& terrainShader, glm::mat4& projection, glm::mat4& view) {
	if (m_debug) {
		for (auto& cube : m_debugCubes) {
			cube.draw(projection, view);
		}
	}

	terrainShader.use();
	terrainShader.setMat4("projection", projection);
	terrainShader.setMat4("view", view);
	terrainShader.setVec4("color", glm::vec4(0.718, 0.255, 0.055, 1.0));
	terrainShader.setVec3("lightColor", glm::vec3(0.6f, 0.0f, 0.8f));
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_positionBuffer.size() / 3);
}

int Chunk::getGridCellIndex(glm::ivec3 position) {
	//each grid cell has an index, that depends on how many vertices are solid or not
	// the index is calculated by a bit-wise or operation into an int
	int index = 0;
	for (int i = 0; i < 8; i++) {
		glm::vec3 lookUpPosition = position + mc::vertices[i];
		int bit = m_vertexVector[getIndex(lookUpPosition.x, lookUpPosition.y, lookUpPosition.z)];
		index |= (bit << i);
	}
	return index;
}