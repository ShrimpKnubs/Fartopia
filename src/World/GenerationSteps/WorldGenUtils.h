// File: EmergentKingdoms/src/World/GenerationSteps/WorldGenUtils.h
#pragma once
#include <algorithm> // For std::min, std::max
#include <cmath>     // For M_PI, std::cos, std::sin
#include "../../Core/FastNoiseLite.h" // For FastNoiseLite

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace World {
namespace Generation {
namespace Utils {

inline float clamp_val(float value, float min_val, float max_val) {
    return std::max(min_val, std::min(value, max_val));
}

inline float getCylindricalWrappedNoise(FastNoiseLite& noise_generator, float fx, float fy, float current_map_width) {
    if (current_map_width <= 0) return noise_generator.GetNoise(fx, fy);
    float u = fx / current_map_width;
    float angle = u * 2.0f * static_cast<float>(M_PI);
    float effective_radius_scaled = current_map_width / (2.0f * static_cast<float>(M_PI));
    float noise_x_sample = effective_radius_scaled * std::cos(angle);
    float noise_z_sample = effective_radius_scaled * std::sin(angle);
    return noise_generator.GetNoise(noise_x_sample, fy, noise_z_sample);
}

} // namespace Utils
} // namespace Generation
} // namespace World