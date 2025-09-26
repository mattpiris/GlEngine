#pragma once
#include "../../core/rendering/Model.h"
#include "Chunk.h"
#include "AssetsList.h"

struct ResourceObjectInfo
{
	const std::shared_ptr<Model> model;
	const int minSpawnHeight; // in cube units
	const int maxSpawnHeight; // relative to world grid
	const float density; 
	const float max_scale;
	const float min_scale;

	ResourceObjectInfo(std::shared_ptr<Model> model, int minSpawnHeight, int maxSpawnHeight, 
		float max_scale, float min_scale, float density)
		:model(model), minSpawnHeight(minSpawnHeight), maxSpawnHeight(maxSpawnHeight),
		max_scale(max_scale), min_scale(min_scale), density(density)
	{}
};

class ResourcesManager
{
public:
	static ResourcesManager& getInstance() {
		static ResourcesManager instance;
		return instance;
	}

	// based on the resource, it retrieves a vector of valid mat4 positions
	// for the objects to spawn
	static std::vector<glm::mat4> spawnObjects(ResourceObjectInfo& resource, const Chunk& chunk);
	
	void loadModel(AssetID, std::string path);
	std::shared_ptr<Model> getModel(AssetID id);
private:
	std::unordered_map<unsigned int, std::shared_ptr<Model>> m_modelMap;
};