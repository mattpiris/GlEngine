#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include "../utils/gameObj/Cube.h"
#include "../utils/Shader.h"

class WorldManager;

struct TerrainVertex
{
	glm::vec3 position;
	glm::vec3 normal;
};


class Chunk
{
public:
	Chunk() = default;
	Chunk(WorldManager* manager, int chunkSize, int chunkHeight, float cellSize, glm::vec2 position, unsigned int generationTicket);
	// this function runs the terrain algorithm again so you should not call it every frame
	// it is meant to be called only for debug purposes to test the parameters
	void reset(); 
	void render(Shader& terrainShader, glm::mat4& projection, glm::mat4& view);

	// getters
	// returns the world position of the chunk
	glm::vec2 getChunkPosition() const						{ return m_position; }
	std::unordered_map<int, int> getSurfaceBuffer()  const	{ return m_surfaceBuffer; }
 	
	unsigned int getGenerationTicket() const { return m_generationTicket; }
	int getChunkSize() const { return m_size; }
	float getCellSize() const { return m_cellSize; }

private:
	WorldManager* m_manager;
	unsigned int m_generationTicket;
	int m_size;
	int m_height;
	float m_cellSize;
	glm::vec2 m_position;

	// stores the information about if an edge is solid or not
	std::vector<int> m_vertexVector; /// should be int, changed for test purposes
	std::vector<float> m_densityVector;
	
	// given a vec2 in unit cubes local to the chunk, it retrieves the y coordinate
	// representing the surface
	std::unordered_map<int, int> m_surfaceBuffer;

	std::vector<float> m_positionBuffer; // for positionvbo, contains all the data position for the triangles
	std::vector<float> m_normalBuffer;

	Shader m_shader;

	unsigned int m_vao;


	unsigned int m_positionvbo;
	unsigned int m_normalvbo;

	int getIndex(int x, int y, int z) const;
	void createMesh();
	// this method fills the vertex vector
	void fillSolidMap();
	// fills the positionBuffer with all the triangles position data
	void fillBuffers();
	void setGlContext(); // self-explanatory
	// gets the index of the grid cells at the specified position for the look up table
	// NOTE: the vector MUST be in unit cubes, hence its components should be integers
	int getGridCellIndex(glm::ivec3 position, int resolution);
	// gets the index of the grid cells at the specified position for the look up table
	// NOTE: the vector MUST be in unit cubes, hence its components should be integers
	// it is used to calculate the index of the cube configuration for the trasnvoxel algorithm
	int getGridTransCellIndex(glm::ivec3 position, int resolution);
	// the position passed as parameter should be the index in the x, y, z iteration in the for loop
	// the resolution parameter is the resolution of the cubes in the grid, must be a multiple of 2
	void runMarchingCubes(glm::ivec3 position, int resolution);
	// the position passed as parameter should be the index in the x, y, z iteration in the for loop
	// the resolution parameter is the resolution of the cubes in the grid, must be a multiple of 2
	// this algorithm solves the issue of the stitching when changing the lod
	void runTransvoxelCubes(glm::ivec3, int resolution);
};