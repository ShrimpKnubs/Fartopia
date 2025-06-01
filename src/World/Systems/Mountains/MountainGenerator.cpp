// File: EmergentKingdoms/src/World/Systems/Mountains/MountainGenerator.cpp
#include "MountainGenerator.h"
#include "../../../Core/BaseConfig.h"
#include "../../GenerationSteps/WorldGenUtils.h"
#include <iostream>
#include <random>
#include <cmath>
#include <algorithm>
#include <omp.h>

namespace World {
namespace Systems {
namespace Mountains {

MountainGenerator::MountainGenerator() {
    massif_radius_factor = MOUNTAIN_MASSIF_RADIUS_FACTOR;
    massif_falloff_steepness = MOUNTAIN_MASSIF_FALLOFF_STEEPNESS;

    range_noise_frequency = MOUNTAIN_RANGE_NOISE_FREQUENCY;
    range_noise_octaves = MOUNTAIN_RANGE_NOISE_OCTAVES;
    range_noise_lacunarity = MOUNTAIN_RANGE_NOISE_LACUNARITY;
    range_noise_gain = MOUNTAIN_RANGE_NOISE_GAIN;
    range_threshold_min = MOUNTAIN_RANGE_THRESHOLD_MIN;
    
    range_base_height_min = MOUNTAIN_RANGE_BASE_HEIGHT_MIN;
    range_peak_height_max = MOUNTAIN_RANGE_PEAK_HEIGHT_MAX;

    detail_noise_frequency = MOUNTAIN_DETAIL_NOISE_FREQUENCY;
    detail_noise_octaves = MOUNTAIN_DETAIL_NOISE_OCTAVES;
    detail_noise_strength = MOUNTAIN_DETAIL_NOISE_STRENGTH;

    range_noise_gen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    range_noise_gen.SetFractalType(FastNoiseLite::FractalType_Ridged); // RESTORED TO RIDGED
    range_noise_gen.SetFractalLacunarity(range_noise_lacunarity);
    range_noise_gen.SetFractalGain(range_noise_gain);

    detail_noise_gen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    detail_noise_gen.SetFractalType(FastNoiseLite::FractalType_FBm);
}

void MountainGenerator::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    unsigned int current_step_seed = base_world_seed + static_cast<unsigned int>(step_seed_offset);
    
    range_noise_gen.SetSeed(static_cast<int>(current_step_seed));
    range_noise_gen.SetFrequency(range_noise_frequency);
    range_noise_gen.SetFractalOctaves(range_noise_octaves);

    detail_noise_gen.SetSeed(static_cast<int>(current_step_seed + 1));
    detail_noise_gen.SetFrequency(detail_noise_frequency);
    detail_noise_gen.SetFractalOctaves(detail_noise_octaves);

    std::mt19937 rng(current_step_seed + 2);
    // Keep massif centers within the map boundaries to simplify wrapping logic for now
    std::uniform_real_distribution<float> x_dist(world_data.map_width * 0.2f, world_data.map_width * 0.8f);
    std::uniform_real_distribution<float> y_dist(world_data.map_height * 0.2f, world_data.map_height * 0.8f);
    
    // For simplicity and to reduce seam potential, let's try one dominant massif region first.
    // Multiple overlapping massifs can be complex to get right without seams.
    float massif_center_x = x_dist(rng);
    float massif_center_y = y_dist(rng);
    
    // Calculate actual radius in map units
    float actual_max_massif_radius = static_cast<float>(std::min(world_data.map_width, world_data.map_height)) * massif_radius_factor;

    std::cout << "  Mountains: Generating mountain ranges (Ridged) centered near (" << massif_center_x << ", " << massif_center_y 
              << ") with actual radius " << actual_max_massif_radius << std::endl;
    float map_width_float = static_cast<float>(world_data.map_width);

    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            float fx = static_cast<float>(x);
            float fy = static_cast<float>(y);

            // 1. Calculate massif strength (Simplified and robust wrapping)
            float dx_to_center = fx - massif_center_x;
            float dy_to_center = fy - massif_center_y;

