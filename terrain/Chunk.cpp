#include "Chunk.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include "../utils/stuff/Log.h"
#include "../utils/ComputeShader.h"
#include "WorldManager.h"

#include "mc_lookupTable.h"
#include "mc_trasnLookupTable.h"
#include "TerrainUtils.h"

#include "../physics/CollisionMesh.h"

#include <algorithm>

Chunk::Chunk(WorldManager* manager, int chunkSize, int chunkHeight, float cellSize, glm::vec2 pos, unsigned int generationTicket)
	:m_manager(manager),
	m_size(chunkSize), m_height(chunkHeight), m_cellSize(cellSize), m_position(pos), m_shader(Shader("utils/shaders/VertexShader.vert", "utils/shaders/FragmentShader.frag")),
	m_generationTicket(generationTicket)
{
	createMesh();
}

void Chunk::fillSolidMap() {
	m_vertexVector.resize((m_size) * (m_height) * (m_size));
	m_densityVector.resize((m_size) * (m_height) * (m_size));
	// float isoSurface = 0.0f;

	for (int x = 0; x < m_size; x++) {
		for (int y = 0; y < m_height; y++) {
			for (int z = 0; z < m_size; z++) {
				glm::vec3 position = glm::vec3(x, y, z) * m_cellSize;
				// the algorithm for wheter the terrain is solid or not is defined here
				// int terr = TerrainUtils::isSolid(glm::vec3(m_position.x, 0, m_position.y) + position);
				// m_vertexVector[getIndex(x, y, z)] = terr;
				
				float density = TerrainUtils::surfaceDensityNoise(glm::vec3(m_position.x, 0, m_position.y) + position);
				// density -= TerrainUtils::caveDensityNoise(glm::vec3(m_position.x, 0, m_position.y) + position);
				m_densityVector[getIndex(x, y, z)] = density;
				// LOG(density);
				
				int is_solid = density > TerrainUtils::isoSurface;
				// m_vertexVector[getIndex(x, y, z)] = is_solid;
				// calculate the surface in unit cubes local to chunk
				if (is_solid) {
					int key = x + m_size * z;
					auto it = m_surfaceBuffer.find(key);
					if (it == m_surfaceBuffer.end()) {
						m_surfaceBuffer[key] = y;
						continue;
					}
					
					if (y > m_surfaceBuffer.at(key)) m_surfaceBuffer[key] = y;
				}
			}
		}
	}
}

void Chunk::fillBuffers()
{
	// logical operations
	// grid cell index
	// look up table -> index of the edges of the cube
	// from the edges of the cube, exctract the joining vertices position
	// construct the data for the triangle
	
	/// this is the skeleton code for generating different lod meshes, 
	/// i have disabled it for now because i'm focusing on another problem because i got bored
	/*
	int gridResolution = pow(2, m_generationTicket);
	int targetSize_x = (m_size - 1) / gridResolution;
	int targetSize_y = (m_height - 1) / gridResolution;

	// check the conditions on the near chunks to solve the sticthing problem
	glm::ivec2 v_top	(1, 0);
	glm::ivec2 v_bottom	(-1, 0);
	glm::ivec2 v_left	(0, -1);
	glm::ivec2 v_rigth	(0, 1);

	unsigned int top	= m_manager->getNearChunkLodState(m_position, v_top);
	unsigned int bottom = m_manager->getNearChunkLodState(m_position, v_bottom);
	unsigned int left	= m_manager->getNearChunkLodState(m_position, v_left);
	unsigned int right	= m_manager->getNearChunkLodState(m_position, v_rigth);

	bool topStitch		= false;
	bool bottomStitch	= false;
	bool leftStitch		= false;
	bool rightStitch	= false;

	if (top - m_generationTicket < 0 && top != -1) topStitch = true;
	if (bottom - m_generationTicket < 0 && bottom != -1) bottomStitch = true;
	if (left - m_generationTicket < 0 && left != -1) leftStitch = true;
	if (right - m_generationTicket < 0 && right != -1) rightStitch = true; 
	*/

	// disabled the resolution stuff for now
	int gridResolution = 1;
	int targetSize_x = (m_size - 1) / gridResolution;
	int targetSize_y = (m_height - 1) / gridResolution;
	for (int x = 0; x < targetSize_x; x++) {
		for (int y = 0; y < targetSize_y; y++) {
			for (int z = 0; z < targetSize_x; z++) {
				runMarchingCubes(glm::ivec3(x, y, z), gridResolution);
			}	
		}
	}

	m_collisionMesh = Physics::CollisionMesh(m_temporaryMesh);
	m_temporaryMesh.clear();
}

