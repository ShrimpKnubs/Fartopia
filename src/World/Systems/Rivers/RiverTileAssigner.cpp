// File: EmergentKingdoms/src/World/Systems/Rivers/RiverTileAssigner.cpp
#include "RiverTileAssigner.h"
#include "../../Map.h"
#include "../../Tile.h"
#include <iostream>
#include <random>
#include <algorithm>

namespace World {
namespace Systems {
namespace Rivers {

RiverTileAssigner::RiverTileAssigner() {
    terrain_river_bed_height = TERRAIN_RIVER_BED;
}

void RiverTileAssigner::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    unsigned int river_assigner_seed = base_world_seed + static_cast<unsigned int>(step_seed_offset);
    std::mt19937 rng(river_assigner_seed);
    std::uniform_real_distribution<float> dist_0_1(0.0f, 1.0f);
    
    std::cout << "  Rivers: Assigning river tile types and adjusting heights..." << std::endl;
    
    // Process only river tiles
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            if (world_data.is_river_tile[index]) {
                // Assign river water tile type
                float h = world_data.heightmap_data[index];
                
                // Adjust river height
                float r_h_adjust = dist_0_1(rng) / 200.0f;
                world_data.heightmap_data[index] = std::min(h, terrain_river_bed_height + 0.01f + r_h_adjust);
                
                // Create the river tile
                World::SlopeAspect aspect = (index < world_data.aspect_map.size()) ? 
                                          world_data.aspect_map[index] : World::SlopeAspect::FLAT;
                
                // FIXED: Use getTilesRef() instead of accessing tiles directly
                world_data.map_context->getTilesRef()[index] = Tile::create(
                    BaseTileType::RIVER_WATER,
                    world_data.heightmap_data[index],
                    world_data.slope_map[index],
                    aspect,
                    -1, // distance_to_land (not applicable for rivers)
                    -1, // distance_to_water (not applicable for rivers)
                    0.0f, // animation_offset (rivers don't animate like lakes)
                    0.0f, // wave_strand_intensity (not applicable for rivers)
                    false // is_marsh_water_patch
                );
            }
        }
    }
    
    std::cout << "  Rivers: Finished assigning river tiles." << std::endl;
}

} // namespace Rivers
} // namespace Systems
} // namespace World