#include "Grid.hpp"
#include <glm/geometric.hpp>

#include <numbers>
#include <algorithm>

std::set<std::uint32_t> Grid2d::getNeighboringCells(const glm::vec2 position, const std::uint32_t xCellId, const std::uint32_t yCellId)
{
	const std::uint32_t cellCornersI = 4 * (xCellId + yCellId);

	for (Corners corner = Corners::SW; corner < Corners::All; ++corner)
	{
		auto cornerCast = static_cast<std::underlying_type_t<Corners>>(corner);
		if (auto distance = glm::distance(position, cornerVectors[cornerCast + cellCornersI]); distance < std::numbers::sqrt2_v<float> * CIRCLE_RADIUS)
		{
			return getAdjacentCells(corner, xCellId, yCellId);
		}
	}

	std::set<std::uint32_t> otherCellsIds;
	auto cornerSwId = static_cast<std::underlying_type_t<Corners>>(Corners::SW);
	auto cornerNeId = static_cast<std::underlying_type_t<Corners>>(Corners::NE);

	if (position.x - CIRCLE_RADIUS < cornerVectors[cornerSwId + cellCornersI].x)
	{
		if (xCellId)
		{
			otherCellsIds.insert(xCellId - 1 + yCellId);
		}
	}
	else if (position.x + CIRCLE_RADIUS > cornerVectors[cornerNeId + cellCornersI].x)
	{
		if (xCellId < GRID_CELLS_PER_AXIS - 1)
		{
			otherCellsIds.insert(xCellId + 1 + yCellId);
		}
	}

	if (position.y - CIRCLE_RADIUS < cornerVectors[cornerSwId + cellCornersI].y)
	{
		if (yCellId)
		{
			otherCellsIds.insert(xCellId + yCellId - GRID_CELLS_PER_AXIS);
		}
	}
	else if (position.y + CIRCLE_RADIUS > cornerVectors[cornerNeId + cellCornersI].y)
	{
		if (yCellId < yCellIdMax)
		{
			otherCellsIds.insert(xCellId + yCellId + GRID_CELLS_PER_AXIS);
		}
	}

	return otherCellsIds;
}

std::set<std::uint32_t> Grid2d::getAdjacentCells(const Corners corner, const std::uint32_t xCellId, const std::uint32_t yCellId)
{
	std::set<std::uint32_t> adjacentCells;

	switch (corner)
	{
		case Corners::SW:
		{
			auto left = xCellId > 0 ? xCellId - 1 : 0;
			auto down = yCellId > 0 ? yCellId - GRID_CELLS_PER_AXIS : 0;
			adjacentCells.insert(left + yCellId);
			adjacentCells.insert(left + down);
			adjacentCells.insert(xCellId + down);
			break;
		}

		case Corners::NW:
		{
			auto left = xCellId > 0 ? xCellId - 1 : 0;
			auto up = yCellId < yCellIdMax ? yCellId + GRID_CELLS_PER_AXIS : yCellId;
			adjacentCells.insert(left + yCellId);
			adjacentCells.insert(left + up);
			adjacentCells.insert(xCellId + up);
			break;
		}

		case Corners::SE:
		{
			auto right = xCellId < GRID_CELLS_PER_AXIS - 1 ? xCellId + 1 : xCellId;
			auto down = yCellId > 0 ? yCellId - GRID_CELLS_PER_AXIS : 0;
			adjacentCells.insert(right + yCellId);
			adjacentCells.insert(right + down);
			adjacentCells.insert(xCellId + down);
			break;
		}

		case Corners::NE:
		{
			auto right = xCellId < GRID_CELLS_PER_AXIS - 1 ? xCellId + 1 : xCellId;
			auto up = yCellId < yCellIdMax ? yCellId + GRID_CELLS_PER_AXIS : yCellId;
			adjacentCells.insert(right + yCellId);
			adjacentCells.insert(right + up);
			adjacentCells.insert(xCellId + up);
			break;
		}
	}

	adjacentCells.erase(xCellId + yCellId);
	return adjacentCells;
}

void Grid2d::findBorderCellsIds()
{
	if (BORDER_CELLS_COUNT > 1)
	{
		std::uint32_t j = 0;
		for (; j < GRID_CELLS_PER_AXIS; ++j)
		{
			borderCellsIds[j] = j;
		}
		for (std::uint32_t i = j - 1 + GRID_CELLS_PER_AXIS; i < CELL_COUNT; i += GRID_CELLS_PER_AXIS)
		{
			borderCellsIds[j] = i;
			++j;
		}

		for (std::uint32_t i = CELL_COUNT - 2; i >= GRID_CELLS_PER_AXIS * (GRID_CELLS_PER_AXIS - 1); --i)
		{
			borderCellsIds[j] = i;
			++j;
		}
		for (std::uint32_t i = CELL_COUNT - 2 * GRID_CELLS_PER_AXIS; i > 0; i -= GRID_CELLS_PER_AXIS)
		{
			borderCellsIds[j] = i;
			++j;
		}

		std::sort(borderCellsIds.begin(), borderCellsIds.end());
	}
	else
	{
		borderCellsIds[0] = 0;
	}
}

void Grid2d::initializeGrid()
{
	auto it = cornerVectors.begin();
	for (std::uint32_t i = 0; i < CELL_COUNT; ++i)
	{
		glm::vec2 SW((i % GRID_CELLS_PER_AXIS) * xLen, (i / GRID_CELLS_PER_AXIS) * yLen);
		glm::vec2 SE(xLen  + (i % GRID_CELLS_PER_AXIS) * xLen, (i / GRID_CELLS_PER_AXIS) * yLen);
		glm::vec2 NE(xLen  + (i % GRID_CELLS_PER_AXIS) * xLen, yLen + (i / GRID_CELLS_PER_AXIS) * yLen);
		glm::vec2 NW((i % GRID_CELLS_PER_AXIS) * xLen, yLen + (i / GRID_CELLS_PER_AXIS) * yLen);
		*it++ = SW;
		*it++ = SE;
		*it++ = NE;
		*it++ = NW;
	}

	findBorderCellsIds();
 
	for (auto& cell : gridCells)
	{
		cell.reserve(20);
	}
}

void Grid2d::addParticleToGridCell(const std::uint32_t i, const glm::vec2 position)
{
	CellId id = getParticleCellId(position);
	const std::uint16_t cellId = id.xCellId + id.yCellId;
	gridCells[cellId].emplace_back(i);

	if (auto adjacentCells = getNeighboringCells(position, id.xCellId, id.yCellId); adjacentCells.size())
	{
		for (const auto adjCellId : adjacentCells)
		{
			gridCells[adjCellId].emplace_back(i);
		}
	}
}

void Grid2d::clearGridCells()
{
	for (auto& cell : gridCells)
	{
		cell.clear();
	}
}

CellId Grid2d::getParticleCellId(const glm::vec2 position)
{
	std::uint16_t xCellId = static_cast<std::uint16_t>(fabs(position.x) / xLen);
	std::uint16_t yCellId = static_cast<std::uint16_t>(fabs(position.y) / yLen) * GRID_CELLS_PER_AXIS;

	if (xCellId > GRID_CELLS_PER_AXIS - 1)
	{
		xCellId = GRID_CELLS_PER_AXIS - 1;
	}

	if (yCellId >= yCellIdMax)
	{
		yCellId = yCellIdMax;
	}
	return {xCellId, yCellId};
}