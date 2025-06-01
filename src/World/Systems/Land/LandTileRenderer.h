// File: EmergentKingdoms/src/World/Systems/Land/LandTileRenderer.h
#pragma once
#include "../ITileRenderer.h"
#include "LandColors.h"
#include "LandConfig.h"
#include "../../../Core/BaseConfig.h"
#include "../../GenerationSteps/WorldGenUtils.h"

namespace World {
namespace Systems {
namespace Land {

class LandTileRenderer : public ITileRenderer {
public:
    bool canRender(BaseTileType tile_type) const override {
        return tile_type == BaseTileType::MEADOW ||
               tile_type == BaseTileType::PLAINS ||
               tile_type == BaseTileType::DRY_PLAINS ||
               tile_type == BaseTileType::HILLS ||
               tile_type == BaseTileType::MOOR ||
               tile_type == BaseTileType::PLATEAU_GRASS ||
               tile_type == BaseTileType::MARSH ||
               tile_type == BaseTileType::BORDER_WALL ||
               tile_type == BaseTileType::VOID;
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
        
        (void)aspect_val; // Unused for most land types
        (void)dist_to_land; // Unused for land
        (void)global_water_animation_progress; // Unused for land
        (void)tile_wave_strand_intensity; // Unused for land

        unsigned int tile_hash = static_cast<unsigned int>(height_val * 1000 + slope_val * 100 + 
                                                          static_cast<int>(aspect_val) + tile_anim_offset * 100 + tile_dist_to_water * 10);

        switch (base_type) {
            case BaseTileType::MEADOW:
                return renderMeadow(tile_hash, tile_dist_to_water);
                
            case BaseTileType::PLAINS:
                return renderPlains(height_val, tile_hash, tile_dist_to_water);
                
            case BaseTileType::DRY_PLAINS:
                return renderDryPlains(tile_hash, tile_dist_to_water);
                
            case BaseTileType::HILLS:
                return renderHills(height_val, slope_val, tile_hash, tile_dist_to_water);
                
            case BaseTileType::MOOR:
                return renderMoor(height_val, slope_val, tile_hash);
                
            case BaseTileType::PLATEAU_GRASS:
                return renderPlateau(tile_hash);
                
            case BaseTileType::MARSH:
                return renderMarsh(is_marsh_water_patch_flag, tile_hash);
                
            case BaseTileType::BORDER_WALL:
                return {'X', Core::Colors::BORDER_COLOR_FG, Core::Colors::BORDER_COLOR_BG};
                
            case BaseTileType::VOID:
            default:
                return {'?', Core::Colors::RED, Core::Colors::BLACK};
        }
    }

    std::string getSystemName() const override {
        return "Land";
    }

private:
    Core::ScreenCell renderMeadow(unsigned int tile_hash, int tile_dist_to_water) const {
        char ch;
        sf::Color fg, bg;
        
        if (tile_dist_to_water >= 0 && tile_dist_to_water < SHORELINE_MAX_DISTANCE) {
            bool is_transition_noise = (tile_hash % 10 == 0); 
            if (!is_transition_noise) {
                if (tile_dist_to_water <= 1) { 
                    bg = Colors::SHORE_WET_DIRT_BG; fg = Colors::SHORE_WET_DIRT_FG; 
                    ch = (tile_hash % 2 == 0) ? '.' : ',';
                } else if (tile_dist_to_water <= 3) { 
                    bg = Colors::SHORE_DAMP_DIRT_BG; fg = Colors::SHORE_DAMP_DIRT_FG; 
                    ch = (tile_hash % 2 == 0) ? ';' : ':';
                } else { 
                    bg = Colors::SHORE_DRY_DIRT_BG; fg = Colors::SHORE_DRY_DIRT_FG; 
                    ch = (tile_hash % 2 == 0) ? '\'' : '.';
                }
                return {ch, fg, bg}; 
            }
        }
        
        ch = (tile_hash % 5 == 0) ? '\'' : '.';
        bg = Core::LandColors::GRASS_DARK_VALLEY;
        fg = Core::LandColors::PLAINS_GRASS_HIGHLIGHT;
        
        return {ch, fg, bg};
    }

