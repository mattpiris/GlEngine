#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <memory>

#include "core/Game.h"
#include "utils/gameObj/Cube.h"
#include "utils/gameObj/Plane.h"

#include "core/rendering/ParticleEmitter.h"
#include "core/rendering/RectangleEmissionShape.h"
#include "core/rendering/Model.h"

#include "terrain/ResourcesManager.h"
#include "terrain/WorldManager.h"

constexpr auto WIN_WIDTH = 1920;
constexpr auto WIN_HEIGHT = 1080;
constexpr auto NEAR_PLANE = 0.1f;
constexpr auto FAR_PLANE = 300.0f;

glm::vec3 cubePos(-10.0f, 0.0f, 1.0f);
glm::vec3 planePos(-1.0f, -0.5f, 0.0f);
float planeSize = 10.0f;

glm::vec3 ambientColor(1.0f, 0.9f, 0.81f);
glm::vec3 emitterPos(0.0f, 0.0f, 0.0f);

glm::vec3 particleColor(1.0f, 0.0f, 1.0f);
glm::vec4 emitterColor(1.0f, 0.0f, 1.0f, 1.0f);
glm::vec4 planeColor(0.45, 0.33, 0.20, 1.0);

// the chunk dimensions should be multiple of 2 for internal reasons
unsigned int chunkSize = 16; // dont put this higher than 32, vs crashes lolslsls
unsigned int n_chunks = 20;
unsigned int n_worldHeight = 64; // world height

unsigned int fieldSize_x = 40;
unsigned int fieldSize_z = 40;

int main()
{
    Game game;
    GLFWwindow* win = game.init_and_createWindow(WIN_WIDTH, WIN_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    Camera* cam = game.createCamera(glm::vec3(0.0f, 0.0f, 0.0f));

    glm::vec3 grassfieldPos(planePos.x - (fieldSize_x / 2), planePos.y, planePos.z - (fieldSize_z / 2));
    Shader shader("utils/shaders/VertexShader.vert", "utils/shaders/FragmentShader.frag");
    Shader particleShader("utils/shaders/ParticleVert.vert", "utils/shaders/ParticleFrag.frag");
    ParticleEmitter emitter(1000, emitterPos, .1f, 15.0f, 10.0f, 1.0f, particleColor, true);
    std::unique_ptr<RectangleEmissionShape> emissionShape = 
        std::make_unique<RectangleEmissionShape>(50.0f, 50.0f, 50.0f, grassfieldPos);
    
    emitter.setEmissionShape(std::move(emissionShape));

    Cube cube(cubePos, &shader);
    Cube emits(emitterPos, &shader, 1.0f, emitterColor);

    Plane plane(planePos, &shader, planeSize, planeColor);
    /*
    ResourcesManager::getInstance().loadModel(AssetID::BUSH_COMMON, "assets/models/Bush_Common.obj");
    ResourcesManager::getInstance().loadModel(AssetID::GRASS_WISPY_TALL, "assets/models/Grass_Wispy_Tall.obj");
    ResourcesManager::getInstance().loadModel(AssetID::TWISTED_TREE1, "assets/models/TwistedTree_1.obj");
    ResourcesManager::getInstance().loadModel(AssetID::TWISTED_TREE2, "assets/models/TwistedTree_2.obj");
    ResourcesManager::getInstance().loadModel(AssetID::TWISTED_TREE3, "assets/models/TwistedTree_3.obj");
    ResourcesManager::getInstance().loadModel(AssetID::TWISTED_TREE4, "assets/models/TwistedTree_4.obj");
    ResourcesManager::getInstance().loadModel(AssetID::TWISTED_TREE5, "assets/models/TwistedTree_5.obj");
    */
    Model grassModel("assets/models/Grass_Wispy_Tall.obj");
    Shader instancedShader("utils/shaders/InstancedModelVert.vert", "utils/shaders/BushModelFrag.frag");

    WorldManager world(n_chunks, n_worldHeight, chunkSize, glm::vec3(0.0f), FAR_PLANE);
    Shader terrainShader("utils/shaders/TerrainVert.vert", "utils/shaders/TerrainFrag.frag");

    stbi_set_flip_vertically_on_load(true);
    // Model assetModel("assets/models/Bush_Common.obj");
    Model assetModel("assets/models/TwistedTree_3.obj");
    // Model assetModel("assets/models/Grass_Wispy_Tall.obj");
    // Model model("assets/models/CommonTree_1.obj");
    Shader modelShader("utils/shaders/BushModelVert.vert", "utils/shaders/BushModelFrag.frag");

    while(!glfwWindowShouldClose(win)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        game.processInputs();
      
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / (float)WIN_HEIGHT, NEAR_PLANE, FAR_PLANE);
        glm::mat4 view = cam->GetViewMatrix();

        shader.use();

        cube.draw(projection, view);
        // plane.draw(projection, view);

        emits.draw(projection, view);

        emitter.update(game.deltaTime());
        emitter.render(particleShader, projection, view);

        world.renderTerrain(terrainShader, projection, view);
        // grassfield.render(instancedShader, projection, view);
        world.renderVegetation(instancedShader, projection, view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        assetModel.render(modelShader, projection, view, model);

#pragma region IMGUI_stuff
        // imgui stuff
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        game.showFPS();
        world.TerrainImGui();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion

        glfwPollEvents();
        glfwSwapBuffers(win);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}