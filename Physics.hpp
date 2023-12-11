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
	Grid2d grid;

	const std::uint32_t xMax;
	const std::uint32_t yMax;

	void checkXCollisions(const std::uint32_t i, const float deltaSubStep) noexcept
	{
		if (positions[i].x <= CIRCLE_RADIUS)
		{
			positions[i] -= deltaSubStep * velocities[i];

			float deltaSubStepTBoundary = (CIRCLE_RADIUS - positions[i].x) / velocities[i].x;
			float deltaSubStepTReflection = deltaSubStep - deltaSubStepTBoundary;

			velocities[i].x = -velocities[i].x;
			positions[i].x = CIRCLE_RADIUS;
			positions[i] += deltaSubStepTReflection * velocities[i];
		}
		else if (positions[i].x >= xMax - CIRCLE_RADIUS)
		{
			positions[i] -= deltaSubStep * velocities[i];

			float deltaSubStepTBoundary = (xMax - CIRCLE_RADIUS - positions[i].x) / velocities[i].x;
			float deltaSubStepTReflection = deltaSubStep - deltaSubStepTBoundary;

			velocities[i].x = -velocities[i].x;
			positions[i].x = static_cast<float>(xMax - CIRCLE_RADIUS);
			positions[i] += deltaSubStepTReflection * velocities[i];
		}
	}

	void checkYCollisions(const std::uint32_t i, const float deltaSubStep) noexcept
	{
		if (positions[i].y <= CIRCLE_RADIUS)
		{
			positions[i] -= deltaSubStep * velocities[i];

			float deltaSubStepTBoundary = (CIRCLE_RADIUS - positions[i].y) / velocities[i].y;
			float deltaSubStepTReflection = deltaSubStep - deltaSubStepTBoundary;

			velocities[i].y = -velocities[i].y;
			positions[i].y = CIRCLE_RADIUS;
			positions[i] += deltaSubStepTReflection * velocities[i];
		}
		else if (positions[i].y >= yMax - CIRCLE_RADIUS)
		{
			positions[i] -= deltaSubStep * velocities[i];

			float deltaSubStepTBoundary = (yMax - CIRCLE_RADIUS - positions[i].y) / velocities[i].y;
			float deltaSubStepTReflection = deltaSubStep - deltaSubStepTBoundary;

			velocities[i].y = -velocities[i].y;
			positions[i].y = static_cast<float>(yMax - CIRCLE_RADIUS);
			positions[i] += deltaSubStepTReflection * velocities[i];
		}
	}

	void resolveBoundaryCollisions(const float deltaSubStep) noexcept
	{
		for (std::uint32_t i = 0; i < positions.size(); ++i)
		{
			checkXCollisions(i, deltaSubStep);
			checkYCollisions(i, deltaSubStep);
		}
	}

	float getCollisionTime(const glm::vec2 relativePosition, const glm::vec2 relativePositionPrev, const glm::vec2 relativeVelocity) noexcept
	{
		float aLin = (relativePosition.y - relativePositionPrev.y) / (relativePosition.x - relativePositionPrev.x);
		float bLin = relativePosition.y - aLin * relativePosition.x;

		float aQuad = 1.f + aLin * aLin;
		float deltaQuad = 4.f * (4.f * CIRCLE_RADIUS * CIRCLE_RADIUS * aQuad - bLin * bLin);

		float bQuad = 2.f * aLin * bLin;
		float x1 = (-bQuad - sqrtf(deltaQuad)) / (2.f * aQuad);
		float x2 = (-bQuad + sqrtf(deltaQuad)) / (2.f * aQuad);
		float y1 = aLin * x1 + bLin;
		float y2 = aLin * x2 + bLin;

		glm::vec2 v1(x1, y1);
		glm::vec2 v2(x2, y2);

		float t1 = glm::distance(v1, relativePosition) / glm::length(relativeVelocity);
		float t2 = glm::distance(v2, relativePosition) / glm::length(relativeVelocity);

		auto pos1 = relativePosition - t1 * relativeVelocity;
		auto pos2 = relativePosition - t2 * relativeVelocity;

		constexpr glm::vec2 posJ(0.f, 0.f);
		if (fabs(glm::distance(pos1, posJ) - 2.f * CIRCLE_RADIUS) < fabs(glm::distance(pos2, posJ) - 2.f * CIRCLE_RADIUS))
		{
			return t1;
		}
		return t2;
	}

	glm::vec2 solveDistQuadratic(const glm::vec2 reference, const float distance) noexcept
	{
		float aLin = reference.y / reference.x;
		float aQuad = 1.f + aLin * aLin;
		float cQuad = distance * distance;
		float x1 = sqrtf(cQuad / aQuad);

		return {x1, aLin * x1};
	}

	void moveAlongsideCenterLine(glm::vec2& posI, glm::vec2& posJ, const float penetrationLength) noexcept
	{
		float distanceToMove = CIRCLE_RADIUS - penetrationLength / 2.f;
		glm::vec2 posICorrection = solveDistQuadratic(posJ - posI, distanceToMove);
		glm::vec2 posJCorrection = solveDistQuadratic(posI - posJ, distanceToMove);

		glm::vec2 tmp = posI;
		if (glm::distance(posJ, posICorrection + posI) > glm::distance(posJ, posI - posICorrection))
		{
			posI += posICorrection;
		}
		else
		{
			posI -= posICorrection;
		}

		if (glm::distance(tmp, posJCorrection + posJ) > glm::distance(tmp, posJ - posJCorrection))
		{
			posJ += posJCorrection;
		}
		else
		{
			posJ -= posJCorrection;
		}
	}

	void updateNewVelocities(const std::uint32_t i, const std::uint32_t j) noexcept
	{
		auto velocity_i = velocities[i];
		auto diffPos_ij = positions[i] - positions[j];
		auto lengthDiffPos_ij = glm::length(positions[i] - positions[j]);

		velocities[i] -= glm::dot((velocities[i] - velocities[j]), diffPos_ij) / (lengthDiffPos_ij * lengthDiffPos_ij) * diffPos_ij;
		velocities[j] += glm::dot((velocities[j] - velocity_i), -diffPos_ij) / (lengthDiffPos_ij * lengthDiffPos_ij) * diffPos_ij;
	}

	void updateAfterCollision(const std::uint32_t i, const std::uint32_t j, const float deltaSubStep, const float penetrationLength) noexcept
	{
		auto relativePosition = positions[i] - positions[j];
		auto relativeVelocity = velocities[i] - velocities[j];
		auto relativePositionPrev = relativePosition - deltaSubStep * relativeVelocity;

		float collisionTime = getCollisionTime(relativePosition, relativePositionPrev, relativeVelocity);

		if (collisionTime > deltaSubStep)
		{
			moveAlongsideCenterLine(positions[i], positions[j], penetrationLength);
			updateNewVelocities(i, j);
			return;
		}
		positions[i] -= collisionTime * velocities[i];
		positions[j] -= collisionTime * velocities[j];

		updateNewVelocities(i, j);

		float afterCollisionTime = deltaSubStep - collisionTime;
		positions[i] += afterCollisionTime * velocities[i];
		positions[j] += afterCollisionTime * velocities[j];
	}

	void resolveCellConflicts(auto& cellParticles, const float deltaSubStep) noexcept
	{
		for (std::uint32_t i = 0; i < cellParticles.size(); ++i)
		{
			for (std::uint32_t j = 0; j < cellParticles.size(); ++j)
			{
				if (float d = glm::distance(positions[cellParticles[i].particleId], positions[cellParticles[j].particleId]); i != j && d < 2.f * CIRCLE_RADIUS)
				{
					updateAfterCollision(cellParticles[i].particleId, cellParticles[j].particleId, deltaSubStep, d);
				}
			}
		}
	}

	void resolveConflicts(const float deltaSubStep) noexcept
	{
		auto& gridCells = grid.getGridCells();

		for (auto& cell : gridCells)
		{
			resolveCellConflicts(cell, deltaSubStep);
		}

		resolveBoundaryCollisions(deltaSubStep);
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
			if (tempVec.cend() == std::find_if(tempVec.cbegin(), tempVec.cend(), [&temp](const glm::vec2& vec) {return glm::distance(temp, vec) < 1.f * CIRCLE_RADIUS;}))
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
		std::uint32_t subStepsCount = 5;
		const float deltaSubStep = DELTA_T / subStepsCount;

		while (subStepsCount--)
		{
			grid.clearGridCells();
			for (std::uint32_t i = 0; i < positions.size(); ++i)
			{
				positions[i] += deltaSubStep * velocities[i];
				grid.addParticleToGridCell(i, positions[i]);
			}

			resolveConflicts(deltaSubStep);
		}
	}
};
#endif