    Core::ScreenCell renderPlains(float height_val, unsigned int tile_hash, int tile_dist_to_water) const {
        char ch;
        sf::Color fg, bg;
        
        if (tile_dist_to_water >= 0 && tile_dist_to_water < SHORELINE_MAX_DISTANCE) {
            bool is_transition_noise = (tile_hash % 10 == 0); 
            if (!is_transition_noise) {
                if (tile_dist_to_water <= 1) { 
                    bg = Colors::SHORE_WET_DIRT_BG; fg = Colors::SHORE_WET_DIRT_FG; 
                    ch = (tile_hash % 2 == 0) ? '.' : ',';
                } else if (tile_dist_to_water <= 3) { 
                    bg = Colors::SHORE_DAMP_DIRT_BG; fg = Colors::SHORE_DAMP_DIRT_FG; 
                    ch = (tile_hash % 2 == 0) ? ';' : ':';
                } else { 
                    bg = Colors::SHORE_DRY_DIRT_BG; fg = Colors::SHORE_DRY_DIRT_FG; 
                    ch = (tile_hash % 2 == 0) ? ',' : '.';
                }
                return {ch, fg, bg}; 
            }
        }
        
        ch = (tile_hash % 4 == 0) ? ',' : '.'; 
        
        float t_plains_range = Core::TERRAIN_PLAINS_HIGH - Core::TERRAIN_PLAINS_LOW;
        if (t_plains_range < 0.001f) t_plains_range = 0.001f;
        float t = (height_val - Core::TERRAIN_PLAINS_LOW) / t_plains_range; 
        t = std::max(0.0f, std::min(1.0f, t)); 
        
        bg = Tile::interpolateColor(Core::LandColors::PLAINS_GRASS_BASE, Core::LandColors::GRASS_MID_SLOPE, t); 
        fg = Tile::interpolateColor(bg, Core::LandColors::PLAINS_GRASS_HIGHLIGHT, 0.3f + t * 0.1f); 
        
        if (tile_hash % 20 == 0) { 
            bg = Tile::interpolateColor(Core::LandColors::EARTH_MID, bg, 0.6f);
            fg = Tile::interpolateColor(bg, Core::LandColors::EARTH_LIGHT, 0.3f);
            ch = (tile_hash % 2 == 0) ? ':' : ';';
        }
        
        return {ch, fg, bg};
    }

    Core::ScreenCell renderDryPlains(unsigned int tile_hash, int tile_dist_to_water) const {
        char ch;
        sf::Color fg, bg;
        
        if (tile_dist_to_water >= 0 && tile_dist_to_water < SHORELINE_MAX_DISTANCE) {
            bool is_transition_noise = (tile_hash % 10 == 0);
            if (!is_transition_noise) { 
                if (tile_dist_to_water <= 1) { 
                    bg = Colors::SHORE_WET_DIRT_BG; 
                    fg = Tile::interpolateColor(bg, Core::LandColors::DRY_GRASS_FG, 0.4f); 
                    ch = (tile_hash % 2 == 0) ? '.' : ':';
                } else if (tile_dist_to_water <= 3) { 
                    bg = Colors::SHORE_DAMP_DIRT_BG; 
                    fg = Tile::interpolateColor(bg, Core::LandColors::DRY_GRASS_FG, 0.6f); 
                    ch = (tile_hash % 2 == 0) ? ';' : ',';
                } else { 
                    bg = Colors::SHORE_DRY_DIRT_BG; 
                    fg = Core::LandColors::DRY_GRASS_FG; 
                    ch = (tile_hash % 2 == 0) ? '"' : ':';
                }
                return {ch, fg, bg};
            }
        }
        
        ch = (tile_hash % 4 == 0) ? '"' : ':';
        bg = Core::LandColors::DRY_GRASS_BG;
        fg = Core::LandColors::DRY_GRASS_FG;
        
        if (tile_hash % 15 == 0) { 
            bg = Tile::interpolateColor(Core::LandColors::PLAINS_GRASS_BASE, bg, 0.4f);
        }
        
        return {ch, fg, bg};
    }

