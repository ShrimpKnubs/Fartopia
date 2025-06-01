// File: EmergentKingdoms/src/World/Systems/Land/LandTileAssigner.cpp
#include "LandTileAssigner.h"
#include "../../Map.h" // ADDED: Include full Map definition for tiles access
#include "../../Tile.h"
#include <iostream>
#include <random>
#include <omp.h>

namespace World {
namespace Systems {
namespace Land {

LandTileAssigner::LandTileAssigner() {
    marsh_water_coverage_chance = MARSH_WATER_COVERAGE_CHANCE;
}

void LandTileAssigner::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    unsigned int land_assigner_seed = base_world_seed + static_cast<unsigned int>(step_seed_offset);
    std::mt19937 rng(land_assigner_seed);
    std::uniform_real_distribution<float> dist_0_1(0.0f, 1.0f);
    
    std::cout << "  Land: Assigning land tile types with shoreline effects..." << std::endl;
    
    // First pass: Handle marsh water patches (needs RNG, so serial)
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            // FIXED: Use getTilesRef() instead of accessing tiles directly
            if (index >= world_data.map_context->getTilesRef().size()) continue;
            
            if (world_data.map_context->getTilesRef()[index].base_type == BaseTileType::MARSH) {
                bool is_marsh_water_patch = (dist_0_1(rng) < marsh_water_coverage_chance);
                world_data.map_context->getTilesRef()[index].is_marsh_water_patch = is_marsh_water_patch;
            }
        }
    }
    
    // Second pass: Create final land tiles (can be parallel)
    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            // FIXED: Use getTilesRef() instead of accessing tiles directly
            if (index >= world_data.map_context->getTilesRef().size()) continue;
            
            BaseTileType current_type = world_data.map_context->getTilesRef()[index].base_type;
            
            // Process only land-type tiles
            if (current_type == BaseTileType::MEADOW ||
                current_type == BaseTileType::PLAINS ||
                current_type == BaseTileType::DRY_PLAINS ||
                current_type == BaseTileType::HILLS ||
                current_type == BaseTileType::MOOR ||
                current_type == BaseTileType::PLATEAU_GRASS ||
                current_type == BaseTileType::MARSH ||
                current_type == BaseTileType::BORDER_WALL ||
                current_type == BaseTileType::VOID) {
                
                World::SlopeAspect aspect = (index < world_data.aspect_map.size()) ? 
                                          world_data.aspect_map[index] : World::SlopeAspect::FLAT;
                
                // Get distance to water (calculated earlier by TileAssigner coordinator)
                // FIXED: Use getTilesRef() instead of accessing tiles directly
                int distance_to_water = world_data.map_context->getTilesRef()[index].distance_to_water;
                
                // Get marsh water patch flag
                // FIXED: Use getTilesRef() instead of accessing tiles directly
                bool is_marsh_water_patch = world_data.map_context->getTilesRef()[index].is_marsh_water_patch;
                
                // FIXED: Use getTilesRef() instead of accessing tiles directly
                world_data.map_context->getTilesRef()[index] = Tile::create(
                    current_type,
                    world_data.heightmap_data[index],
                    world_data.slope_map[index],
                    aspect,
                    -1, // distance_to_land (not applicable for land tiles)
                    distance_to_water, // distance_to_water for shoreline effects
                    0.0f, // animation_offset (land doesn't animate)
                    0.0f, // wave_strand_intensity (not applicable for land)
                    is_marsh_water_patch
                );
            }
        }
    }
    
    std::cout << "  Land: Finished assigning land tiles with shoreline effects." << std::endl;
}

} // namespace Land
} // namespace Systems
} // namespace World