void Chunk::runMarchingCubes(glm::ivec3 position, int resolution)
{
	glm::ivec3 pos = position * resolution;
	int index = getGridCellIndex(pos, resolution);
	//std::vector<int> m_edgeIndexes = mc::lookUpTable[index];
	int* m_edgeIndexes = mc::lookUpTable[index].data();

	for (int i = 0; i < 16; i += 3) // at most 15 vertices per cube
	{
		if (m_edgeIndexes[i] == -1) break;
		// we use a tritable to keep track of the vertices of each triangle
		glm::vec3 triTable[3];

		// process each cube individually 
		for (int j = 0; j < 3; j++) {
			std::pair<int, int> vertexPositions = mc::edges[m_edgeIndexes[i + j]];

			// get the vertex position in local unit cube positions
			glm::vec3 positionA = mc::vertices[vertexPositions.first];
			glm::vec3 positionB = mc::vertices[vertexPositions.second];

			glm::vec3 voxelA = positionA + glm::vec3(position);
			glm::vec3 voxelB = positionB + glm::vec3(position);

			voxelA *= resolution;
			voxelB *= resolution;

			float density1 = m_densityVector[getIndex(voxelA.x, voxelA.y, voxelA.z)];
			float density2 = m_densityVector[getIndex(voxelB.x, voxelB.y, voxelB.z)];


			float step = 1.0f / TerrainUtils::step;
			// normal interpolation parameter
			float t = (TerrainUtils::isoSurface - density1) / (density2 - density1);
			// snap to nearest quantized step
			float quantizedT = round(t / step) * step;

			// compute the snapped vertex position
			glm::vec3 weighted = voxelA + quantizedT * (voxelB - voxelA);
			// glm::vec3 cubeWorldPosition = glm::vec3(x, y, z);
			// glm::vec3 worldPosition = glm::vec3(m_position.x, 0, m_position.y) + (cubeWorldPosition + weighted) * m_cellSize;
			glm::vec3 worldPosition = glm::vec3(m_position.x, 0, m_position.y) + weighted * m_cellSize;
			triTable[j] = (worldPosition);
		}

		// todo:: rotate based on orientation lod, for now the assumption is that
		// the vertices lie on the xy plane
		glm::vec3 tan1 = triTable[1] - triTable[0];
		glm::vec3 tan2 = triTable[2] - triTable[0];
		glm::vec3 normal = glm::normalize(glm::cross(tan1, tan2));

		for (int j = 0; j < 3; j++) {
			// finally pushing the positions to the buffer
			m_positionBuffer.push_back(triTable[j].x);
			m_positionBuffer.push_back(triTable[j].y);
			m_positionBuffer.push_back(triTable[j].z);

			// push the same normal for the position to implement flat shading
			m_normalBuffer.push_back(normal.x);
			m_normalBuffer.push_back(normal.y);
			m_normalBuffer.push_back(normal.z);
		}

		Physics::Triangle triangle(triTable[0], triTable[1], triTable[2]);
		m_temporaryMesh.push_back(triangle);
	}
}

