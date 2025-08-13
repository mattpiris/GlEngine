#pragma once
#include <glm/glm.hpp>

class Particle
{
public:
	Particle() = default;
	Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 color, float size, float life, bool isAlive)
		: position(position), velocity(velocity),
		color(color), size(size), life(life), isAlive(isAlive)
	{}
private:
	friend class ParticleEmitter;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;
	float life;
	float size;
	bool isAlive;
};