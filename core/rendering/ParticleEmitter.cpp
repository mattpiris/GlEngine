#include "ParticleEmitter.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <random>
#include <iostream>

ParticleEmitter::ParticleEmitter(unsigned int numParticles, glm::vec3 position, float velocity,
	float duration, float particlesPerSecond, float particleSize, glm::vec3 color, bool blending)
	:m_maxParticles(numParticles), m_position(position), m_velocity(velocity),
	m_duration(duration), m_particleSize(particleSize), m_emissionRate(1.0f / particlesPerSecond),
	m_time(0.0f), m_emissionShape(nullptr), m_color(color), m_blending(blending) {
	init();
}

void ParticleEmitter::init()
{
	srand(time(0));
	m_particlePool.resize(m_maxParticles);
	m_colorBuffer.resize(m_maxParticles);
	m_instanceBuffer.resize(m_maxParticles * 4);
	// construction of the particle pool	
	for (int i = 0; i < m_maxParticles; i++) {
		float rndmx = (rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
		float rndmy = (rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
		float rndmz = (rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
		glm::vec3 velocity = glm::normalize(glm::vec3(rndmx, rndmy, rndmz)) * m_velocity;
		m_particlePool[i] = (Particle(m_position, velocity, glm::vec4(1.0f), m_particleSize, 
			m_duration, false));
		m_particleCemetery.push_back(i);
		
		m_colorBuffer[i] = glm::vec4(m_color, 1.0f);
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// setting up geometry buffer
	glGenBuffers(1, &m_meshvbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_meshvbo);
	glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);
	// in the shader, the location = 0 is for the local position of the quad
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
	glEnableVertexAttribArray(0);

	// the location = 1 is for the color, might change this later
	glGenBuffers(1, &m_colorvbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorvbo);
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 2));
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(float) * 4, NULL, GL_STREAM_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	// instanced vbo creation
	glGenBuffers(1, &m_instancedvbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_instancedvbo);
	// we pass null here because we update the buffer data in the update function
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(float) * 4, NULL, GL_STREAM_DRAW);
	// data for the per-instance position
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	// data for the per-instance size
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Particle* ParticleEmitter::getLastDeadParticle() {
	if (m_particleCemetery.empty()) return nullptr;
	size_t lastIndex = m_particleCemetery.back();
	m_particleCemetery.pop_back();
	return &m_particlePool[lastIndex];
}

void ParticleEmitter::spawnParticle(Particle* particle) {
	if (!m_emissionShape)
	{
		particle->position = m_position;
		float rndmx = (rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
		float rndmy = (rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
		float rndmz = (rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
		glm::vec3 newVelocity = glm::normalize(glm::vec3(rndmx, rndmy, rndmz)) * m_velocity;
		particle->velocity = newVelocity;
	}
	else {
		particle->position = m_emissionShape->generatePosition();
		glm::vec3 velocity = m_emissionShape->generateDirection() * m_velocity;
		particle->velocity = velocity;
	}

	particle->life = m_duration;
	particle->isAlive = true;
}

void ParticleEmitter::setEmissionShape(std::unique_ptr<IEmissionShape> emissionShape) {
	m_emissionShape = std::move(emissionShape);
}

void ParticleEmitter::update(float deltaTime) {
	// update all the particle lifecycle in here, only add the information about the active particles
	// in the instance buffer
	// find an algorithm that spawns particles when necessary
	m_time += deltaTime;
	int numToSpawn = static_cast<int>(m_time / m_emissionRate);
	m_time -= numToSpawn * m_emissionRate;
	for (int i = 0; i < numToSpawn; i++) {
		Particle* particle = getLastDeadParticle();
		if (particle) {
			spawnParticle(particle);
		}
	}

	m_activeParticles = 0;
	m_instanceBuffer.clear();
	m_colorBuffer.clear();

	for (auto& particle : m_particlePool) {
		if (!particle.isAlive) continue;

		m_activeParticles++;
		particle.position += particle.velocity * deltaTime;
		m_instanceBuffer.push_back(particle.position.x);
		m_instanceBuffer.push_back(particle.position.y);
		m_instanceBuffer.push_back(particle.position.z);
		m_instanceBuffer.push_back(particle.size);

		particle.life -= deltaTime;
		// mark it dead for the next loop
		if (particle.life < 0) {
			particle.isAlive = false;
			m_particleCemetery.push_back(m_activeParticles);
		}
		float alpha = 1.0f;
		if (m_blending) { alpha = particle.life / m_duration; }
		
		glm::vec4 color(m_color, alpha);
		m_colorBuffer.push_back(color);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_instancedvbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_instanceBuffer.size() * sizeof(float), m_instanceBuffer.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (m_blending) {
		glBindBuffer(GL_ARRAY_BUFFER, m_colorvbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_colorBuffer.size() * sizeof(float) * 4, m_colorBuffer.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void ParticleEmitter::render(Shader& shader, glm::mat4& projection, glm::mat4& view) {
	if (m_blending) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

	glBindVertexArray(m_vao);  // <- move out of the loop!
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_activeParticles);
}