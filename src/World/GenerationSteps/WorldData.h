// File: EmergentKingdoms/src/World/WorldData.h
#pragma once
#include <vector>
#include <algorithm> // For std::min, std::max
#include "Tile.h" // For SlopeAspect

namespace World {

// Forward declare Map to break circular dependency if Map includes WorldData members directly
class Map; 

struct WorldData {
    // Core data structures passed around
    std::vector<float>& heightmap_data;
    std::vector<bool>& is_river_tile;
    std::vector<bool>& is_lake_tile;
    std::vector<float>& slope_map;
    std::vector<SlopeAspect>& aspect_map;
    
    // Map dimensions (read-only for steps)
    const int map_width;
    const int map_height;

    // Contextual data (read-only for steps, mostly)
    // For steps that might need to directly modify Map's final `tiles` or call Map methods
    Map* map_context; 

    WorldData(
        std::vector<float>& hd, std::vector<bool>& irt, std::vector<bool>& ilt,
        std::vector<float>& sm, std::vector<SlopeAspect>& sam,
        int mw, int mh, Map* map_ctx
    ) : heightmap_data(hd), is_river_tile(irt), is_lake_tile(ilt),
        slope_map(sm), aspect_map(sam),
        map_width(mw), map_height(mh), map_context(map_ctx)
    {}

    // Helper to get height from the primary heightmap_data, handling wrapping/clamping
    // This is useful for many generation steps.
    float getWrappedHeight(int x, int y) const {
        // If y is out of bounds, clamp y and wrap x. This effectively mirrors top/bottom for neighbors.
        // More robust would be to decide if this should return a boundary value or error.
        // For erosion, often clamping y is acceptable.
        int query_y = std::max(0, std::min(y, map_height - 1));
        int query_x = (x % map_width + map_width) % map_width; // Ensure positive modulo

        size_t index = static_cast<size_t>(query_y) * map_width + query_x;
        if (index < heightmap_data.size()) {
            return heightmap_data[index];
        }
        return 0.0f; // Default if out of bounds after clamping (shouldn't happen with correct logic)
    }
    
    // Overload for accessing a specific heightmap (e.g. a temporary one)
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