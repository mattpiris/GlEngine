#pragma once
#include <unordered_map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <functional>

class Scene
{
public:
	Scene() = default;
	virtual void update() = 0;
	virtual void updateCamera(double x, double y) = 0;
	virtual void render() = 0;
	
	// the status of the key is in the higher 10 bits,
	// the key value in the lower
	// this number is meant to be used in the action map to perform the action desired
	// status can be: GLFW_RELEASE, GLFW_PRESS, GLFW_REPEAT
	static int encodeAction(int key, int status) {
		return (status << 10) | (key & 0x3FF);
	}

	virtual std::unordered_map<int, std::function<void()>>& getActionMap() final { return m_actionMap; }
protected:
	std::unordered_map<int, std::function<void()>> m_actionMap;

	virtual void registerAction(int key, int action, std::function<void()> func) {
		int id = encodeAction(key, action);
		m_actionMap[id] = func;
	}
};