// File: EmergentKingdoms/src/World/Systems/Mountains/MountainTileRenderer.h
#pragma once
#include "../ITileRenderer.h"
#include "MountainColors.h"
#include "MountainConfig.h"
#include "../../../Core/BaseConfig.h"

namespace World {
namespace Systems {
namespace Mountains {

class MountainTileRenderer : public ITileRenderer {
public:
    bool canRender(BaseTileType tile_type) const override {
        return tile_type == BaseTileType::MOUNTAIN_LOWER ||
               tile_type == BaseTileType::MOUNTAIN_MID ||
               tile_type == BaseTileType::MOUNTAIN_UPPER ||
               tile_type == BaseTileType::MOUNTAIN_PEAK_SNOW ||
               tile_type == BaseTileType::CLIFF_FACE ||
               tile_type == BaseTileType::STEEP_SLOPE ||
               tile_type == BaseTileType::ROCKY_SLOPE;
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
        
        (void)dist_to_land; // Unused
        (void)global_water_animation_progress; // Unused
        (void)tile_wave_strand_intensity; // FIXED: Added void cast to suppress warning
        (void)is_marsh_water_patch_flag; // Unused
        (void)tile_dist_to_water; // Unused

        unsigned int tile_hash = static_cast<unsigned int>(height_val * 1000 + slope_val * 100 + 
                                                          static_cast<int>(aspect_val) + tile_anim_offset * 100);

        switch (base_type) {
            case BaseTileType::MOUNTAIN_LOWER:
            case BaseTileType::MOUNTAIN_MID:
            case BaseTileType::MOUNTAIN_UPPER:
                return renderMountainTile(base_type, height_val, slope_val, tile_hash);
                
            case BaseTileType::MOUNTAIN_PEAK_SNOW:
                return renderSnowPeak(height_val, slope_val, tile_hash);
                
            case BaseTileType::CLIFF_FACE:
                return renderCliffFace(aspect_val, tile_hash);
                
            case BaseTileType::STEEP_SLOPE:
                return renderSteepSlope(height_val, slope_val, aspect_val, tile_hash);
                
            case BaseTileType::ROCKY_SLOPE:
                return renderRockySlope(height_val, slope_val, aspect_val, tile_hash);
                
            default:
                return {' ', Core::Colors::WHITE, Core::Colors::BLACK};
        }
    }

    std::string getSystemName() const override {
        return "Mountains";
    }

private:
    Core::ScreenCell renderMountainTile(BaseTileType base_type, float height_val, 
                                       float slope_val, unsigned int tile_hash) const {
        char ch = (tile_hash % 3 == 0) ? '^' : 'M'; 
        if (slope_val > Core::SLOPE_THRESHOLD_STEEP * 1.2f) ch = '^'; 
        
        float t_rock; 
        if (base_type == BaseTileType::MOUNTAIN_LOWER) {
            t_rock = (height_val - Core::TERRAIN_MOUNTAIN_BASE) / (Core::TERRAIN_MOUNTAIN_MID - Core::TERRAIN_MOUNTAIN_BASE); 
        } else if (base_type == BaseTileType::MOUNTAIN_MID) {
            t_rock = (height_val - Core::TERRAIN_MOUNTAIN_MID) / (Core::TERRAIN_MOUNTAIN_HIGH - Core::TERRAIN_MOUNTAIN_MID); 
        } else {
            t_rock = (height_val - Core::TERRAIN_MOUNTAIN_HIGH) / (Core::TERRAIN_MOUNTAIN_PEAK_ZONE - Core::TERRAIN_MOUNTAIN_HIGH); 
        }
        t_rock = std::max(0.0f, std::min(1.0f, t_rock)); 
        
        sf::Color bg = Tile::interpolateColor(Colors::ROCK_DARK_BASE, Colors::ROCK_PEAK, t_rock); 
        sf::Color fg = Tile::interpolateColor(Colors::ROCK_MID_SLOPE, Colors::SNOW_SHADOW, t_rock * 0.8f); 
        
        // Add some vegetation to lower mountains
        if (base_type == BaseTileType::MOUNTAIN_LOWER && slope_val < Core::SLOPE_THRESHOLD_MODERATE && tile_hash % 5 == 0) {
            bg = Tile::interpolateColor(Core::LandColors::GRASS_MID_SLOPE, bg, 0.6f);
            fg = Core::LandColors::GRASS_LIGHT_HILLTOP;
            ch = (tile_hash % 2 == 0) ? 'n' : ';';
        }
        
        if (slope_val > Core::SLOPE_THRESHOLD_VERY_STEEP && height_val > Core::TERRAIN_MOUNTAIN_MID) ch = 'A'; 
        
        return {ch, fg, bg};
    }

    Core::ScreenCell renderSnowPeak(float height_val, float slope_val, unsigned int tile_hash) const {
        char ch = (tile_hash % 2 == 0) ? '*' : 'o'; 
        if (slope_val > Core::SLOPE_THRESHOLD_STEEP) ch = '^'; 
        
        sf::Color bg = Tile::interpolateColor(Colors::SNOW_SHADOW, Colors::SNOW_BRIGHT, 
                                             (height_val - SNOWLINE_MIN_HEIGHT) / (1.0f - SNOWLINE_MIN_HEIGHT)); 
        sf::Color fg = Colors::ROCK_PEAK; 
        if (slope_val < Core::SLOPE_THRESHOLD_GENTLE) fg = Core::Colors::WHITE; 
        
        return {ch, fg, bg};
    }

