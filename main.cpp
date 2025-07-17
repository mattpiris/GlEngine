#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

#include "core/Game.h"
#include "utils/gameObj/Cube.h"
#include "core/rendering/ParticleEmitter.h"
#include "core/rendering/RectangleEmissionShape.h"

constexpr auto WIN_WIDTH = 1920;
constexpr auto WIN_HEIGHT = 1080;

glm::vec3 cubePos(-10.0f, 0.0f, 1.0f);
glm::vec3 ambientColor(1.0f, 0.9f, 0.81f);
glm::vec3 emitterPos(0.0f, 0.0f, 0.0f);
glm::vec3 particleColor(1.0f, 0.0f, 1.0f);

int main()
{
    Game game;
    GLFWwindow* win = game.init_and_createWindow(WIN_WIDTH, WIN_HEIGHT);
    Camera* cam = game.createCamera(glm::vec3(0.0f, 0.0f, 0.0f));
    Shader shader("utils/shaders/VertexShader.vert", "utils/shaders/FragmentShader.frag");
    Shader particleShader("utils/shaders/ParticleVert.vert", "utils/shaders/ParticleFrag.frag");
    ParticleEmitter emitter(1000, emitterPos, .1f, 15.0f, 10.0f, 1.0f, particleColor, true);
    std::unique_ptr<RectangleEmissionShape> emissionShape = 
        std::make_unique<RectangleEmissionShape>(50.0f, 50.0f, 50.0f, emitterPos);
    
    emitter.setEmissionShape(std::move(emissionShape));

    Cube cube(cubePos, &shader);
    Cube emits(emitterPos, &shader);

    while(!glfwWindowShouldClose(win)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        game.processInputs();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam->GetViewMatrix();

        shader.use();
        shader.setVec3("ambientColor", ambientColor);

        cube.draw(projection, view);
        emits.draw(projection, view);

        emitter.update(game.deltaTime());
        emitter.render(particleShader, projection, view);

        glfwPollEvents();
        glfwSwapBuffers(win);
    }
    return 0;
}