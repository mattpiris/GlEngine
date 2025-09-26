#include "ResourcesManager.h"
#include <random>
#include "../utils/stuff/Log.h"


std::vector<glm::mat4> ResourcesManager::spawnObjects(ResourceObjectInfo& resource, const Chunk& chunk) {
    std::vector<glm::mat4> positionBuffer;
    int numSpawns = resource.density * chunk.getChunkSize() * chunk.getChunkSize();
    
    
    for (int i = 0; i < numSpawns; i++) {
        // get a random (x, z) position
        int x = rand() % chunk.getChunkSize();
        int z = rand() % chunk.getChunkSize();
        int y = -1;
        
        float x_offset = (float)rand() / RAND_MAX * 2.0f;
        float z_offset = (float)rand() / RAND_MAX * 2.0f;
        
        float angle = rand() % 360;
        float scale = ((float)rand() / RAND_MAX) * 
            (resource.max_scale - resource.min_scale) + resource.min_scale;
        
        // confront with the max height 
        // the index should match with the one in chunk.cpp
        int chunkGridIndex = x + chunk.getChunkSize() * z;
        auto surfaceBuffer = chunk.getSurfaceBuffer();
        int candidate_y = surfaceBuffer[chunkGridIndex];
        // the vector of the vector should be sorted in decreasing order
        // with the first element being the greatest, the last one the least

        if (candidate_y > resource.minSpawnHeight &&
            candidate_y < resource.maxSpawnHeight) {
            y = candidate_y;
        }
            
        // if a valid y position in found, create the position buffer
        if (y != -1) {
            glm::mat4 position(1.0f);
            float cellSize = chunk.getCellSize();
            glm::vec3 localChunkPosition(x * cellSize, y * cellSize, z * cellSize);
            glm::vec3 worldPosition = glm::vec3(chunk.getChunkPosition().x + x_offset, 0, chunk.getChunkPosition().y + z_offset) + localChunkPosition;
            position = glm::translate(position, worldPosition);
            position = glm::scale(position, glm::vec3(scale));
            position = glm::rotate(position, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
            positionBuffer.push_back(position);
        }
    }

    return positionBuffer;
}

void ResourcesManager::loadModel(AssetID id, std::string path) {
    unsigned int key = static_cast<unsigned int>(id);
    if (m_modelMap.find(key) != m_modelMap.end()) {
        LOG("RESOURCE_MANAGER::WARNING:: map already contains a model for the id:");
        LOG(key);
        return;
    }

    m_modelMap[key] = std::make_shared<Model>(path);
}

std::shared_ptr<Model> ResourcesManager::getModel(AssetID id) {
    unsigned int key = static_cast<unsigned int>(id);
    if (m_modelMap.find(key) == m_modelMap.end()) {
        LOG("RESOURCE_MANAGER::ERROR:: model should be loaded before requesting the data");
        return nullptr;
    }

    return m_modelMap[key];
}
