// File: EmergentKingdoms/src/World/GenerationSteps/SlopeAspectCalculator.cpp
#include "SlopeAspectCalculator.h"
#include "../../Core/BaseConfig.h" // For thresholds
#include "WorldGenUtils.h"   // For M_PI (if not defined elsewhere)
#include <iostream>
#include <cmath>    // For std::fabs, std::atan2, std::sqrt
#include <algorithm> // For std::max
#include <omp.h>

#ifndef M_PI // Ensure M_PI is defined
#define M_PI 3.14159265358979323846
#endif

namespace World {
namespace Generation {

SlopeAspectCalculator::SlopeAspectCalculator() {
    slope_threshold_very_steep = Core::SLOPE_THRESHOLD_VERY_STEEP;
    terrain_mountain_mid_height = Core::TERRAIN_MOUNTAIN_MID;
}

void SlopeAspectCalculator::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    (void)base_world_seed; // Not used
    (void)step_seed_offset; // Not used
    std::cout << "  Calculating slope and aspect..." << std::endl;

    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t current_idx = static_cast<size_t>(y) * world_data.map_width + x;
            float h_c = world_data.heightmap_data[current_idx];

            float h_n = world_data.getWrappedHeight(x, y - 1);
            float h_s = world_data.getWrappedHeight(x, y + 1);
            float h_w = world_data.getWrappedHeight(x - 1, y);
            float h_e = world_data.getWrappedHeight(x + 1, y);
            float h_nw = world_data.getWrappedHeight(x - 1, y - 1);
            float h_ne = world_data.getWrappedHeight(x + 1, y - 1);
            float h_sw = world_data.getWrappedHeight(x - 1, y + 1);
            float h_se = world_data.getWrappedHeight(x + 1, y + 1);

            float dz_dx = (h_ne + 2.0f * h_e + h_se) - (h_nw + 2.0f * h_w + h_sw);
            float dz_dy = (h_sw + 2.0f * h_s + h_se) - (h_nw + 2.0f * h_n + h_ne);
            dz_dx /= 8.0f;
            dz_dy /= 8.0f;

            float max_diff = 0.0f;
            max_diff = std::max(max_diff, std::fabs(h_c - h_n));
            max_diff = std::max(max_diff, std::fabs(h_c - h_s));
            max_diff = std::max(max_diff, std::fabs(h_c - h_w));
            max_diff = std::max(max_diff, std::fabs(h_c - h_e));
            world_data.slope_map[current_idx] = max_diff;

            if ((std::fabs(dz_dx) < 1e-7 && std::fabs(dz_dy) < 1e-7) || world_data.slope_map[current_idx] < 0.0001f) {
                world_data.aspect_map[current_idx] = SlopeAspect::FLAT;
            } else {
                float angle_rad = std::atan2(dz_dy, dz_dx); 
                float angle_deg = angle_rad * 180.0f / static_cast<float>(M_PI);
                
                angle_deg = 90.0f - angle_deg; 
                if (angle_deg < 0.0f) angle_deg += 360.0f;

                if (world_data.slope_map[current_idx] > slope_threshold_very_steep * 1.1f && h_c > terrain_mountain_mid_height) {
                    world_data.aspect_map[current_idx] = SlopeAspect::STEEP_PEAK;
                }
                else if (angle_deg >= 337.5f || angle_deg < 22.5f) world_data.aspect_map[current_idx] = SlopeAspect::NORTH;
                else if (angle_deg >= 22.5f && angle_deg < 67.5f) world_data.aspect_map[current_idx] = SlopeAspect::NORTHEAST;
                else if (angle_deg >= 67.5f && angle_deg < 112.5f) world_data.aspect_map[current_idx] = SlopeAspect::EAST;
                else if (angle_deg >= 112.5f && angle_deg < 157.5f) world_data.aspect_map[current_idx] = SlopeAspect::SOUTHEAST;
                else if (angle_deg >= 157.5f && angle_deg < 202.5f) world_data.aspect_map[current_idx] = SlopeAspect::SOUTH;
                else if (angle_deg >= 202.5f && angle_deg < 247.5f) world_data.aspect_map[current_idx] = SlopeAspect::SOUTHWEST;
                else if (angle_deg >= 247.5f && angle_deg < 292.5f) world_data.aspect_map[current_idx] = SlopeAspect::WEST;
                else if (angle_deg >= 292.5f && angle_deg < 337.5f) world_data.aspect_map[current_idx] = SlopeAspect::NORTHWEST;
                else world_data.aspect_map[current_idx] = SlopeAspect::FLAT;
            }
        }
    }
}

} // namespace Generation
} // namespace World