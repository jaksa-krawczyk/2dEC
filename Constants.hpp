#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdint>

inline constexpr std::uint32_t CIRCLE_DENSITY = 10;
inline constexpr std::uint32_t CIRCLE_RADIUS = 10;

inline constexpr float vxMax = 25.f, vyMax = 25.f, vxMin = -25.f, vyMin = -25.f;

inline constexpr float deltaT = 0.05f;

inline constexpr std::uint32_t TRIANGLES_PER_CIRCLE = 64;
inline constexpr std::uint32_t VERTICES_PER_CIRCLE = 3 * (TRIANGLES_PER_CIRCLE - 2);

constexpr std::uint32_t getBallCount(const std::uint32_t xMax, const std::uint32_t yMax)
{
	auto windowArea = xMax * yMax;
	return (CIRCLE_DENSITY * windowArea) / (100 * 4 * CIRCLE_RADIUS * CIRCLE_RADIUS);
}

#endif