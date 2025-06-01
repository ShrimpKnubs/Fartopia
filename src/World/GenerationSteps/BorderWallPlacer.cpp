// File: EmergentKingdoms/src/World/GenerationSteps/BorderWallPlacer.cpp
#include "BorderWallPlacer.h"
#include "../Tile.h" // For Tile::createSpecial and BaseTileType
#include <iostream>
#include <omp.h>

namespace World {
namespace Generation {

void BorderWallPlacer::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    (void)base_world_seed; // Not used
    (void)step_seed_offset; // Not used
    std::cout << "  Setting top and bottom border walls..." << std::endl;

    #pragma omp parallel for
    for (int x_border = 0; x_border < world_data.map_width; ++x_border) {
        // Top border - FIXED: Use getTilesRef() instead of direct tiles access
        world_data.map_context->getTilesRef()[static_cast<size_t>(0) * world_data.map_width + x_border] = Tile::createSpecial(BaseTileType::BORDER_WALL);
        
        // Bottom border - FIXED: Use getTilesRef() instead of direct tiles access
        if (world_data.map_height > 1) {
            world_data.map_context->getTilesRef()[static_cast<size_t>(world_data.map_height - 1) * world_data.map_width + x_border] = Tile::createSpecial(BaseTileType::BORDER_WALL);
        }
    }
}

} // namespace Generation
} // namespace World