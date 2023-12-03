#ifndef GRID_HPP
#define GRID_HPP

#include "Constants.hpp"
#include <glm/vec2.hpp>

#include <vector>
#include <array>
#include <set>
#include <optional>

struct ParticleCellInfo
{
	std::uint32_t particleId = 0;
	ParticleCellInfo(const std::uint32_t i) noexcept : particleId(i) {}
};

enum class Corners
{
	SW = 0,
	SE,
	NE,
	NW,
	All
};

inline Corners& operator++(Corners& corner)
{
	corner = static_cast<Corners>(static_cast<std::underlying_type_t<Corners>>(corner) + 1);
	return corner;
}

class Grid2d
{
private:
	const float xLen;
	const float yLen;

	std::array<glm::vec2, 4 * CELL_COUNT> cornerVectors;
	using GridCellsT = std::array<std::vector<ParticleCellInfo>, CELL_COUNT>;
	GridCellsT gridCells;

	std::set<std::uint32_t> getNeighboringCells(const glm::vec2 position, const std::uint32_t xCellId, const std::uint32_t yCellId);
	std::set<std::uint32_t> getAdjacentCells(const Corners corner, const std::uint32_t xCellId, const std::uint32_t yCellId);

public:
	Grid2d(const std::uint32_t xMax, const std::uint32_t yMax) noexcept : 
		xLen(static_cast<float>(xMax) / static_cast<float>(GRID_CELLS_PER_AXIS)), 
		yLen(static_cast<float>(yMax) / static_cast<float>(GRID_CELLS_PER_AXIS)) {}

	void initializeGrid();
	void addParticleToGridCell(const std::uint32_t i, const glm::vec2 position);
	void clearGridCells();

	GridCellsT& getGridCells()
	{
		return gridCells;
	}
};
#endif