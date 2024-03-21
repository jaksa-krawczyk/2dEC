#ifndef GRID_HPP
#define GRID_HPP

#include "Constants.hpp"
#include <glm/vec2.hpp>

#include <vector>

class Grid2d
{
private:
	const std::uint32_t xCellsCount;
	const std::uint32_t yCellsCount;
	const float xLen;
	const float yLen;

	using GridCellsT = std::vector<std::vector<std::uint32_t>>;
	GridCellsT gridCells;

public:
	Grid2d(const std::uint32_t xMax, const std::uint32_t yMax) noexcept : 
		xCellsCount((xMax) / (2 * CIRCLE_RADIUS)), yCellsCount((yMax) / (2 * CIRCLE_RADIUS)),
		xLen(static_cast<float>(xMax) / static_cast<float>(xCellsCount)),
		yLen(static_cast<float>(yMax) / static_cast<float>(yCellsCount)){}

	void initializeGrid();
	void addParticleToGridCell(const std::uint32_t i, const glm::vec2 position) noexcept;
	void clearGridCells() noexcept;

	const GridCellsT& getGridCells()
	{
		return gridCells;
	}

	const std::uint32_t getXCellsCount()
	{
		return xCellsCount;
	}

	const std::uint32_t getYCellsCount()
	{
		return yCellsCount;
	}
};
#endif