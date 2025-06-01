// File: EmergentKingdoms/src/World/Systems/Mountains/MountainTileAssigner.cpp
#include "MountainTileAssigner.h"
#include "../../Map.h"
#include "../../Tile.h"
#include "../../../Core/BaseConfig.h"
#include <iostream>
#include <random>
#include <omp.h>

namespace World {
namespace Systems {
namespace Mountains {

MountainTileAssigner::MountainTileAssigner() {
    snowline_min_height = SNOWLINE_MIN_HEIGHT;
}

void MountainTileAssigner::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    (void)base_world_seed; // Not used for mountain assignment
    (void)step_seed_offset; // Not used for mountain assignment
    
    std::cout << "  Mountains: Assigning mountain tile types..." << std::endl;
    
    // Process only mountain and rocky tiles
    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            // Check if this is a mountain-type tile that we should handle
            // FIXED: Use getTilesRef() instead of accessing tiles directly
            if (index >= world_data.map_context->getTilesRef().size()) continue;
            
            BaseTileType current_type = world_data.map_context->getTilesRef()[index].base_type;
            
            if (current_type == BaseTileType::MOUNTAIN_LOWER ||
                current_type == BaseTileType::MOUNTAIN_MID ||
                current_type == BaseTileType::MOUNTAIN_UPPER ||
                current_type == BaseTileType::MOUNTAIN_PEAK_SNOW ||
                current_type == BaseTileType::CLIFF_FACE ||
                current_type == BaseTileType::STEEP_SLOPE ||
                current_type == BaseTileType::ROCKY_SLOPE) {
                
                World::SlopeAspect aspect = (index < world_data.aspect_map.size()) ? 
                                          world_data.aspect_map[index] : World::SlopeAspect::FLAT;
                
                // FIXED: Use getTilesRef() instead of accessing tiles directly
                world_data.map_context->getTilesRef()[index] = Tile::create(
                    current_type,
                    world_data.heightmap_data[index],
                    world_data.slope_map[index],
                    aspect,
                    -1, // distance_to_land (not applicable for mountains)
                    -1, // distance_to_water (not applicable for mountains)
                    0.0f, // animation_offset (mountains don't animate)
                    0.0f, // wave_strand_intensity (not applicable for mountains)
                    false // is_marsh_water_patch
                );
            }
        }
    }
    
    std::cout << "  Mountains: Finished assigning mountain tiles." << std::endl;
}

} // namespace Mountains
} // namespace Systems
} // namespace World