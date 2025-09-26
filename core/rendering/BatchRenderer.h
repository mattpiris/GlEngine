#pragma once
#include "Model.h"
#include <memory>
class BatchRenderer
{
public:
	BatchRenderer() = default;
	BatchRenderer(std::shared_ptr<Model> model, std::vector<glm::mat4>& positions);
	void render(Shader& shader, glm::mat4& projection, glm::mat4& view);
private:
	std::vector<glm::mat4> m_positions;
	std::shared_ptr<Model> m_model;
};