// this isnt implemented yet.
// it is a transvoxel algorithm that should fix the sticthing lod issue
// on borders of chunks that have different resolution meshes
// what remains to do is to calculate the correct orientation of the cube indexes to be generated
// because for now the code assumes a convention orientation
// NOTE:: this code isnt tested yet, so there's the debug process to be made aswell
void Chunk::runTransvoxelCubes(glm::ivec3 position, int resolution) {
	glm::ivec3 pos = position * resolution;
	int index = getGridTransCellIndex(pos, resolution);
	unsigned char classIndex = mc::transitionCellClass[index];

	bool inverted = (classIndex & 0x80);
	unsigned char dataIndex = classIndex & 0x7F;

	mc::TransitionCellData data = mc::transitionCellData[dataIndex];
	
	const unsigned short* vertexInfo = mc::transitionVertexData[index];

	std::vector<glm::vec3> localVertices(data.GetVertexCount());
	for (int v = 0; v < data.GetVertexCount(); v++) {
		unsigned short code = vertexInfo[v];
		int edge = code & 0xFF;
		int cornerA = edge & 0x0F;
		int cornerB = (edge >> 4) & 0x0F;

		glm::vec3 posA = mc::mc_transVertices[cornerA] + glm::ivec3(position);
		glm::vec3 posB = mc::mc_transVertices[cornerB] + glm::ivec3(position);

		posA *= resolution;
		posB *= resolution;

		float d1 = m_densityVector[getIndex(posA.x, posA.y, posA.z)];
		float d2 = m_densityVector[getIndex(posB.x, posB.y, posB.z)];

		float t = (TerrainUtils::isoSurface - d1) / (d2 - d1);
		float quantizedT = round(t / TerrainUtils::step) * TerrainUtils::step;

		glm::vec3 weighted = posA + quantizedT * (posB - posA);
		glm::vec3 worldPos = glm::vec3(m_position.x, 0, m_position.y) + weighted * m_cellSize;

		localVertices[v] = worldPos;
	}

	// Step 3: assemble triangles using data.triangles
	for (int t = 0; t < data.GetTriangleCount(); t++) {
		int i0 = data.vertexIndex[t * 3 + 0];
		int i1 = data.vertexIndex[t * 3 + 1];
		int i2 = data.vertexIndex[t * 3 + 2];

		glm::vec3 v0 = localVertices[i0];
		glm::vec3 v1 = localVertices[i1];
		glm::vec3 v2 = localVertices[i2];

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
		if (inverted) std::swap(v1, v2); // fix winding

		// push into buffers
		auto pushVertex = [&](const glm::vec3& v) {
			m_positionBuffer.push_back(v.x);
			m_positionBuffer.push_back(v.y);
			m_positionBuffer.push_back(v.z);
			m_normalBuffer.push_back(normal.x);
			m_normalBuffer.push_back(normal.y);
			m_normalBuffer.push_back(normal.z);
		};

		pushVertex(v0);
		pushVertex(v1);
		pushVertex(v2);
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
	return x + z * m_size + y * m_size * m_size;
}

void Chunk::createMesh() {
	fillSolidMap();
	fillBuffers();
	setGlContext();
}

void Chunk::regenerateMesh() {
	m_vertexVector.clear();
	m_positionBuffer.clear();
	m_normalBuffer.clear();
	fillBuffers();
	setGlContext();
}

void Chunk::reset() {
	// cleanup
	m_vertexVector.clear();
	m_positionBuffer.clear();
	m_normalBuffer.clear();
	createMesh();
}

void Chunk::render(Shader& terrainShader, glm::mat4& projection, glm::mat4& view) {
	terrainShader.use();
	terrainShader.setMat4("projection", projection);
	terrainShader.setMat4("view", view);
	terrainShader.setVec4("color", glm::vec4(0.318, 0.255, 1.0f, 1.0));
	terrainShader.setVec3("lightColor", glm::vec3(0.6f, 0.0f, 0.8f));
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_positionBuffer.size() / 3);
	glBindVertexArray(0);
}

Physics::AABB Chunk::getAABB() const {
	glm::vec3 min(m_position.x, 0.0f, m_position.y);
	glm::vec3 max = min + glm::vec3(m_size, m_height, m_size) * m_cellSize;
	return Physics::AABB(min, max);
}

int Chunk::getGridCellIndex(glm::ivec3 position, int resolution) {
	//each grid cell has an index, that depends on how many vertices are solid or not
	// the index is calculated by a bit-wise or operation into an int
	int index = 0;
	for (int i = 0; i < 8; i++) {
		glm::vec3 lookUpPosition = position + mc::vertices[i] * resolution;
		int bit = (m_densityVector[getIndex(lookUpPosition.x, lookUpPosition.y, 
			lookUpPosition.z)] > TerrainUtils::isoSurface) ? 1 : 0 ;
		index |= (bit << i);
	}
	return index;
}

int Chunk::getGridTransCellIndex(glm::ivec3 position, int resolution) {
	// gets the vertices of the high resolution cube to map it to the low resolution one
	int index = 0; 
	for (int i = 0; i < 9; i++) {
		glm::ivec3 lookUpPosition = position + mc::mc_transVertices[i] * resolution;
		int bit = (m_densityVector[getIndex(lookUpPosition.x, lookUpPosition.y,
			lookUpPosition.z)] > TerrainUtils::isoSurface) ? 1 : 0;
		index |= (bit << i);
	}
	return index;
}