    Core::ScreenCell renderCliffFace(SlopeAspect aspect_val, unsigned int tile_hash) const {
        char ch = '#'; 
        sf::Color bg = Colors::ROCK_DARK_BASE; 
        sf::Color fg = Colors::ROCK_MID_SLOPE; 
        
        if (aspect_val == SlopeAspect::SOUTH || aspect_val == SlopeAspect::SOUTHEAST || 
            aspect_val == SlopeAspect::SOUTHWEST || aspect_val == SlopeAspect::EAST || 
            aspect_val == SlopeAspect::WEST) { 
            bg = Colors::ROCK_MID_SLOPE; 
            fg = Colors::ROCK_LIGHT_EXPOSED; 
        } 
        
        switch (aspect_val) { 
            case SlopeAspect::NORTH: 
            case SlopeAspect::NORTHWEST: 
                ch = '|'; fg = Colors::ROCK_DARK_BASE; bg = Colors::ROCK_MID_SLOPE; break; 
            case SlopeAspect::NORTHEAST: 
                ch = '|'; fg = Colors::ROCK_DARK_BASE; bg = Colors::ROCK_MID_SLOPE; break; 
            case SlopeAspect::SOUTH: 
            case SlopeAspect::SOUTHWEST: 
                ch = '|'; break; 
            case SlopeAspect::SOUTHEAST: 
                ch = '|'; break; 
            case SlopeAspect::EAST: 
                ch = '['; break; 
            case SlopeAspect::WEST: 
                ch = ']'; break; 
            default: 
                ch = (tile_hash % 2 == 0) ? '#' : 'H'; break; 
        } 
        
        return {ch, fg, bg};
    }

    Core::ScreenCell renderSteepSlope(float height_val, float slope_val, SlopeAspect aspect_val, 
                                     unsigned int tile_hash) const {
        char ch = (tile_hash % 3 == 0) ? '%' : ':'; 
        
        float t_steep_range = Core::TERRAIN_MOUNTAIN_BASE - Core::TERRAIN_STEEP_SLOPES;
        if (t_steep_range < 0.001f) t_steep_range = 0.001f;
        float t = (height_val - Core::TERRAIN_STEEP_SLOPES) / t_steep_range; 
        t = std::max(0.0f, std::min(1.0f, t)); 
        
        sf::Color bg, fg;
        if (height_val < Core::TERRAIN_STEEP_SLOPES + (t_steep_range) * 0.5f) { 
            bg = Tile::interpolateColor(Core::LandColors::GRASS_MID_SLOPE, Colors::ROCK_DARK_BASE, t * 0.7f + 0.1f); 
            fg = Tile::interpolateColor(Core::LandColors::GRASS_LIGHT_HILLTOP, Colors::ROCK_MID_SLOPE, t * 0.6f + 0.1f); 
        } else { 
            bg = Tile::interpolateColor(Colors::ROCK_DARK_BASE, Colors::ROCK_MID_SLOPE, t); 
            fg = Tile::interpolateColor(Colors::ROCK_MID_SLOPE, Colors::ROCK_LIGHT_EXPOSED, t); 
        } 
        
        if (slope_val > Core::SLOPE_THRESHOLD_STEEP * 0.8f) { 
            switch (aspect_val) { 
                case SlopeAspect::NORTH: case SlopeAspect::NORTHWEST: ch = '/'; break; 
                case SlopeAspect::NORTHEAST: ch = '\\'; break; 
                case SlopeAspect::SOUTH: case SlopeAspect::SOUTHWEST: ch = '\\'; break; 
                case SlopeAspect::SOUTHEAST: ch = '/'; break; 
                case SlopeAspect::EAST: ch = '<'; break; 
                case SlopeAspect::WEST: ch = '>'; break; 
                default: ch = '#'; break; 
            } 
        } 
        
        return {ch, fg, bg};
    }

    Core::ScreenCell renderRockySlope(float height_val, float slope_val, SlopeAspect aspect_val, 
                                     unsigned int tile_hash) const {
        char ch = (tile_hash % 3 == 0) ? '%' : ':';
        
        float t_rocky_range = Core::TERRAIN_MOUNTAIN_BASE - Core::TERRAIN_STEEP_SLOPES;
        if (t_rocky_range < 0.001f) t_rocky_range = 0.001f;
        float t = (height_val - Core::TERRAIN_STEEP_SLOPES) / t_rocky_range;
        t = std::max(0.0f, std::min(1.0f, t));
        
        sf::Color bg = Tile::interpolateColor(Colors::ROCK_DARK_BASE, Colors::ROCK_MID_SLOPE, t * 0.8f + 0.1f); 
        sf::Color fg = Tile::interpolateColor(bg, Colors::ROCK_LIGHT_EXPOSED, t * 0.7f + 0.2f);
        
        if (slope_val > Core::SLOPE_THRESHOLD_STEEP * 0.5f) {
            switch (aspect_val) {
                case SlopeAspect::NORTH: case SlopeAspect::NORTHWEST: ch = '/'; break;
                case SlopeAspect::NORTHEAST: ch = '\\'; break;
                case SlopeAspect::SOUTH: case SlopeAspect::SOUTHWEST: ch = '\\'; break;
                case SlopeAspect::SOUTHEAST: ch = '/'; break;
                default: ch = '#'; break;
            }
        }
        
        return {ch, fg, bg};
    }
};

} // namespace Mountains
} // namespace Systems
} // namespace World