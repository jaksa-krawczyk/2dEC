#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdint>

inline constexpr std::uint32_t CIRCLE_DENSITY = 20;
inline constexpr std::uint32_t CIRCLE_RADIUS = 5;

inline constexpr float vxMax = 25.f, vyMax = 25.f, vxMin = -25.f, vyMin = -25.f;

inline constexpr float DELTA_T = 0.05f;

inline constexpr std::uint32_t TRIANGLES_PER_CIRCLE = 64;
inline constexpr std::uint32_t VERTICES_PER_CIRCLE = 3 * (TRIANGLES_PER_CIRCLE - 2);

inline constexpr std::uint32_t GRID_CELLS_PER_AXIS = 20;
inline constexpr std::uint32_t CELL_COUNT = GRID_CELLS_PER_AXIS * GRID_CELLS_PER_AXIS;
inline constexpr std::uint32_t BORDER_CELLS_COUNT = GRID_CELLS_PER_AXIS == 1 ? 1 : 4 * GRID_CELLS_PER_AXIS - 4;

consteval std::uint32_t getBallCount(const std::uint32_t xMax, const std::uint32_t yMax)
{
	auto windowArea = xMax * yMax;
	return (CIRCLE_DENSITY * windowArea) / (100 * 4 * CIRCLE_RADIUS * CIRCLE_RADIUS);
}

#endif