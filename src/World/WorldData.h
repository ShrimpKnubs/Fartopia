// File: EmergentKingdoms/src/World/WorldData.h
#pragma once
#include <vector>
#include <algorithm> // For std::min, std::max
#include "Tile.h"    // For SlopeAspect (World::SlopeAspect)

namespace World {

class Map; 

struct WorldData {
    // Core data structures passed around by reference
    std::vector<float>& heightmap_data;
    std::vector<bool>& is_river_tile;
    std::vector<bool>& is_lake_tile;
    std::vector<float>& slope_map;
    std::vector<SlopeAspect>& aspect_map; 
    std::vector<bool>& lake_has_waves_map; // For conditional lake waves
    
    // Map dimensions (read-only for steps)
    const int map_width;
    const int map_height;

    Map* map_context; 

    WorldData(
        std::vector<float>& hd, std::vector<bool>& irt, std::vector<bool>& ilt,
        std::vector<float>& sm, std::vector<SlopeAspect>& sam,
        std::vector<bool>& lhw_map, // For lake waves
        int mw, int mh, Map* map_ctx
    ) : heightmap_data(hd), is_river_tile(irt), is_lake_tile(ilt),
        slope_map(sm), aspect_map(sam),
        lake_has_waves_map(lhw_map), // Initialize lake waves map
        map_width(mw), map_height(mh), map_context(map_ctx)
    {}

    float getWrappedHeight(int x, int y) const {
        int query_y = std::max(0, std::min(y, map_height - 1));
        int query_x = (x % map_width + map_width) % map_width; 

        size_t index = static_cast<size_t>(query_y) * map_width + query_x;
        if (index < heightmap_data.size()) {
            return heightmap_data[index];
        }
        return 0.0f; 
    }
    
    float getWrappedHeight(const std::vector<float>& specific_heightmap, int x, int y) const {
        int query_y = std::max(0, std::min(y, map_height - 1));
        int query_x = (x % map_width + map_width) % map_width;

        size_t index = static_cast<size_t>(query_y) * map_width + query_x;
        if (index < specific_heightmap.size()) {
            return specific_heightmap[index];
        }
        return 0.0f;
    }
};

} // namespace World