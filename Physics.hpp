#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "Constants.hpp"
#include "Grid.hpp"

#include <glm/vec2.hpp>
#include <glm/geometric.hpp>

#include <cstdint>
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>

template<typename PosArrType, typename VelArrType>
class Physics
{
private:
	PosArrType& positions;
	VelArrType& velocities;

	const std::uint32_t xMax;
	const std::uint32_t yMax;

	Grid2d grid;

	void checkXCollisions(const std::uint32_t i) noexcept
	{
		if (positions[i].x <= CIRCLE_RADIUS)
		{
			positions[i] -= deltaT * velocities[i];

			float deltaTBoundary = (CIRCLE_RADIUS - positions[i].x) / velocities[i].x;
			float deltaTReflection = deltaT - deltaTBoundary;

			velocities[i].x = -velocities[i].x;
			positions[i].x = CIRCLE_RADIUS;
			positions[i] += deltaTReflection * velocities[i];
		}
		else if (positions[i].x >= xMax - CIRCLE_RADIUS)
		{
			positions[i] -= deltaT * velocities[i];

			float deltaTBoundary = (xMax - CIRCLE_RADIUS - positions[i].x) / velocities[i].x;
			float deltaTReflection = deltaT - deltaTBoundary;

			velocities[i].x = -velocities[i].x;
			positions[i].x = static_cast<float>(xMax - CIRCLE_RADIUS);
			positions[i] += deltaTReflection * velocities[i];
		}
	}

	void checkYCollisions(const std::uint32_t i) noexcept
	{
		if (positions[i].y <= CIRCLE_RADIUS)
		{
			positions[i] -= deltaT * velocities[i];

			float deltaTBoundary = (CIRCLE_RADIUS - positions[i].y) / velocities[i].y;
			float deltaTReflection = deltaT - deltaTBoundary;

			velocities[i].y = -velocities[i].y;
			positions[i].y = CIRCLE_RADIUS;
			positions[i] += deltaTReflection * velocities[i];
		}
		else if (positions[i].y >= yMax - CIRCLE_RADIUS)
		{
			positions[i] -= deltaT * velocities[i];

			float deltaTBoundary = (yMax - CIRCLE_RADIUS - positions[i].y) / velocities[i].y;
			float deltaTReflection = deltaT - deltaTBoundary;

			velocities[i].y = -velocities[i].y;
			positions[i].y = static_cast<float>(yMax - CIRCLE_RADIUS);
			positions[i] += deltaTReflection * velocities[i];
		}
	}

	void resolveBoundaryCollisions() noexcept
	{
		for (std::uint32_t i = 0; i < positions.size(); ++i)
		{
			checkXCollisions(i);
			checkYCollisions(i);
		}
	}

	void updateAfterCollision(const std::uint32_t i, const std::uint32_t j) noexcept
	{
		positions[i] -= deltaT * velocities[i];
		positions[j] -= deltaT * velocities[j];

		auto velocity_i = velocities[i];
		auto diffPos_ij = positions[i] - positions[j];
		auto lengthDiffPos_ij = glm::length(positions[i] - positions[j]);

		velocities[i] -= glm::dot((velocities[i] - velocities[j]), diffPos_ij) / (lengthDiffPos_ij * lengthDiffPos_ij) * diffPos_ij;
		velocities[j] += glm::dot((velocities[j] - velocity_i), -diffPos_ij) / (lengthDiffPos_ij * lengthDiffPos_ij) * diffPos_ij;

		positions[i] += deltaT * velocities[i];
		positions[j] += deltaT * velocities[j];
	}

	void resolveCellConflicts(auto& cellParticles)
	{
		for (std::uint32_t i = 0; i < cellParticles.size(); ++i)
		{
			for (std::uint32_t j = 0; j < cellParticles.size(); ++j)
			{
				if (i != j && glm::distance(positions[cellParticles[i].particleId], positions[cellParticles[j].particleId]) < 2.f * CIRCLE_RADIUS)
				{
					updateAfterCollision(cellParticles[i].particleId, cellParticles[j].particleId);
				}
			}
		}
	}

	void resolveConflicts() noexcept
	{
		auto& gridCells = grid.getGridCells();

		for (auto& cell : gridCells)
		{
			resolveCellConflicts(cell);
		}

		resolveBoundaryCollisions();
	}

public:
	Physics(PosArrType& positions_, VelArrType& velocities_, const std::uint32_t xMax_, const std::uint32_t yMax_) noexcept : positions(positions_), velocities(velocities_), xMax(xMax_), yMax(yMax_), grid(xMax, yMax)
	{
	}

	void generateStartValues() noexcept
	{
		std::random_device rd;
		std::mt19937 engine(rd());

		std::uniform_int_distribution<std::uint32_t> posXDistr(CIRCLE_RADIUS, xMax - CIRCLE_RADIUS);
		std::uniform_int_distribution<std::uint32_t> posYDistr(CIRCLE_RADIUS, yMax - CIRCLE_RADIUS);

		std::vector<glm::vec2> tempVec;
		tempVec.reserve(positions.size());
		constexpr std::uint32_t MAX_ITER = 1000000;
		std::uint32_t i = 0;
		while (tempVec.size() < positions.size())
		{
			++i;
			auto temp = glm::vec2(posXDistr(engine), posYDistr(engine));
			if (tempVec.cend() == std::find_if(tempVec.cbegin(), tempVec.cend(), [&temp](const glm::vec2& vec) {return glm::distance(temp, vec) <= 2.f * CIRCLE_RADIUS; }))
			{
				tempVec.push_back(temp);
			}
			
			if (i == MAX_ITER)
			{
				std::cout << "Can't find nonoverlapping coordinates for a new circle, breaking.\n";
				break;
			}
		}
		std::copy(tempVec.begin(), tempVec.end(), positions.begin());

		std::uniform_real_distribution<float> velXDistr(vxMin, vxMax);
		std::uniform_real_distribution<float> velYDistr(vyMin, vyMax);
		for (auto& velocity : velocities)
		{
			velocity.x = velXDistr(engine);
			velocity.y = velYDistr(engine);
		}

		grid.initializeGrid();
	}

	void doIteration() noexcept
	{
		grid.clearGridCells();
		for (std::uint32_t i = 0; i < positions.size(); ++i)
		{
			positions[i] += deltaT * velocities[i];
			grid.addParticleToGridCell(i, positions[i]);
		}

		resolveConflicts();
	}
};
#endif