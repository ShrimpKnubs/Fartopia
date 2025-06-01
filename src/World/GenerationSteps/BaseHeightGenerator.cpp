// File: EmergentKingdoms/src/World/GenerationSteps/BaseHeightGenerator.cpp
#include "BaseHeightGenerator.h"
#include "../../Core/BaseConfig.h" // FIXED: Changed from Config.h to BaseConfig.h
#include "WorldGenUtils.h"
#include <iostream>
#include <algorithm>
#include <omp.h>

namespace World {
namespace Generation {

// Local constants specific to height generation (moved from old Config.h)
namespace {
    const float BASIN_CARVING_NOISE_FREQUENCY = Core::BASE_NOISE_FREQUENCY * 3.5f;
    const int BASIN_CARVING_NOISE_OCTAVES = 3;
    const float BASIN_CARVING_STRENGTH = 0.06f;
    const float BASIN_CARVING_HEIGHT_THRESHOLD_MAX = Core::TERRAIN_PLAINS_HIGH + 0.03f;
    const float BASIN_CARVING_NOISE_TRIGGER_MIN = 0.2f;
    const float BASIN_CARVING_NOISE_TRIGGER_MAX = 0.8f;
}

BaseHeightGenerator::BaseHeightGenerator() {
    base_noise_frequency_param = Core::BASE_NOISE_FREQUENCY;
    base_noise_octaves_param = Core::BASE_NOISE_OCTAVES;
    base_noise_lacunarity_param = Core::BASE_NOISE_LACUNARITY;
    base_noise_persistence_param = Core::BASE_NOISE_PERSISTENCE;
    
    detail_noise_frequency_multiplier_param = 5.0f; 
    detail_noise_octaves_param = 3;                

    // Set base generation range to allow for lowlands and some basic uplands
    terrain_generation_min_height_param = Core::TERRAIN_VERY_LOW_LAND; 
    terrain_generation_max_height_param = Core::TERRAIN_UPLANDS_HIGH; // Mountains will add on top of this

    base_height_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    base_height_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    
    detail_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    detail_noise.SetFractalType(FastNoiseLite::FractalType_FBm);

    // FIXED: Use local constants instead of Core:: namespace
    basin_carving_noise_frequency_param = BASIN_CARVING_NOISE_FREQUENCY;
    basin_carving_noise_octaves_param = BASIN_CARVING_NOISE_OCTAVES;
    basin_carving_strength_param = BASIN_CARVING_STRENGTH;
    basin_carving_height_threshold_max_param = BASIN_CARVING_HEIGHT_THRESHOLD_MAX;
    basin_carving_noise_trigger_min_param = BASIN_CARVING_NOISE_TRIGGER_MIN;
    basin_carving_noise_trigger_max_param = BASIN_CARVING_NOISE_TRIGGER_MAX;

    basin_carving_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S); 
    basin_carving_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
}

void BaseHeightGenerator::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    unsigned int current_step_seed = base_world_seed + static_cast<unsigned int>(step_seed_offset);

    base_height_noise.SetSeed(static_cast<int>(current_step_seed));
    base_height_noise.SetFractalOctaves(base_noise_octaves_param);
    base_height_noise.SetFractalLacunarity(base_noise_lacunarity_param);
    base_height_noise.SetFractalGain(base_noise_persistence_param); 
    base_height_noise.SetFrequency(base_noise_frequency_param);

    detail_noise.SetSeed(static_cast<int>(current_step_seed + 1));
    detail_noise.SetFrequency(base_noise_frequency_param * detail_noise_frequency_multiplier_param);
    detail_noise.SetFractalOctaves(detail_noise_octaves_param);

    basin_carving_noise.SetSeed(static_cast<int>(current_step_seed + 2)); 
    basin_carving_noise.SetFrequency(basin_carving_noise_frequency_param);
    basin_carving_noise.SetFractalOctaves(basin_carving_noise_octaves_param);
    
    float map_width_float = static_cast<float>(world_data.map_width);
    const size_t current_map_size = static_cast<size_t>(world_data.map_width) * static_cast<size_t>(world_data.map_height);

    std::cout << "  Generating base heightmap (tuned for more varied landforms)..." << std::endl;
    float min_h_raw = 2.0f, max_h_raw = -2.0f;

    std::vector<float> raw_heights(current_map_size); 

    #pragma omp parallel for reduction(min:min_h_raw) reduction(max:max_h_raw)
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            float fx = static_cast<float>(x);
            float fy = static_cast<float>(y);

            float base_h_val = Utils::getCylindricalWrappedNoise(base_height_noise, fx, fy, map_width_float); 
            float detail_h_val = Utils::getCylindricalWrappedNoise(detail_noise, fx, fy, map_width_float) * 0.12f; 

            float current_raw_h = base_h_val + detail_h_val;
            raw_heights[index] = current_raw_h; 
            min_h_raw = std::min(min_h_raw, current_raw_h);
            max_h_raw = std::max(max_h_raw, current_raw_h);
        }
    }
    
    float range_raw = max_h_raw - min_h_raw;
    if (range_raw < 0.0001f) range_raw = 1.0f; 
    
    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            float fx = static_cast<float>(x);
            float fy = static_cast<float>(y);

            float normalized_h_initial = (raw_heights[index] - min_h_raw) / range_raw; 

            // Apply a power curve to gently expand mid-range heights, creating more rolling terrain
            // A value like 0.85 makes mid-to-high values more common.
            // A value like 1.15 makes low-to-mid values more common.
            // Let's try to make mid-range more common for hills/uplands.
            normalized_h_initial = std::pow(normalized_h_initial, 0.90f); 

            if (normalized_h_initial < basin_carving_height_threshold_max_param) { // Basin carving logic remains
                float carve_noise_val_raw = Utils::getCylindricalWrappedNoise(basin_carving_noise, fx, fy, map_width_float);
                float carve_noise_val_norm = (carve_noise_val_raw + 1.0f) / 2.0f; 

                if (carve_noise_val_norm > basin_carving_noise_trigger_min_param &&
                    carve_noise_val_norm < basin_carving_noise_trigger_max_param) {
                    float carve_factor = (carve_noise_val_norm - basin_carving_noise_trigger_min_param) / 
                                         (basin_carving_noise_trigger_max_param - basin_carving_noise_trigger_min_param);
                    carve_factor = Utils::clamp_val(carve_factor, 0.0f, 1.0f);
                    normalized_h_initial -= basin_carving_strength_param * carve_factor;
                }
            }
            
            normalized_h_initial = Utils::clamp_val(normalized_h_initial, 0.0f, 1.0f);

            world_data.heightmap_data[index] = terrain_generation_min_height_param + normalized_h_initial * (terrain_generation_max_height_param - terrain_generation_min_height_param);
            world_data.heightmap_data[index] = Utils::clamp_val(world_data.heightmap_data[index], 0.0f, 1.0f); 
        }
    }
}

} // namespace Generation
} // namespace World