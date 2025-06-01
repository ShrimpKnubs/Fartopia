// File: EmergentKingdoms/src/World/Systems/Lakes/LakeTileRenderer.h
#pragma once
#include "../ITileRenderer.h"
#include "LakeColors.h"
#include "LakeConfig.h"
#include <cmath>

namespace World {
namespace Systems {
namespace Lakes {

class LakeTileRenderer : public ITileRenderer {
public:
    bool canRender(BaseTileType tile_type) const override {
        return tile_type == BaseTileType::LAKE_WATER || tile_type == BaseTileType::POND_WATER;
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
        (void)is_marsh_water_patch_flag; // Unused
        (void)tile_dist_to_water; // Unused

        if (base_type == BaseTileType::POND_WATER) {
            return renderPond(height_val, global_water_animation_progress, tile_anim_offset);
        } else if (base_type == BaseTileType::LAKE_WATER) {
            return renderLake(height_val, dist_to_land, global_water_animation_progress, 
                            tile_anim_offset, tile_wave_strand_intensity);
        }

        return {' ', Core::Colors::WHITE, Core::Colors::BLACK};
    }

    std::string getSystemName() const override {
        return "Lakes";
    }

private:
    Core::ScreenCell renderPond(float height_val, float global_water_animation_progress, 
                               float tile_anim_offset) const {
        (void)height_val; // Unused for now
        
        char ch = Colors::SHORE_WATER_CHAR; 
        sf::Color bg = Colors::POND_WATER_BG;
        sf::Color fg = Colors::POND_WATER_FG;
        
        if (std::fmod(global_water_animation_progress + tile_anim_offset, 1.0f) > 0.5f) {
            unsigned int hash = static_cast<unsigned int>(tile_anim_offset * 1000);
            ch = (hash % 2 == 0) ? '\'' : '`';
        }
        
        return {ch, fg, bg};
    }

    Core::ScreenCell renderLake(float height_val, int dist_to_land, 
                               float global_water_animation_progress,
                               float tile_anim_offset, float tile_wave_strand_intensity) const {
        (void)height_val; // Unused for now
        
        // UNIFIED WATER ANIMATION NETWORK - Seamless shore-to-deep system
        float shore_distance = static_cast<float>(dist_to_land);
        
        // UNIVERSAL WATER ANIMATION - Works at all depths
        float base_time = global_water_animation_progress * WAVE_SPEED;
        
        // Create wave layers that work across entire lake
        // Layer 1: Primary shore waves (strongest near shore)
        float shore_wave_intensity = 1.0f;
        if (shore_distance >= 0) {
            shore_wave_intensity = std::max(0.2f, 1.0f - (shore_distance / static_cast<float>(WAVE_MAX_DISTANCE_FROM_SHORE)));
        }
        
        // Shore waves flow perpendicular to shore (inward)
        float shore_wave_phase = base_time - shore_distance * WAVE_FREQUENCY;
        float shore_wave = std::sin(shore_wave_phase * 2.0f * 3.14159f + tile_anim_offset * 6.28f) * shore_wave_intensity;
        
        // Layer 2: Deep water currents (gentle, omnipresent)
        float deep_current_phase = base_time * 0.3f + tile_anim_offset * 2.0f;
        float deep_current = std::sin(deep_current_phase * 2.0f * 3.14159f) * 0.4f;
        
        // Layer 3: Lake-wide pulses (very slow, affects entire lake)
        float lake_pulse_phase = base_time * 0.15f + tile_anim_offset * 1.5f;
        float lake_pulse = std::sin(lake_pulse_phase * 2.0f * 3.14159f) * 0.2f;
        
        // Layer 4: Surface texture (fast, subtle)
        float surface_texture_phase = base_time * 2.0f + tile_anim_offset * 8.0f;
        float surface_texture = std::sin(surface_texture_phase * 2.0f * 3.14159f) * 0.15f;
        
        // SEAMLESS BLENDING - Combine all layers based on depth
        float total_wave;
        float wave_strength = tile_wave_strand_intensity;
        
        if (shore_distance >= 0 && shore_distance < WAVE_MAX_DISTANCE_FROM_SHORE) {
            // Near shore: Strong shore waves + background layers
            total_wave = shore_wave * wave_strength + 
                       deep_current * 0.3f + 
                       lake_pulse * 0.5f + 
                       surface_texture * 0.2f;
        } else {
            // Deep water: Gentle currents + lake pulse + surface texture
            total_wave = deep_current * 0.6f + 
                       lake_pulse * 0.8f + 
                       surface_texture * 0.3f +
                       shore_wave * 0.1f; // Faint shore wave influence
        }
        
        return determineWaveVisual(total_wave, shore_distance, wave_strength, tile_anim_offset);
    }

