// File: EmergentKingdoms/src/World/Systems/Vegetation/VegetationGenerator.cpp
#include "VegetationGenerator.h"
#include "../../GenerationSteps/WorldGenUtils.h"
#include "../../../Core/FastNoiseLite.h"
#include "VegetationConfig.h"
#include <iostream>
#include <random>

namespace World {
namespace Systems {
namespace Vegetation {

VegetationGenerator::VegetationGenerator() {
    // Initialize noise generators with different seeds for varied patterns
}

void VegetationGenerator::generateVegetationMaps(WorldData& world_data, unsigned int base_seed) {
    const int map_size = world_data.map_width * world_data.map_height;
    
    // Initialize all vegetation maps
    tree_density_map.resize(map_size, 0.0f);
    bush_density_map.resize(map_size, 0.0f);
    flower_density_map.resize(map_size, 0.0f);
    rock_placement_map.resize(map_size, 0.0f);
    resource_placement_map.resize(map_size, 0.0f);
    wind_direction_map.resize(map_size, 0);
    
    std::cout << "    Vegetation: Generating natural distribution patterns..." << std::endl;
    
    // Setup noise generators for natural distribution
    setupNoiseGenerators(base_seed);
    
    // Generate vegetation density patterns
    generateTreeDistribution(world_data);
    generateBushDistribution(world_data);
    generateFlowerDistribution(world_data);
    generateRockDistribution(world_data);
    generateResourceDistribution(world_data);
    generateWindPatterns(world_data);
    
    std::cout << "    Vegetation: Completed natural distribution generation." << std::endl;
}

void VegetationGenerator::setupNoiseGenerators(unsigned int base_seed) {
    // Tree distribution - large scale patterns for forests
    tree_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    tree_noise.SetSeed(static_cast<int>(base_seed + 100));
    tree_noise.SetFrequency(0.008f);  // Large scale forest patterns
    tree_noise.SetFractalOctaves(4);
    tree_noise.SetFractalLacunarity(2.0f);
    tree_noise.SetFractalGain(0.5f);
    
    // Bush distribution - medium scale clustering
    bush_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    bush_noise.SetSeed(static_cast<int>(base_seed + 200));
    bush_noise.SetFrequency(0.015f);  // Medium scale patterns
    bush_noise.SetFractalOctaves(3);
    
    // Flower distribution - small scale meadow patterns
    flower_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    flower_noise.SetSeed(static_cast<int>(base_seed + 300));
    flower_noise.SetFrequency(0.025f);  // Smaller meadow patterns
    flower_noise.SetFractalOctaves(2);
    
    // Rock distribution - scattered geological features
    rock_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    rock_noise.SetSeed(static_cast<int>(base_seed + 400));
    rock_noise.SetFrequency(0.012f);
    
    // Resource distribution - rare but clustered deposits
    resource_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    resource_noise.SetSeed(static_cast<int>(base_seed + 500));
    resource_noise.SetFrequency(0.006f);  // Large scale geological patterns
    
    // Wind patterns - regional wind flow
    wind_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    wind_noise.SetSeed(static_cast<int>(base_seed + 600));
    wind_noise.SetFrequency(0.003f);  // Very large scale weather patterns
}

void VegetationGenerator::generateTreeDistribution(WorldData& world_data) {
    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            float height = world_data.heightmap_data[index];
            float slope = world_data.slope_map[index];
            
            // Trees don't grow in unsuitable conditions
            if (height < TREE_MIN_HEIGHT || height > TREE_MAX_HEIGHT || 
                slope > TREE_MAX_SLOPE ||
                world_data.is_river_tile[index] || world_data.is_lake_tile[index]) {
                tree_density_map[index] = 0.0f;
                continue;
            }
            
            // Get base tree density from noise
            float tree_noise_val = Generation::Utils::getCylindricalWrappedNoise(
                tree_noise, static_cast<float>(x), static_cast<float>(y), 
                static_cast<float>(world_data.map_width)
            );
            tree_noise_val = (tree_noise_val + 1.0f) / 2.0f;  // Normalize to 0-1
            
            // Height preference - trees like mid-elevation areas
            float height_factor = 1.0f;
            if (height >= Core::TERRAIN_ROLLING_HILLS_LOW && height <= Core::TERRAIN_UPLANDS_LOW) {
                height_factor = 1.3f;  // Bonus for ideal tree elevations
            } else if (height < Core::TERRAIN_PLAINS_HIGH) {
                height_factor = 0.7f;  // Lower density in very low areas
            }
            
            // Slope preference - gentle slopes are better
            float slope_factor = 1.0f - (slope / TREE_MAX_SLOPE) * 0.4f;
            
            // Distance from water bonus (but not too close)
            float water_factor = 1.0f;
            // Trees benefit from being near (but not too near) water
            // This would need distance_to_water calculation
            
            tree_density_map[index] = tree_noise_val * height_factor * slope_factor * water_factor;
        }
    }
}

void VegetationGenerator::generateBushDistribution(WorldData& world_data) {
    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            float height = world_data.heightmap_data[index];
            
            // Bushes are more tolerant than trees
            if (world_data.is_river_tile[index] || world_data.is_lake_tile[index]) {
                bush_density_map[index] = 0.0f;
                continue;
            }
            
            float bush_noise_val = Generation::Utils::getCylindricalWrappedNoise(
                bush_noise, static_cast<float>(x), static_cast<float>(y), 
                static_cast<float>(world_data.map_width)
            );
            bush_noise_val = (bush_noise_val + 1.0f) / 2.0f;
            
            // Bushes like forest edges and clearings
            float tree_interaction = 1.0f - tree_density_map[index] * 0.3f;  // Some inverse correlation
            
            // Height tolerance - bushes are more adaptable
            float height_factor = 1.0f;
            if (height >= TREE_MIN_HEIGHT && height <= TREE_MAX_HEIGHT) {
                height_factor = 1.2f;  // Good growing conditions
            }
            
            bush_density_map[index] = bush_noise_val * tree_interaction * height_factor;
        }
    }
}

