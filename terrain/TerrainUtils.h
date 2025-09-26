#pragma once
#include <glm/vec3.hpp>

// class that helps for terrain-related methods that need to be abstracted away 
// into their own classes
class TerrainUtils
{
public:
	static int isSolid(glm::vec3 position);
	// returns the density of the node at the requested position
	// used for weighted marching cubes
	static float surfaceDensityNoise(glm::vec3 position);
	static float caveDensityNoise(glm::vec3 position);

	static float octavePerlin(glm::vec3 position, int octaves, float frequency_gain, float amplitude_gain);
	
	static bool drawImgUI();

	// settings for terrain generation
	static int step;
	static int baseHeight;
	static int terrainEnd;
	static float terrainAmplitude;
	static int power;
	static float caveDensity;
	static float caveStrenght;
	static float caveScale;
	static float scale;
	static float isoSurface;
};