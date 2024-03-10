#ifndef WORLD_HPP
#define WORLD_HPP

#include "Physics.hpp"
#include "Renderer2d.hpp"

#include <array>

template<std::uint32_t xMax, std::uint32_t yMax>
class World
{
private:
	std::array<glm::vec2, getBallCount(xMax, yMax)> posArr;
	std::array<glm::vec2, getBallCount(xMax, yMax)> velArr;
	Physics<decltype(posArr), decltype(velArr)> physicsEngine;
	Renderer2d<decltype(posArr), decltype(velArr)> renderer;

public:
	World() noexcept : physicsEngine(posArr, velArr, xMax, yMax), renderer(posArr, velArr, xMax, yMax)
	{
	}

	bool initializeWorld()
	{
		physicsEngine.generateStartValues();
		return renderer.initialize();
	}

	void run()
	{
		while (renderer.isWindowActive())
		{
			physicsEngine.doIteration();
			renderer.render();
		}

		renderer.cleanup();
	}
};

#endif