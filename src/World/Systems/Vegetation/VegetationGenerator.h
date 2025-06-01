// File: EmergentKingdoms/src/World/Systems/Vegetation/VegetationGenerator.h
#pragma once
#include "../../WorldData.h"
#include "../../../Core/FastNoiseLite.h"
#include <vector>

namespace World {
namespace Systems {
namespace Vegetation {

/**
 * Generates natural vegetation distribution patterns using noise-based algorithms
 * Creates realistic clustering and spacing for medieval fantasy landscapes
 */
class VegetationGenerator {
public:
    VegetationGenerator();
    
    // Main generation method - creates all vegetation maps
    void generateVegetationMaps(WorldData& world_data, unsigned int base_seed);
    
    // Query methods for vegetation density at specific coordinates
    float getTreeDensity(int x, int y, int map_width) const;
    float getBushDensity(int x, int y, int map_width) const;
    float getFlowerDensity(int x, int y, int map_width) const;
    float getRockPlacement(int x, int y, int map_width) const;
    float getResourcePlacement(int x, int y, int map_width) const;
    int getWindDirection(int x, int y, int map_width) const;

private:
    // Noise generators for different vegetation types
    FastNoiseLite tree_noise;
    FastNoiseLite bush_noise;
    FastNoiseLite flower_noise;
    FastNoiseLite rock_noise;
    FastNoiseLite resource_noise;
    FastNoiseLite wind_noise;
    
    // Generated density maps
    std::vector<float> tree_density_map;
    std::vector<float> bush_density_map;
    std::vector<float> flower_density_map;
    std::vector<float> rock_placement_map;
    std::vector<float> resource_placement_map;
    std::vector<int> wind_direction_map;
    
    // Setup methods
    void setupNoiseGenerators(unsigned int base_seed);
    
    // Individual generation methods
    void generateTreeDistribution(WorldData& world_data);
    void generateBushDistribution(WorldData& world_data);
    void generateFlowerDistribution(WorldData& world_data);
    void generateRockDistribution(WorldData& world_data);
    void generateResourceDistribution(WorldData& world_data);
    void generateWindPatterns(WorldData& world_data);
};

} // namespace Vegetation
} // namespace Systems
} // namespace World