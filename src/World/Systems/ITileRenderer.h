// File: EmergentKingdoms/src/World/Systems/ITileRenderer.h
#pragma once
#include "../Tile.h"
#include "../../Core/Renderer.h"

namespace World {
namespace Systems {

/**
 * Interface for system-specific tile renderers
 * Each terrain system (Rivers, Lakes, Mountains, Land, Vegetation) implements this
 */
class ITileRenderer {
public:
    virtual ~ITileRenderer() = default;
    
    /**
     * Check if this renderer can handle the given tile type
     */
    virtual bool canRender(BaseTileType tile_type) const = 0;
    
    /**
     * Render the tile and return the display cell
     */
    virtual Core::ScreenCell render(
        BaseTileType base_type, 
        float height_val, 
        float slope_val, 
        SlopeAspect aspect_val, 
        int dist_to_land, 
        float global_water_animation_progress, 
        float tile_anim_offset, 
        float tile_wave_strand_intensity,
        bool is_marsh_water_patch_flag, 
        int tile_dist_to_water
    ) const = 0;
    
    /**
     * Get the name of this rendering system for debugging
     */
    virtual std::string getSystemName() const = 0;
};

} // namespace Systems
} // namespace World