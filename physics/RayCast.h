#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "../utils/Shader.h"
namespace Physics 
{
	struct RayCastHit
	{
		glm::vec3 hitPoint;
		float t;
	};


	class RayCast
	{
	public:
		glm::vec3 origin;
		glm::vec3 direction;
		float maxDist;
		bool active;

		RayCast(glm::vec3 origin, glm::vec3 direction, float maxDist)
			:origin(origin), direction(direction), maxDist(maxDist), active(true) {
			// sets up opengl related stuff to draw the raycast eventually
			
			m_vertices.push_back(origin);
			m_vertices.push_back(origin + direction * maxDist);

			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			glGenBuffers(1, &m_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), m_vertices.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		void update(glm::vec3 newOrigin, glm::vec3 newDirection) {
			origin = newOrigin;
			direction = newDirection;
			// updating gpu side aswell
			m_vertices[0] = origin;
			m_vertices[1] = origin + direction * maxDist;

			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(glm::vec3), m_vertices.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void draw(Shader& shader, glm::mat4& projection, glm::mat4& view) {
			shader.use();
			shader.setMat4("projection", projection);
			shader.setMat4("view", view);
			glBindVertexArray(m_vao);
			glDrawArrays(GL_LINES, 0, 2);
			glBindVertexArray(0);
		}

	private:
		unsigned int m_vao;
		unsigned int m_vbo;
		std::vector<glm::vec3> m_vertices;
	};
}

