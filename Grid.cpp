#include "Grid.hpp"

/*
Space is uniformly partitioned, each grid cell is a square with length 2 * CIRCLE_RADIUS.
Cells are stored in one dimensional vector. For 3x3 space partition cells are labbeled as follows:
6 7 8
3 4 5
0 1 2
*/

void Grid2d::initializeGrid()
{
	gridCells.reserve(xCellsCount * yCellsCount);
	for (std::uint32_t i = 0; i < xCellsCount * yCellsCount; ++i)
	{
		gridCells.push_back({});
	}

	for (auto& cell : gridCells)
	{
		cell.reserve(3);
	}
}

void Grid2d::addParticleToGridCell(const std::uint32_t i, const glm::vec2 position) noexcept
{
	std::uint32_t xCellId = static_cast<std::uint32_t>(fabs(position.x) / xLen);
	std::uint32_t yCellId = static_cast<std::uint32_t>(fabs(position.y) / yLen);

	if (xCellId > xCellsCount - 1)
	{
		xCellId = xCellsCount - 1;
	}
	if (yCellId > yCellsCount - 1)
	{
		yCellId = yCellsCount - 1;
	}
	gridCells[xCellId + yCellId * xCellsCount].emplace_back(i);
}

void Grid2d::clearGridCells() noexcept
{
	for (auto& cell : gridCells)
	{
		cell.clear();
	}
}