            // Correct wrapping for dx_to_center
            if (std::abs(dx_to_center) > world_data.map_width / 2.0f) {
                dx_to_center -= static_cast<float>(world_data.map_width) * ( (dx_to_center > 0) ? 1.0f : -1.0f );
            }

            float dist_sq_to_center = dx_to_center * dx_to_center + dy_to_center * dy_to_center;
            float massif_strength = 0.0f;

            if (actual_max_massif_radius > 0.001f) {
                float max_radius_sq = actual_max_massif_radius * actual_max_massif_radius;
                if (dist_sq_to_center < max_radius_sq) {
                    massif_strength = 1.0f - (dist_sq_to_center / max_radius_sq); // Linear falloff
                    massif_strength = std::pow(massif_strength, massif_falloff_steepness); // Apply power curve
                }
            }
            massif_strength = Generation::Utils::clamp_val(massif_strength, 0.0f, 1.0f);

            if (massif_strength < 0.01f) { // If outside massif influence, skip
                continue;
            }

            // 2. Sample range structure noise (Ridged noise for sharp peaks)
            float range_noise_raw = Generation::Utils::getCylindricalWrappedNoise(range_noise_gen, fx, fy, map_width_float); // -1 to 1
            // Ridged noise output is often in [0, 1] or [-1, 1] depending on implementation. FastNoiseLite Ridged is usually -1 to 1.
            // We want to use the "ridges" (higher values).
            float range_effect = (range_noise_raw + 1.0f) / 2.0f; // Normalize to 0-1

            if (range_effect > range_threshold_min) {
                // 3. Calculate strength from range noise
                float ridge_strength_factor = (range_effect - range_threshold_min) / (1.0f - range_threshold_min);
                ridge_strength_factor = Generation::Utils::clamp_val(ridge_strength_factor, 0.0f, 1.0f);
                ridge_strength_factor = std::pow(ridge_strength_factor, 1.75f); // Sharpen ridges more

                // 4. Determine target mountain height
                float effective_strength = ridge_strength_factor * massif_strength;
                effective_strength = Generation::Utils::clamp_val(effective_strength, 0.0f, 1.0f);

                float height_scaling_strength = std::pow(effective_strength, 0.6f); // Power < 1 makes it reach peak height faster

                float target_h_before_detail = range_base_height_min + height_scaling_strength * (range_peak_height_max - range_base_height_min);
                
                // 5. Add detail/cragginess
                float detail_val = Generation::Utils::getCylindricalWrappedNoise(detail_noise_gen, fx, fy, map_width_float); // -1 to 1
                // Apply detail more strongly where effective_strength is high
                float applied_detail_strength = detail_noise_strength * effective_strength; 
                float final_mountain_h = target_h_before_detail + detail_val * applied_detail_strength;

                final_mountain_h = Generation::Utils::clamp_val(final_mountain_h, 0.0f, range_peak_height_max);
                
                // 6. Combine with existing terrain: mountains primarily add height
                float current_terrain_h = world_data.heightmap_data[index];
                float new_h;

                // Mountains should raise terrain, use max or a strong blend.
                // If mountain calc is higher, use it, influenced by effective_strength.
                if (final_mountain_h > current_terrain_h) {
                    // Blend based on strength, ensuring mountains make a mark
                    float blend_factor = Generation::Utils::clamp_val(effective_strength * 2.0f, 0.0f, 1.0f); // Stronger blend
                    new_h = current_terrain_h * (1.0f - blend_factor) + final_mountain_h * blend_factor;
                    new_h = std::max(new_h, current_terrain_h); // Ensure it doesn't lower terrain
                } else {
                     // If calculated mountain is lower, only slightly raise based on strength if at all
                     // This prevents low "mountain noise" from flattening interesting base terrain
                     new_h = current_terrain_h + (final_mountain_h - current_terrain_h) * effective_strength * 0.1f;
                     new_h = std::max(current_terrain_h, new_h); // Still ensure no lowering
                }
                
                world_data.heightmap_data[index] = Generation::Utils::clamp_val(new_h, 0.0f, 1.0f);
            }
        }
    }
    std::cout << "  Mountains: Finished mountain range generation (Ridged)." << std::endl;
}

} // namespace Mountains
} // namespace Systems
} // namespace World