#pragma once
#include "../core/Scene.h"
#include "../utils/Camera.h"
#include "../terrain/ResourcesManager.h"
#include "../terrain/WorldManager.h"

#include <memory>
class TestScene : public Scene
{
public:
	TestScene();
	virtual void render() override;
	virtual void update() override;
	virtual void updateCamera(double x, double y) override;
private:
	std::shared_ptr<Camera> m_camera;
	bool m_firstMouse;
	double m_lastx;
	double m_lasty;

	WorldManager m_map;
	
	// debug stuff
	Shader m_terrainShader;
	Shader m_cubeShader;
	Cube m_debugCube;
	Physics::RayCast m_ray;
};