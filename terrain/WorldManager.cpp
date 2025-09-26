#include "WorldManager.h"
#include "TerrainUtils.h"
#include "ResourcesManager.h"
#include "../core/rendering/BatchRenderer.h"
#include "../utils/stuff/Log.h"
#include <thread>

float cellSize = 1.0f;
WorldManager::WorldManager(int n_chunks, int chunkHeight, int chunkSize, glm::vec3 viewerStartingPos, float viewDistance)
    :m_numChunks(n_chunks), m_chunkHeight(chunkHeight), m_chunkSize(chunkSize)
{
    // creation of worldMap
    for (int x = 0; x < n_chunks; x++) {
            for (int z = 0; z < n_chunks; z++) {
                glm::vec2 chunkPos = glm::vec2(static_cast<float>(x * (chunkSize) * cellSize), static_cast<float>(z * (chunkSize) * cellSize));
                // calculate the generation id
                // pass it to the cunk to generate its own mesh
                unsigned int generationTicket = getGenerationTicket(viewerStartingPos, glm::vec3(chunkPos.x, 0, chunkPos.y));
                std::shared_ptr<Chunk> ptr = std::make_shared<Chunk>(this, m_chunkSize + 1, m_chunkHeight, cellSize, chunkPos, generationTicket);
                m_worldMap.push_back(ptr);
                
                // i had the problem to map a unique id to the chunk
                // it wouldnt work with negative positions as i would get a negative id potentially
                // which gave me a problem when thinking about adding chunks to negative positions
                // i mapped the id of the hashed chunk position to the pointer, solving that problem
                // i dont know if with negative positions the index looses its uniqueness
                size_t index = chunkIndex(x, z);
                m_chunkIdMap[index] = ptr;
            }
    }


    // createVegetation();
}

void WorldManager::renderTerrain(Shader& shader, glm::mat4 projection, glm::mat4 view) {
    for (auto& chunk : m_worldMap) {
        chunk->render(shader, projection, view);
    }
}

void WorldManager::renderVegetation(Shader& shader, glm::mat4 projection, glm::mat4 view) {
    for (auto& batch : m_batches) {
        batch.second.render(shader, projection, view);
    }
}

unsigned int WorldManager::getNearChunkLodState(glm::vec2 position, glm::ivec2 direction) {
    glm::vec2 chunkUnitPosition = position / (m_chunkSize * cellSize);
    glm::ivec2 target((int)(chunkUnitPosition.x + direction.x), (int)(chunkUnitPosition.y + direction.y));
    size_t key = chunkIndex(target.x, target.y);
    
    if (!m_chunkIdMap.contains(key)) {
        return -1;
    }
    return m_chunkIdMap[key]->getGenerationTicket();
}

void WorldManager::TerrainImGui() {
    bool settings = TerrainUtils::drawImgUI();
    if (settings) {
        for (auto& chunk : m_worldMap) {
            chunk.reset();
        }
    }
}

size_t WorldManager::chunkIndex(int x, int z) const {
    // cantor pairing variant
    auto hash = std::hash<long long>{}(((long long)x << 32) ^ (long long)z);
    return hash;
}

unsigned int WorldManager::getGenerationTicket(glm::vec3 viewPosition, glm::vec3 targetChunkPosition) {
    // define the generation system properties
    // could refine this later with a better system to keep track of the levels of detail
    int level0 = 10;
    int level1 = 5;
    int level2 = 5;
    
    int lod0 = level0;
    int lod1 = level0 + level1;
    int lod2 = level0 + level1 + level2;
    
    // calculates the distance between the chunk position and the view position in chunk units
    glm::vec3 distance = viewPosition - targetChunkPosition;
    int chunkUnits = static_cast<int>(std::round(glm::length(distance) / m_chunkSize));

    // four level of details for now
    if (chunkUnits < lod0) return 0;
    if (chunkUnits < lod1) return 1;
    if (chunkUnits < lod2) return 2;
    return 3;
}

void WorldManager::createVegetation() {
    auto grassModel = ResourcesManager::getInstance().getModel(AssetID::GRASS_WISPY_TALL);
    auto tree1Model = ResourcesManager::getInstance().getModel(AssetID::TWISTED_TREE1);
    auto tree2Model = ResourcesManager::getInstance().getModel(AssetID::TWISTED_TREE2);
    auto tree3Model = ResourcesManager::getInstance().getModel(AssetID::TWISTED_TREE3);
    auto tree4Model = ResourcesManager::getInstance().getModel(AssetID::TWISTED_TREE4);
    
    unsigned int maxTreeSpawn = 30;
    unsigned int minTreeSpawn = 10;
    float maxScale = 1.5f;
    float minScale = 0.7f;
    float density = (float)1 / (m_chunkSize * m_chunkSize);

    ResourceObjectInfo grassInfo(grassModel, 10, 30, 2.0f, .5f, 0.3f);
    ResourceObjectInfo tree1Info(tree1Model, minTreeSpawn, maxTreeSpawn, maxScale, minScale, density);
    ResourceObjectInfo tree2Info(tree2Model, minTreeSpawn, maxTreeSpawn, maxScale, minScale, density);
    ResourceObjectInfo tree3Info(tree3Model, minTreeSpawn, maxTreeSpawn, maxScale, minScale, density);
    ResourceObjectInfo tree4Info(tree4Model, minTreeSpawn, maxTreeSpawn, maxScale, minScale, density);
    
    m_batches[AssetID::GRASS_WISPY_TALL] = createInstances(grassInfo);
    m_batches[AssetID::TWISTED_TREE1] = createInstances(tree1Info);
    // m_batches[AssetID::TWISTED_TREE2] = createInstances(tree2Info);
    // m_batches[AssetID::TWISTED_TREE3] = createInstances(tree3Info);
    // m_batches[AssetID::TWISTED_TREE4] = createInstances(tree4Info);
}

BatchRenderer WorldManager::createInstances(ResourceObjectInfo& info) {
    std::vector<glm::mat4> positions;
    
    // create positions vector
    for (unsigned int j = 0; j < m_worldMap.size(); j++) {
        std::vector<glm::mat4> local = ResourcesManager::spawnObjects(info, *m_worldMap[j]);
        positions.insert(positions.end(), std::make_move_iterator(local.begin()),
            std::make_move_iterator(local.end()));
    }

    return BatchRenderer(info.model, positions);
}
