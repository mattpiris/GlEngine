#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "../utils/Shader.h"
#include "Chunk.h"
#include "../../core/rendering/Model.h"
#include "../core/rendering/BatchRenderer.h"
#include "ResourcesManager.h"


// this class manages the world for now
// this means that this should handle the chunk rendering/resources related things
class WorldManager
{
public:
	WorldManager(int n_chunks, int m_ChunkHeight, int chunkSize, glm::vec3 viewerStartingPos, float viewDistance);
	void renderTerrain(Shader& shader, glm::mat4 projection, glm::mat4 view);
	void renderVegetation(Shader& shader, glm::mat4 projection, glm::mat4 view);
	// this function is used by chunk to adjust the stitches near the edge
	// the position is the chunk position, the direction is the direction the world manager should look at
	unsigned int getNearChunkLodState(glm::vec2 position, glm::ivec2 direction);
	void TerrainImGui();
private:
	// later all the information about the chunks could be stored here
	// std::vector<Chunk> m_worldMap;
	std::vector<std::shared_ptr<Chunk>> m_worldMap;
	std::unordered_map<int, std::shared_ptr<Chunk>> m_chunkIdMap;
	
	std::unordered_map<AssetID, BatchRenderer> m_batches;
	std::unordered_map<unsigned int, std::vector<Chunk>> m_LodMap;

	int m_numChunks;
	int m_chunkHeight;
	int m_chunkSize;

	std::vector<unsigned int> m_postionvbos;
	std::vector<glm::mat4> m_positions;
	// retrieves the hash of the chunk position coordinates to map to the id map
	// it works with negative positions aswell
	size_t chunkIndex(int x, int z) const;
	// returns the generation ticket of the chunk based on the distance bewteen the view position 
	// and the chunk position 
	unsigned int getGenerationTicket(glm::vec3 viewPosition, glm::vec3 targetChunkPosition);

	void createVegetation();
	BatchRenderer createInstances(ResourceObjectInfo& info);
};