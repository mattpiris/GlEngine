#pragma once

#include <vector>
#include <glm.hpp>
#include <memory>

#include "Particle.h"
#include "IEmissionShape.h"
#include "../../utils/Shader.h"

/// to abstract things away, i need to redefine some values
// also, it would be cool if the emitter followed some primitive shapes. 
// I was thinking of implementing a sphere, a semisphere and a 3d rectangle 

/// TODO:
/// particle emitter shapes     support for spawn bursts
// ->sphere                     add support for the textures and the colors
// ->semisphere
// ->cone
// ->quad
class ParticleEmitter
{
public:
    // the num particles parameters indicates the max number of particles that can be alive at the
    // same time on the screen
    // the velocity is the scalar velocity assigned to the velocity vector
    // for now the velocity direction is assigned randomly, might change this layer
    ParticleEmitter(unsigned int numParticles, glm::vec3 position, float velocity, 
        float duration, float particlesPerSecond, float particleSize, glm::vec3 color, bool blending);
    
    void setEmissionShape(std::unique_ptr<IEmissionShape> emissionShape);
    void update(float deltaTime);
    void render(Shader& shader, glm::mat4& projection, glm::mat4& view);
private:
    const std::vector<float> quadVertices = {
        // positions     // colors
        -0.05f,  0.05f,
         0.05f, -0.05f,
        -0.05f, -0.05f,

        -0.05f,  0.05f,
         0.05f, -0.05f,
         0.05f,  0.05f,
    };

    std::vector<Particle> m_particlePool;
    std::vector<float> m_instanceBuffer;
    std::vector<glm::vec4> m_colorBuffer;
    std::vector<size_t> m_particleCemetery;

    std::unique_ptr<IEmissionShape> m_emissionShape;
    glm::vec3 m_color;
    bool m_blending;
    float m_velocity;
    float m_duration;
    float m_particleSize;
    float m_emissionRate;
    float m_time;

    unsigned int m_vao;
    unsigned int m_meshvbo;
    unsigned int m_instancedvbo;
    unsigned int m_colorvbo;

    unsigned int m_maxParticles;
    unsigned int m_activeParticles;

    glm::vec3 m_position;
    // this method sets up all the particle buffers
    void init();
    void spawnParticle(Particle* particle);
    Particle* getLastDeadParticle();
};