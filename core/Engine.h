#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../utils/Camera.h"
#include "Scene.h"

#include <unordered_map>
#include <memory>

// application sorta thing
class Engine
{
public:
	Engine();
	~Engine();
	static GLFWwindow* init_and_createWindow(unsigned int width, unsigned int height, bool fullscreen = false);

	static void addScene(unsigned int layer, std::shared_ptr<Scene> newScene);
	static void changeScene(unsigned int layer);

	// creates and retrieves the newly created camera
	// camera pointer lifetime is managed internally
	static std::shared_ptr<Camera> createCamera(glm::vec3 cameraPos);
	static void showFPS();
	static void run();

	static void enableDepthTest(bool depth);

	// retrieves deltaTime
	static float deltaTime();


	GLFWwindow* getWindow();
	static float getWindowHeight() { return m_height; }
	static float getWindowWidth() { return m_width; }
private:
	static GLFWwindow* m_window;
	static std::shared_ptr<Scene> m_currentScene;
	static std::unordered_map<unsigned int, std::shared_ptr<Scene>> m_sceneMap;

	static unsigned int m_width;
	static unsigned int m_height;

	static float m_currentTime;
	static float m_lastFrame;
	static float m_elapsedTime;
	static bool m_isRunning;

	static void updateDeltaTime();
	static inline bool isRunning();
	static void render();

	// resizewindow callback
	static void callback_window_resize(GLFWwindow* win, int width, int height);
	static void callback_mouse_input(GLFWwindow* win, double xpos, double ypos);
	static void callback_key_input(GLFWwindow* win, int key, int scancode, int action, int mods);
	static void callback_mouse_key_input(GLFWwindow* win, int key, int action, int mods);

	static void processInputs();
};