void VegetationGenerator::generateFlowerDistribution(WorldData& world_data) {
    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            float height = world_data.heightmap_data[index];
            
            if (world_data.is_river_tile[index] || world_data.is_lake_tile[index]) {
                flower_density_map[index] = 0.0f;
                continue;
            }
            
            float flower_noise_val = Generation::Utils::getCylindricalWrappedNoise(
                flower_noise, static_cast<float>(x), static_cast<float>(y), 
                static_cast<float>(world_data.map_width)
            );
            flower_noise_val = (flower_noise_val + 1.0f) / 2.0f;
            
            // Flowers love open meadows (low tree density)
            float open_area_bonus = 1.0f + (1.0f - tree_density_map[index]) * 0.5f;
            
            // Prefer lower elevations for most flowers
            float height_factor = 1.0f;
            if (height >= Core::TERRAIN_VERY_LOW_LAND && height <= Core::TERRAIN_PLAINS_HIGH) {
                height_factor = 1.4f;  // Ideal meadow conditions
            }
            
            flower_density_map[index] = flower_noise_val * open_area_bonus * height_factor;
        }
    }
}

void VegetationGenerator::generateRockDistribution(WorldData& world_data) {
    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            float height = world_data.heightmap_data[index];
            float slope = world_data.slope_map[index];
            
            if (world_data.is_river_tile[index] || world_data.is_lake_tile[index]) {
                rock_placement_map[index] = 0.0f;
                continue;
            }
            
            float rock_noise_val = Generation::Utils::getCylindricalWrappedNoise(
                rock_noise, static_cast<float>(x), static_cast<float>(y), 
                static_cast<float>(world_data.map_width)
            );
            rock_noise_val = (rock_noise_val + 1.0f) / 2.0f;
            
            // Rocks more likely on slopes and higher elevations
            float slope_bonus = 1.0f + slope * 3.0f;
            float height_bonus = 1.0f;
            if (height >= Core::TERRAIN_ROLLING_HILLS_LOW) {
                height_bonus = 1.0f + (height - Core::TERRAIN_ROLLING_HILLS_LOW) * 2.0f;
            }
            
            rock_placement_map[index] = rock_noise_val * slope_bonus * height_bonus;
        }
    }
}

