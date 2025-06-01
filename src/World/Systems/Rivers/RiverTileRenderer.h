// File: EmergentKingdoms/src/World/Systems/Rivers/RiverTileRenderer.h
#pragma once
#include "../ITileRenderer.h"
#include "RiverColors.h"
#include "RiverConfig.h"

namespace World {
namespace Systems {
namespace Rivers {

class RiverTileRenderer : public ITileRenderer {
public:
    bool canRender(BaseTileType tile_type) const override {
        return tile_type == BaseTileType::RIVER_WATER;
    }

    Core::ScreenCell render(
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
    ) const override {
        
        (void)slope_val; // Unused
        (void)aspect_val; // Unused  
        (void)dist_to_land; // Unused
        (void)global_water_animation_progress; // Unused
        (void)tile_anim_offset; // Unused
        (void)tile_wave_strand_intensity; // Unused
        (void)is_marsh_water_patch_flag; // Unused
        (void)tile_dist_to_water; // Unused

        if (base_type != BaseTileType::RIVER_WATER) {
            return {' ', Core::Colors::WHITE, Core::Colors::BLACK};
        }

        char ch = Colors::DEEP_WATER_CHAR;
        sf::Color fg, bg;
        
        // River color based on height (depth)
        float t = height_val / Core::TERRAIN_PLAINS_LOW; 
        t = std::max(0.0f, std::min(1.0f, t)); 
        bg = Tile::interpolateColor(Colors::WATER_DEEP_RIVER, Colors::WATER_SHALLOW_RIVER, t);
        fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.3f); 

        return {ch, fg, bg};
    }

    std::string getSystemName() const override {
        return "Rivers";
    }
};

} // namespace Rivers
} // namespace Systems
} // namespace World