#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../utils/Camera.h"

class Game
{
public:
	Game();
	~Game();
	static GLFWwindow* init_and_createWindow(unsigned int width, unsigned int height, bool fullscreen = false);
	GLFWwindow* getWindow();

	// processes all the user inputs. 
	// updates the delta time of the game, and if the user presses the esc key the 
	// window terminates. 
	// it automatically handles the wasd keys for the camera
	static void processInputs();
	// creates and retrieves the newly created camera
	// camera pointer lifetime is managed internally
	static Camera* createCamera(glm::vec3 cameraPos);
	// retrieves deltaTime
	static float deltaTime();
private:
	static GLFWwindow* m_window;
	static Camera* m_camera;

	static unsigned int m_width;
	static unsigned int m_height;

	static float m_currentTime;
	static float m_lastFrame;
	static float m_elapsedTime;

	static bool m_firstMouse;
	static float m_lastx;
	static float m_lasty;

	static void updateDeltaTime();

	// resizewindow callback
	static void callback_window_resize(GLFWwindow* win, int width, int height);
	static void callback_mouse_input(GLFWwindow* win, double xpos, double ypos);
};