void VegetationGenerator::generateResourceDistribution(WorldData& world_data) {
    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            float height = world_data.heightmap_data[index];
            
            if (world_data.is_river_tile[index] || world_data.is_lake_tile[index]) {
                resource_placement_map[index] = 0.0f;
                continue;
            }
            
            float resource_noise_val = Generation::Utils::getCylindricalWrappedNoise(
                resource_noise, static_cast<float>(x), static_cast<float>(y), 
                static_cast<float>(world_data.map_width)
            );
            resource_noise_val = (resource_noise_val + 1.0f) / 2.0f;
            
            // Different resources prefer different elevations
            float geological_factor = 1.0f;
            
            // Gold likes higher elevations (hills and mountains)
            if (height >= GOLD_MIN_HEIGHT) {
                geological_factor *= 1.5f;
            }
            
            // Iron prefers lower to mid elevations
            if (height <= IRON_MAX_HEIGHT) {
                geological_factor *= 1.3f;
            }
            
            resource_placement_map[index] = resource_noise_val * geological_factor;
        }
    }
}

void VegetationGenerator::generateWindPatterns(WorldData& world_data) {
    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            float wind_noise_val = Generation::Utils::getCylindricalWrappedNoise(
                wind_noise, static_cast<float>(x), static_cast<float>(y), 
                static_cast<float>(world_data.map_width)
            );
            
            // Convert noise to wind direction (0-7 for 8 directions)
            int wind_dir = static_cast<int>((wind_noise_val + 1.0f) * 4.0f) % WIND_PATTERN_VARIATIONS;
            wind_direction_map[index] = wind_dir;
        }
    }
}

float VegetationGenerator::getTreeDensity(int x, int y, int map_width) const {
    if (tree_density_map.empty()) return 0.0f;
    size_t index = static_cast<size_t>(y) * map_width + x;
    return (index < tree_density_map.size()) ? tree_density_map[index] : 0.0f;
}

float VegetationGenerator::getBushDensity(int x, int y, int map_width) const {
    if (bush_density_map.empty()) return 0.0f;
    size_t index = static_cast<size_t>(y) * map_width + x;
    return (index < bush_density_map.size()) ? bush_density_map[index] : 0.0f;
}

float VegetationGenerator::getFlowerDensity(int x, int y, int map_width) const {
    if (flower_density_map.empty()) return 0.0f;
    size_t index = static_cast<size_t>(y) * map_width + x;
    return (index < flower_density_map.size()) ? flower_density_map[index] : 0.0f;
}

float VegetationGenerator::getRockPlacement(int x, int y, int map_width) const {
    if (rock_placement_map.empty()) return 0.0f;
    size_t index = static_cast<size_t>(y) * map_width + x;
    return (index < rock_placement_map.size()) ? rock_placement_map[index] : 0.0f;
}

float VegetationGenerator::getResourcePlacement(int x, int y, int map_width) const {
    if (resource_placement_map.empty()) return 0.0f;
    size_t index = static_cast<size_t>(y) * map_width + x;
    return (index < resource_placement_map.size()) ? resource_placement_map[index] : 0.0f;
}

int VegetationGenerator::getWindDirection(int x, int y, int map_width) const {
    if (wind_direction_map.empty()) return 0;
    size_t index = static_cast<size_t>(y) * map_width + x;
    return (index < wind_direction_map.size()) ? wind_direction_map[index] : 0;
}

} // namespace Vegetation
} // namespace Systems
} // namespace World