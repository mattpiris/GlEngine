#include "TestScene.h"
#include "../core/Engine.h"
#include "../physics/RayCast.h"

#include "../utils/gameObj/Cube.h"


// the chunk dimensions should be multiple of 2 for internal reasons
unsigned int chunkSize = 16; // dont put this higher than 32, vs crashes lolslsls
unsigned int n_chunks = 10;
unsigned int n_worldHeight = 64; // world height

TestScene::TestScene()
	:m_camera(nullptr), m_firstMouse(false), m_lastx(0.0f), m_lasty(0.0f),
    m_map(n_chunks, n_worldHeight, chunkSize, glm::vec3(0.0f), 300.0f),
    m_terrainShader("utils/shaders/TerrainVert.vert", "utils/shaders/TerrainFrag.frag"),
    m_cubeShader("utils/shaders/VertexShader.vert", "utils/shaders/FragmentShader.frag"),
    m_ray(glm::vec3(-1.0f), glm::vec3(0.0f), 50.0f),
    m_debugCube(glm::vec3(0.0f), m_cubeShader)
{
	m_camera = Engine::createCamera(glm::vec3(0.0f, 0.0f, 0.0f));

    // creation of code for camera movement
    m_actionMap[Scene::encodeAction(GLFW_KEY_W, GLFW_REPEAT)] = [this]() { 
        m_camera->ProcessKeyboard(FORWARD, Engine::deltaTime()); };
    m_actionMap[Scene::encodeAction(GLFW_KEY_S, GLFW_REPEAT)] = [this]() {
        m_camera->ProcessKeyboard(BACKWARD, Engine::deltaTime()); };
    m_actionMap[Scene::encodeAction(GLFW_KEY_A, GLFW_REPEAT)] = [this]() {
        m_camera->ProcessKeyboard(LEFT, Engine::deltaTime()); };
    m_actionMap[Scene::encodeAction(GLFW_KEY_D, GLFW_REPEAT)] = [this]() {
        m_camera->ProcessKeyboard(RIGHT, Engine::deltaTime()); };
    // shoot raycast
    m_actionMap[Scene::encodeAction(GLFW_MOUSE_BUTTON_LEFT, GLFW_REPEAT)] = [this]() {
        glm::vec3 position = m_camera->Position;
        glm::vec3 direction = m_camera->Front;
        direction = glm::normalize(direction);
        m_ray.update(position, direction);
        m_ray.active = true;
    };

    m_actionMap[Scene::encodeAction(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE)] = [this]() {
        m_ray.active = false;
        };
}

void TestScene::update() {}


void TestScene::render() {
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), Engine::getWindowWidth() / Engine::getWindowHeight(), 0.01f, 300.0f);
    glm::mat4 view = m_camera->GetViewMatrix();
    m_map.renderTerrain(m_terrainShader, projection, view);
    
    if (m_ray.active) {
        Shader rayShader("utils/shaders/rayCastVert.vert", "utils/shaders/rayCastFrag.frag");
        m_ray.draw(rayShader, projection, view);

        auto hit = m_map.getNearestTerrainIntersection(m_ray);
        if (hit) {
            glm::vec3 cubePos = hit->hitPoint;
            m_debugCube.setPosition(cubePos);
            m_debugCube.draw(projection, view);
        }
    }
}

void TestScene::updateCamera(double x, double y) {
    if (!m_camera) return;
    // if (m_freeCursor) return;

    double xposOut = x;
    double yposOut = y;

    if (m_firstMouse)
    {
        m_lastx = x;
        m_lasty = y;
        m_firstMouse = false;
    }

    double xoffset = x - m_lastx;
    double yoffset = m_lasty - y; // reversed since y-coordinates go from bottom to top

    m_lastx = x;
    m_lasty = y;

    m_camera->ProcessMouseMovement(xoffset, yoffset);
}