    Core::ScreenCell renderHills(float height_val, float slope_val, unsigned int tile_hash, 
                                int tile_dist_to_water) const {
        char ch;
        sf::Color fg, bg;
        
        if (tile_dist_to_water >= 0 && tile_dist_to_water < SHORELINE_MAX_DISTANCE) {
            bool is_transition_noise = (tile_hash % 10 == 0); 
            if (!is_transition_noise) { 
                float rock_mix = Generation::Utils::clamp_val(slope_val * 10.0f, 0.1f, 0.5f);
                if (tile_dist_to_water <= 1) { 
                    bg = Tile::interpolateColor(Colors::SHORE_WET_DIRT_BG, sf::Color(80, 75, 70), rock_mix); 
                    fg = Tile::interpolateColor(Colors::SHORE_WET_DIRT_FG, sf::Color(110, 105, 100), rock_mix);
                    ch = (tile_hash % 2 == 0) ? '.' : ';';
                } else if (tile_dist_to_water <= 3) { 
                    bg = Tile::interpolateColor(Colors::SHORE_DAMP_DIRT_BG, sf::Color(110, 105, 100), rock_mix * 0.7f); 
                    fg = Tile::interpolateColor(Colors::SHORE_DAMP_DIRT_FG, sf::Color(140, 135, 130), rock_mix * 0.7f);
                    ch = (tile_hash % 2 == 0) ? ':' : ',';
                } else { 
                    bg = Tile::interpolateColor(Colors::SHORE_DRY_DIRT_BG, Core::LandColors::GRASS_MID_SLOPE, 0.3f); 
                    fg = Tile::interpolateColor(Colors::SHORE_DRY_DIRT_FG, Core::LandColors::EARTH_LIGHT, 0.3f);
                    ch = (tile_hash % 2 == 0) ? '"' : ';';
                }
                return {ch, fg, bg}; 
            }
        }
        
        ch = (tile_hash % 4 == 0) ? '"' : ';'; 
        if (slope_val > Core::SLOPE_THRESHOLD_MODERATE) ch = (tile_hash % 2 == 0) ? 'n' : 'u'; 
        
        float t_hills_range = Core::TERRAIN_ROLLING_HILLS_HIGH - Core::TERRAIN_ROLLING_HILLS_LOW;
        if (t_hills_range < 0.001f) t_hills_range = 0.001f;
        float t = (height_val - Core::TERRAIN_ROLLING_HILLS_LOW) / t_hills_range; 
        t = std::max(0.0f, std::min(1.0f, t)); 
        t = std::pow(t, 0.8f); 
        
        bg = Tile::interpolateColor(Core::LandColors::GRASS_MID_SLOPE, Core::LandColors::GRASS_LIGHT_HILLTOP, t); 
        float rockiness = Generation::Utils::clamp_val(slope_val * 15.0f, 0.0f, 0.4f); 
        bg = Tile::interpolateColor(bg, sf::Color(110, 105, 100), rockiness);
        fg = Tile::interpolateColor(bg, Core::LandColors::EARTH_LIGHT, 0.15f + t * 0.1f); 
        
        if (slope_val > Core::SLOPE_THRESHOLD_MODERATE * 0.6f && tile_hash % 3 == 0) { 
             fg = Tile::interpolateColor(bg, sf::Color(140, 135, 130), 0.3f + slope_val * 2.0f);
             ch = (tile_hash % 2 == 0) ? '%' : '^';
        }
        
        return {ch, fg, bg};
    }

    Core::ScreenCell renderMoor(float height_val, float slope_val, unsigned int tile_hash) const {
        char ch = (tile_hash % 3 == 0) ? ':' : ';';
        
        float t_moor_range = MOOR_MAX_HEIGHT - MOOR_MIN_HEIGHT;
        if (t_moor_range < 0.001f) t_moor_range = 0.001f;
        float t = (height_val - MOOR_MIN_HEIGHT) / t_moor_range;
        t = std::max(0.0f, std::min(1.0f, t));
        
        sf::Color bg = Tile::interpolateColor(Colors::MOOR_HEATH_BG_DARK, Colors::MOOR_HEATH_BG_LIGHT, t);
        sf::Color fg = Colors::MOOR_GRASS_PATCH_FG;
        
        if (tile_hash % 7 == 0) ch = 'i'; 
        if (slope_val > Core::SLOPE_THRESHOLD_GENTLE && tile_hash % 4 == 0) {
            bg = Tile::interpolateColor(bg, sf::Color(80, 75, 70), 0.2f);
            ch = (tile_hash % 2 == 0) ? 'o' : '.'; 
        }
        
        return {ch, fg, bg};
    }

    Core::ScreenCell renderPlateau(unsigned int tile_hash) const {
        char ch = (tile_hash % 2 == 0) ? '_' : '-';
        sf::Color bg = Colors::PLATEAU_GRASS_BG;
        sf::Color fg = Tile::interpolateColor(bg, Core::LandColors::GRASS_LIGHT_HILLTOP, 0.3f);
        
        if (tile_hash % 8 == 0) { 
            fg = Colors::PLATEAU_ROCK_FG;
            ch = (tile_hash % 3 == 0) ? 'O' : 'o'; 
        }
        
        return {ch, fg, bg};
    }

    Core::ScreenCell renderMarsh(bool is_marsh_water_patch_flag, unsigned int tile_hash) const {
        char ch;
        sf::Color fg, bg;
        
        if (is_marsh_water_patch_flag) {
            ch = (tile_hash % 3 == 0) ? ',' : '.';
            bg = Colors::MARSH_WATER_PATCH_BG;
            fg = Tile::interpolateColor(bg, sf::Color(70, 110, 170), 0.3f);
        } else {
            ch = (tile_hash % 5 == 0) ? '|' : 
                 (tile_hash % 5 == 1) ? 'i' : 
                 (tile_hash % 5 == 2) ? ';' : '"';
            if (tile_hash % 8 < 2) { 
                bg = Colors::WHEAT_MARSH_BG;
                fg = Colors::WHEAT_MARSH_FG;
            } else {
                bg = Colors::MARSH_GRASS_BG;
                fg = Colors::MARSH_REED_FG;
            }
        }
        
        return {ch, fg, bg};
    }
};

} // namespace Land
} // namespace Systems
} // namespace World