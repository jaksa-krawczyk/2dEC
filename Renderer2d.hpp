#ifndef RENDERER2D_HPP
#define RNDERER2D_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Constants.hpp"
#include "Shaders.hpp"
#include "ImGuiHandler.hpp"

#include <cstdint>
#include <vector>
#include <numbers>
#include <iostream>

inline void glfwError(int id, const char* description)
{
	std::cout << "GLFW error: " << description << "\n";
}

template<typename PosArrType, typename VelArrType>
class Renderer2d
{
private:
	const PosArrType& positions;
	const VelArrType& velocities;
	const std::uint32_t xMax;
	const std::uint32_t yMax;

	GLFWwindow* window;
	GLuint vertexBuffer, vertexArray, indexBuffer;

	Shaders shader;
	std::vector<glm::vec2> circleVerticies;
	std::vector<std::uint32_t> indices;

	ImGuiHandler<VelArrType> imGuiHandler;

	bool prepareBuffersAndShaders()
	{
		const std::uint32_t nrOfVertices = TRIANGLES_PER_CIRCLE * positions.size();
		circleVerticies.reserve(nrOfVertices);
		std::cout << "Numbers of particles: " << positions.size() << "\n";

		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * nrOfVertices, nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		indices.reserve(3 * (TRIANGLES_PER_CIRCLE - 2) * positions.size());
		for (std::uint32_t j = 0; j < positions.size(); ++j)
		{
			std::uint32_t index = j * TRIANGLES_PER_CIRCLE;
			for (std::uint32_t i = 0; i < TRIANGLES_PER_CIRCLE - 2; ++i)
			{
				indices.push_back(index);
				indices.push_back(index + i + 1);
				indices.push_back(index + i + 2);
			}
		}

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_DYNAMIC_DRAW);

		auto programId = shader.getProgramId();
		if (programId)
		{
			glUseProgram(programId.value());
			return true;
		}
		return false;
	}

	void translateAndMakeCircles()
	{
		const float xScale = xMax / 2.f;
		const float yScale = yMax / 2.f;
		const float radius_x = CIRCLE_RADIUS * 2.f / xMax;
		const float radius_y = CIRCLE_RADIUS * 2.f / yMax;
		const float angle = 2.f * std::numbers::pi_v<float> / TRIANGLES_PER_CIRCLE;

		circleVerticies.clear();
		for (const auto& circleCenter : positions)
		{
			float x_center = (circleCenter.x - xScale) / xScale;
			float y_center = (circleCenter.y - yScale) / yScale;

			for (std::uint32_t i = 0; i < TRIANGLES_PER_CIRCLE; ++i)
			{
				float currentAngle = i * angle;
				float x = radius_x * cosf(currentAngle) + x_center;
				float y = radius_y * sinf(currentAngle) + y_center;

				circleVerticies.push_back(glm::vec2(x, y));
			}
		}
	}

public:
	Renderer2d(const PosArrType& positions_, const VelArrType& velocities_, const std::uint32_t xMax_, const std::uint32_t yMax_) noexcept :
		positions(positions_), velocities(velocities_), xMax(xMax_), yMax(yMax_), window(nullptr), imGuiHandler(velocities)
	{
	}

	bool initialize()
	{
		glfwSetErrorCallback(&glfwError);

		if (!glfwInit())
		{
			std::cout << "glfwInit failed\n";
			return false;
		}

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(xMax, yMax, "Elastic 2d collisions", NULL, NULL);
		if (window == nullptr)
		{
			std::cout << "window nullptr\n";
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);
		if (GLEW_OK != glewInit())
		{
			std::cout << "glewInit() failed\n";
			return false;
		}
		
		imGuiHandler.initialize(window);
		return prepareBuffersAndShaders();
	}

	void render()
	{
		glfwMakeContextCurrent(window);
		glfwPollEvents();
		translateAndMakeCircles();
		glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(float) * circleVerticies.size(), circleVerticies.data());

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		imGuiHandler.render();
		glfwSwapBuffers(window);
	}

	bool isWindowActive() noexcept
	{
		return !glfwWindowShouldClose(window);
	}

	void cleanup() noexcept
	{
		imGuiHandler.cleanup();
		glDeleteBuffers(1, &indexBuffer);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteVertexArrays(1, &vertexArray);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

#endif