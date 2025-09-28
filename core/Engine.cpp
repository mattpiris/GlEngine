#include "Engine.h"
#include "../utils/stuff/Log.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../terrain/WorldManager.h"
#include "Scene.h"

// static variables for callbacks
unsigned int Engine::m_width = 0;
unsigned int Engine::m_height = 0;

/*
bool    Engine::m_firstMouse = false;
bool    Engine::m_freeCursor = false;

double   Engine::m_lastx = 0.0f;
double   Engine::m_lasty = 0.0f;
*/
float   Engine::m_elapsedTime = 0.0f;
float   Engine::m_currentTime = 0.0f;
float   Engine::m_lastFrame = 0.0f;

std::shared_ptr<Scene> Engine::m_currentScene;
std::unordered_map<unsigned int, std::shared_ptr<Scene>> Engine::m_sceneMap;

bool m_keyMap[1024] = { false };
GLFWwindow* Engine::m_window = nullptr;


Engine::Engine() {}
Engine::~Engine() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

GLFWwindow* Engine::init_and_createWindow(unsigned int width, unsigned int height, bool fullscreen)
{
    if (glfwInit() == GLFW_FALSE) {
        LOG("ERROR::Engine: failed to initialize glfw");
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    if (fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
        m_window = glfwCreateWindow(vmode->width, vmode->height, "hello GL", monitor, NULL);
        glViewport(0, 0, vmode->width, vmode->height);
    }
    else {
        m_window = glfwCreateWindow(width, height, "hello GL", nullptr, nullptr);
        glViewport(0, 0, width, height);
    }

    m_width = width;
    m_height = height;

    if (m_window == nullptr) {
        LOG("ERROR::Engine: failed to create window");
        return m_window;
    }

    glfwMakeContextCurrent(m_window);

    if (glewInit() != GLEW_OK) {
        LOG("ERROR::Engine: failed to initialize glew");
        return nullptr;
    }
    glfwSetFramebufferSizeCallback(m_window, callback_window_resize);
    glfwSetCursorPosCallback(m_window, callback_mouse_input);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(m_window, callback_key_input);
    glfwSetMouseButtonCallback(m_window, callback_mouse_key_input);

    // imgui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    return m_window;
}

void Engine::addScene(unsigned int layer, std::shared_ptr<Scene> newScene) {
    if (m_sceneMap.find(layer) != m_sceneMap.end()) {
        LOG("ENGINE::ADD_SCENE::ERROR:: collision in map layer");
        return;
    }

    m_sceneMap[layer] = newScene;
}

void Engine::changeScene(unsigned int layer) {
    if (m_sceneMap.find(layer) == m_sceneMap.end()) {
        LOG("ENGINE::CHANGE_SCENE::ERROR:: scene doesnt exist at specified layer");
        m_currentScene = nullptr;
        return;
    }

    m_currentScene = m_sceneMap[layer];
}

GLFWwindow* Engine::getWindow() {
    return m_window;
}

 void Engine::run() {
     while (isRunning()) {
         updateDeltaTime();
         processInputs();
         if(m_currentScene)
            m_currentScene->update();
         
         render();
     }
 }

 void Engine::enableDepthTest(bool depth) {
     if (depth) {
         glEnable(GL_DEPTH_TEST);
     }
 }
 
 inline bool Engine::isRunning() {
     if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
         glfwSetWindowShouldClose(m_window, true);
         return false;
     }

     return !glfwWindowShouldClose(m_window);
 }

 void Engine::render()
 {
     glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     // processInputs();
     if (m_currentScene) m_currentScene->render(); 

#pragma region imgui
     ImGui_ImplOpenGL3_NewFrame();
     ImGui_ImplGlfw_NewFrame();
     ImGui::NewFrame();
     
     showFPS();

     ImGui::Render();
     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion


     glfwPollEvents();
     glfwSwapBuffers(m_window);
 }

std::shared_ptr<Camera> Engine::createCamera(glm::vec3 cameraPos) {
    // m_camera = new Camera(cameraPos);
    return std::make_shared<Camera>(cameraPos);
}

void Engine::showFPS() {
    float fps = 1 / deltaTime();
    ImGui::SetNextWindowPos(ImVec2(10, 0), ImGuiCond_Always);
    ImGui::Begin("Stats");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame Time: %.2f ms", deltaTime() * 1000.0f);
    ImGui::End();
}

float Engine::deltaTime() {
    return m_elapsedTime;
}

void Engine::updateDeltaTime() {
    m_currentTime = static_cast<float>(glfwGetTime());
    m_elapsedTime = m_currentTime - m_lastFrame;
    m_lastFrame = m_currentTime;
}

void Engine::callback_window_resize(GLFWwindow* win, int width, int height) {
    glViewport(0, 0, width, height);
}


void Engine::callback_mouse_input(GLFWwindow* win, double xpos, double ypos) {
    if(m_currentScene) m_currentScene->updateCamera(xpos, ypos);
}

void Engine::callback_key_input(GLFWwindow* win, int key, int scancode, int action, int mods) {
    if (!m_currentScene) return;
    // updates the key map
    int _key = Scene::encodeAction(key, action);
    if (action == GLFW_PRESS) {
        m_keyMap[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        m_keyMap[key] = false;
    }

    auto it = m_currentScene->getActionMap().find(_key);
    if (it != m_currentScene->getActionMap().end()) {
        it->second();
    }
}

void Engine::callback_mouse_key_input(GLFWwindow* win, int key, int action, int mods) {
    if (!m_currentScene) return;
    // updates the key map
    int _key = Scene::encodeAction(key, action);
    if (action == GLFW_PRESS) {
        m_keyMap[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        m_keyMap[key] = false;
    }

    auto it = m_currentScene->getActionMap().find(_key);
    if (it != m_currentScene->getActionMap().end()) {
        it->second();
    }
}

// had to do this to avoid the glfw_repeat delay bruh
void Engine::processInputs() {
    for (int i = 0; i < 1024; i++) {
        if (!m_keyMap[i]) continue;
        int key = Scene::encodeAction(i, GLFW_REPEAT);
        if (m_currentScene->getActionMap()[key]) {
            m_currentScene->getActionMap()[key]();
        };
    }
}