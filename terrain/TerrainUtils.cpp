#include "TerrainUtils.h"
#include "stb_perlin.h"
#include "FastNoise.h"
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int		TerrainUtils::step				= 5;
int		TerrainUtils::baseHeight		= 20;
int		TerrainUtils::terrainEnd		= 1;
float	TerrainUtils::terrainAmplitude	= 881.0f;
int		TerrainUtils::power				= 8;

float	TerrainUtils::caveDensity		= 0.510f;
float	TerrainUtils::caveStrenght		= 20.0f;
float	TerrainUtils::caveScale			= 0.05f;
float	TerrainUtils::scale				= 0.7f;
float	TerrainUtils::isoSurface		= 0.0f;

int TerrainUtils::isSolid(glm::vec3 position) {
	// glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z) * size;
	if (position.y <= terrainEnd) return 1;

	// i had to put a scale for perlin noise because if the cellsize
	// is an integer, the diff vector of the perlin noise under the hood is 
	// always zero
	glm::vec3 perlinPos = position * scale;

	// is overworld solid
	int is_terrainSolid;
	// float overworldPerlin = stb_perlin_noise3(perlinPos.x, 0, perlinPos.z, 0, 0, 0);
	float overworldPerlin = octavePerlin(glm::vec3(perlinPos.x, 0, perlinPos.z), 3, 2.0f, 0.5f);
	float height = baseHeight + overworldPerlin * terrainAmplitude;

	is_terrainSolid = (position.y < height) ? 1 : 0;

	// carving out caves
	float cavePerlin = stb_perlin_noise3(perlinPos.x, perlinPos.y, perlinPos.z, 0, 0, 0);
	int is_caveSolid = (cavePerlin > caveDensity) ? 1 : 0;

	int is_solid = is_terrainSolid & is_caveSolid;

	return is_solid;
}

float TerrainUtils::surfaceDensityNoise(glm::vec3 position) {
	if (position.y < terrainEnd) return 1.0f;
	glm::vec3 perlinPos = position * scale;
	/*
	float noise = octavePerlin(glm::vec3(perlinPos.x, 0, perlinPos.z), 5, 2.0f, 0.5f);
	noise = (noise + 1.0f) / 2.0f;
	float height = baseHeight + noise * terrainAmplitude;
	*/
	FastNoiseLite noiseLite;
	noiseLite.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noiseLite.SetSeed(1337);
	noiseLite.SetFrequency(0.013);
	noiseLite.SetFractalType(FastNoiseLite::FractalType_FBm);
	noiseLite.SetFractalOctaves(5);
	noiseLite.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean);
	// noise = (position.y < height) ? noise : 0;
	float noise = noiseLite.GetNoise(position.x * scale, position.z * scale);
	noise = (noise + 1.0f) / 2.0f;
	noise = pow(noise, power);
	float height = baseHeight + noise * terrainAmplitude;
	float density = height - position.y;
	// noise = (position.y < height) ? noise : (noise - 1.0f);

	// if we're above the ground, we return the density
	if (position.y > height - 10.0f) {
		float depth = height - position.y;
		float caveFactor = glm::clamp(depth / 10.0f, 0.0f, 1.0f);
		float underground = caveDensityNoise(position);
		float erosion = noiseLite.GetNoise(position.x * caveScale, position.y * caveScale) * 2.0f;
		return density - glm::abs(caveFactor * caveStrenght * underground) - erosion;
	}

	// carving out caves
	float underground = caveDensityNoise(position);
	if (underground < caveDensity) {
		density = underground * caveStrenght;
	}
	return density;
}

// range [-1, 1]
float TerrainUtils::caveDensityNoise(glm::vec3 position) {
	glm::vec3 perlinPos = position * caveScale;
	float caveNoise = stb_perlin_noise3(perlinPos.x, perlinPos.y, perlinPos.z, 0, 0, 0);
	//caveNoise = (caveNoise + 1.0f) / 2.0f;
	// caveNoise = (caveNoise < caveDensity) ? caveNoise : 0.0f;
	return caveNoise;
}

// range [-1, 1]
float TerrainUtils::octavePerlin(glm::vec3 position, int octaves, float frequency_gain, float amplitude_gain)
{
	float noise = 0.0f; 
	float max = 0.0f;
	float amplitude = 1.0f;
	float frequency = 1.0f;
	for (int i = 0; i < octaves; i++) {
		noise += stb_perlin_noise3(position.x * frequency, position.y * frequency,
			position.z, 0, 0, 0) * amplitude;
		
		max += amplitude;
		amplitude *= amplitude_gain;
		frequency *= frequency_gain;
	}

	return noise / max;
}

bool TerrainUtils::drawImgUI() {
	ImGui::SetNextWindowPos(ImVec2(10, 100), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(400, 300));
	ImGui::Begin("Terrain Controls");
	bool setting = false;
	ImGui::SliderInt("Cube resolution", &TerrainUtils::step, 1, 10);
	ImGui::SliderInt("Base Height", &TerrainUtils::baseHeight, -64, 64);
	ImGui::SliderInt("World End", &TerrainUtils::terrainEnd, 1, 10);
	ImGui::SliderFloat("Perlin Scale", &TerrainUtils::scale, 0.001f, 3.0f);
	ImGui::SliderInt("Power", &TerrainUtils::power, 1, 10);
	ImGui::SliderFloat("Terrain Amplitude", &TerrainUtils::terrainAmplitude, 1.0f, 2000.0f);
	
	ImGui::SliderFloat("Cave Scale", &TerrainUtils::caveScale, 0.0010f, 0.7f);
	ImGui::SliderFloat("Cave Density", &TerrainUtils::caveDensity, -1.0f, 1.0f);
	ImGui::SliderFloat("Cave Strenght", &TerrainUtils::caveStrenght, 1.0f, 200.0f);
	ImGui::SliderFloat("IsoSurface", &TerrainUtils::isoSurface, -0.1f, 0.5f);
	
	if (ImGui::Button("Generate Terrain")) {
		setting = true;
	}
	
	ImGui::End();
	return setting;
}