    Core::ScreenCell determineWaveVisual(float total_wave, float shore_distance, 
                                        float wave_strength, float tile_anim_offset) const {
        char ch;
        sf::Color fg, bg;
        
        unsigned int tile_hash = static_cast<unsigned int>(tile_anim_offset * 1000);
        
        // DYNAMIC WATER DEPTH ZONES - Scaled for massive medieval lakes (300-1000+ tiles deep)
        float depth_factor = (shore_distance >= 0) ? shore_distance : 200.0f;
        
        // Zone 1: Active surf zone (0-5 units from shore)
        if (depth_factor <= 5.0f) {
            float breaking_threshold = 0.6f - depth_factor * 0.08f; // Higher threshold near shore
            
            if (total_wave > breaking_threshold && wave_strength > 0.3f) {
                ch = Colors::WAVE_FOAM_HEAVY;
                fg = Colors::WAVE_FOAM_PRIMARY;
                bg = Colors::WAVE_FOAM_PRIMARY_BG;
            } else if (total_wave > breaking_threshold * 0.6f) {
                ch = Colors::WAVE_FOAM_MEDIUM;
                fg = Colors::WAVE_FOAM_SECONDARY;
                bg = Colors::WAVE_FOAM_SECONDARY_BG;
            } else if (total_wave > 0.2f) {
                ch = Colors::WAVE_CREST;
                fg = Colors::WAVE_CREST_FG;
                bg = Colors::WAVE_CREST_BG;
            } else if (total_wave < -0.3f) {
                ch = Colors::WAVE_TROUGH;
                bg = Colors::WATER_SHORE_LAKE;
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.3f);
            } else {
                ch = Colors::SHORE_WATER_CHAR;
                bg = Colors::WATER_SHORE_LAKE;
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.25f + total_wave * 0.1f);
            }
        }
        // Zone 2: Shallow transition (5-15 units)
        else if (depth_factor <= 15.0f) {
            float transition_blend = (depth_factor - 5.0f) / 10.0f;
            
            if (total_wave > 0.4f) {
                ch = Colors::WAVE_CREST;
                fg = Colors::WAVE_CREST_FG;
                bg = Tile::interpolateColor(Colors::WAVE_CREST_BG, Colors::WATER_SHALLOW_LAKE, transition_blend);
            } else if (total_wave < -0.2f) {
                ch = Colors::WAVE_TROUGH;
                bg = Tile::interpolateColor(Colors::WATER_SHORE_LAKE, Colors::WATER_SHALLOW_LAKE, transition_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.2f);
            } else if (total_wave > 0.15f) {
                ch = Colors::WAVE_SWELL;
                bg = Tile::interpolateColor(Colors::WATER_SHORE_LAKE, Colors::WATER_SHALLOW_LAKE, transition_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.18f + total_wave * 0.05f);
            } else {
                ch = (total_wave > 0.05f) ? Colors::WAVE_RIPPLE : Colors::WATER_CALM;
                bg = Tile::interpolateColor(Colors::WATER_SHORE_LAKE, Colors::WATER_SHALLOW_LAKE, transition_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.15f + total_wave * 0.03f);
            }
        }
        // Zone 3: Medium depth (15-50 units)
        else if (depth_factor <= 50.0f) {
            float mid_blend = (depth_factor - 15.0f) / 35.0f;
            
            if (total_wave > 0.25f) {
                ch = Colors::WAVE_SWELL;
                bg = Tile::interpolateColor(Colors::WATER_SHALLOW_LAKE, Colors::WATER_MID_LAKE, mid_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.15f + total_wave * 0.05f);
            } else if (total_wave < -0.15f) {
                ch = Colors::WAVE_TROUGH;
                bg = Tile::interpolateColor(Colors::WATER_SHALLOW_LAKE, Colors::WATER_MID_LAKE, mid_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.12f);
            } else if (total_wave > 0.08f) {
                ch = Colors::WAVE_RIPPLE;
                bg = Tile::interpolateColor(Colors::WATER_SHALLOW_LAKE, Colors::WATER_MID_LAKE, mid_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.13f + total_wave * 0.02f);
            } else {
                ch = Colors::WATER_CALM;
                bg = Tile::interpolateColor(Colors::WATER_SHALLOW_LAKE, Colors::WATER_MID_LAKE, mid_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.11f + total_wave * 0.01f);
            }
        }
        // Zone 4: Deep water (50-150 units)
        else if (depth_factor <= 150.0f) {
            float deep_blend = (depth_factor - 50.0f) / 100.0f;
            
            if (total_wave > 0.2f) {
                ch = Colors::WAVE_RIPPLE;
                bg = Tile::interpolateColor(Colors::WATER_MID_LAKE, Colors::WATER_DEEP_LAKE_CORE, deep_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.1f + total_wave * 0.02f);
            } else if (total_wave < -0.1f) {
                ch = Colors::WATER_CALM;
                bg = Tile::interpolateColor(Colors::WATER_MID_LAKE, Colors::WATER_DEEP_LAKE_CORE, deep_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.08f);
            } else if (total_wave > 0.05f) {
                ch = (tile_hash % 3 == 0) ? Colors::WAVE_RIPPLE : Colors::WATER_CALM;
                bg = Tile::interpolateColor(Colors::WATER_MID_LAKE, Colors::WATER_DEEP_LAKE_CORE, deep_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.09f + total_wave * 0.01f);
            } else {
                ch = Colors::WATER_DEEP;
                bg = Tile::interpolateColor(Colors::WATER_MID_LAKE, Colors::WATER_DEEP_LAKE_CORE, deep_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.08f + total_wave * 0.005f);
            }
        }
        // Zone 5: Mystical depths (150+ units)
        else {
            float mystical_blend = std::min(1.0f, (depth_factor - 150.0f) / 200.0f);
            
            if (total_wave > 0.15f) {
                ch = Colors::WAVE_FOAM_LIGHT;
                bg = Tile::interpolateColor(Colors::WATER_DEEP_LAKE_CORE, Colors::WATER_DEEP_MYSTICAL, mystical_blend);
                fg = Tile::interpolateColor(bg, Colors::WAVE_SPARKLE_FG, 0.5f);
            } else if (total_wave > 0.03f) {
                ch = (tile_hash % 4 == 0) ? Colors::WAVE_RIPPLE : Colors::WATER_DEEP;
                bg = Tile::interpolateColor(Colors::WATER_DEEP_LAKE_CORE, Colors::WATER_DEEP_MYSTICAL, mystical_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.06f + total_wave * 0.02f);
            } else {
                ch = Colors::WATER_DEEP;
                bg = Tile::interpolateColor(Colors::WATER_DEEP_LAKE_CORE, Colors::WATER_DEEP_MYSTICAL, mystical_blend);
                fg = Tile::interpolateColor(bg, Core::Colors::WHITE, 0.05f + total_wave * 0.01f);
            }
            
            // Rare mystical sparkles in the deepest waters
            int sparkle_rarity = std::max(800, 2000 - static_cast<int>(depth_factor));
            if (total_wave > 0.1f && (tile_hash % sparkle_rarity) == 0) {
                ch = Colors::WAVE_FOAM_LIGHT;
                fg = Colors::WAVE_SPARKLE_FG;
            }
        }
        
        return {ch, fg, bg};
    }
};

} // namespace Lakes
} // namespace Systems
} // namespace World