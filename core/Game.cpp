#include "Game.h"
#include "../utils/stuff/Log.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../terrain/WorldManager.h"

// static variables for callbacks
unsigned int Game::m_width = 0;
unsigned int Game::m_height = 0;

bool    Game::m_firstMouse = false;
bool    Game::m_freeCursor = false;

double   Game::m_lastx = 0.0f;
double   Game::m_lasty = 0.0f;
float   Game::m_elapsedTime = 0.0f;
float   Game::m_currentTime = 0.0f;
float   Game::m_lastFrame = 0.0f;

Camera* Game::m_camera = nullptr;
GLFWwindow* Game::m_window = nullptr;


Game::Game() {}
Game::~Game() {
    if (m_camera) {
        delete(m_camera);
    }
}

GLFWwindow* Game::init_and_createWindow(unsigned int width, unsigned int height, bool fullscreen)
{
    if (glfwInit() == GLFW_FALSE) {
        LOG("ERROR::GAME: failed to initialize glfw");
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
        LOG("ERROR::GAME: failed to create window");
        return m_window;
    }

    glfwMakeContextCurrent(m_window);

    if (glewInit() != GLEW_OK) {
        LOG("ERROR::GAME: failed to initialize glew");
        return nullptr;
    }
    glfwSetFramebufferSizeCallback(m_window, callback_window_resize);
    glfwSetCursorPosCallback(m_window, callback_mouse_input);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // imgui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    return m_window;
}

GLFWwindow* Game::getWindow() {
    return m_window;
}

void Game::processInputs() {
    updateDeltaTime();

    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);

    // camera input stuff
    if (m_camera) {
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
            m_camera->ProcessKeyboard(FORWARD, deltaTime());
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
            m_camera->ProcessKeyboard(BACKWARD, deltaTime());
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
            m_camera->ProcessKeyboard(LEFT, deltaTime());
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
            m_camera->ProcessKeyboard(RIGHT, deltaTime());
    }

    // i had to do this to tweak imgui parameters
    static bool fKeyWasPressed = false;
    bool fKeyIsPressed = glfwGetKey(m_window, GLFW_KEY_F) == GLFW_PRESS;

    if (fKeyIsPressed && !fKeyWasPressed) {
            m_freeCursor = !m_freeCursor;
            if (m_freeCursor) {
                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else {
                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
    }

    fKeyWasPressed = fKeyIsPressed;
}

Camera* Game::createCamera(glm::vec3 cameraPos) {
    m_camera = new Camera(cameraPos);
    return m_camera;
}

void Game::showFPS() {
    float fps = 1 / deltaTime();
    ImGui::SetNextWindowPos(ImVec2(10, 0), ImGuiCond_Always);
    ImGui::Begin("Stats");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame Time: %.2f ms", deltaTime() * 1000.0f);
    ImGui::End();
}

float Game::deltaTime() {
    return m_elapsedTime;
}

void Game::updateDeltaTime() {
    m_currentTime = static_cast<float>(glfwGetTime());
    m_elapsedTime = m_currentTime - m_lastFrame;
    m_lastFrame = m_currentTime;
}

void Game::callback_window_resize(GLFWwindow* win, int width, int height) {
    glViewport(0, 0, width, height);
}

void Game::callback_mouse_input(GLFWwindow* win, double xpos, double ypos) {
    if (!m_camera) return;
    if (m_freeCursor) return;

    double xposOut = xpos;
    double yposOut = ypos;

    if (m_firstMouse)
    {
        m_lastx = xpos;
        m_lasty = ypos;
        m_firstMouse = false;
    }

    double xoffset = xpos - m_lastx;
    double yoffset = m_lasty - ypos; // reversed since y-coordinates go from bottom to top

    m_lastx = xpos;
    m_lasty = ypos;

    m_camera->ProcessMouseMovement(xoffset, yoffset);
}