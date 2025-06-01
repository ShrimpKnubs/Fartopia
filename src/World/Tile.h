// File: EmergentKingdoms/src/World/Tile.h
#pragma once
#include "../Core/Renderer.h"
#include <SFML/Graphics.hpp>

namespace World {

enum class BaseTileType { 
    // Original terrain types
    PLAINS, HILLS, STEEP_SLOPE, CLIFF_FACE, MOUNTAIN_LOWER, MOUNTAIN_MID, 
    MOUNTAIN_UPPER, MOUNTAIN_PEAK_SNOW, RIVER_WATER, LAKE_WATER, BORDER_WALL, VOID,
    MEADOW, MARSH, POND_WATER, MOOR, PLATEAU_GRASS, ROCKY_SLOPE, DRY_PLAINS,
    
    // ===== MEDIEVAL VEGETATION SYSTEM =====
    // Trees - Noble and Ancient
    ANCIENT_OAK,        // Majestic ancient oak trees (Ω)
    NOBLE_PINE,         // Tall noble pines (↑) 
    SILVER_BIRCH,       // Elegant silver birch (!)
    WEEPING_WILLOW,     // Graceful weeping willows (ψ)
    YOUNG_TREE,         // Smaller growing trees (Y)
    TREE_GROVE,         // Clusters of trees (♠)
    DENSE_FOREST,       // Thick forest coverage (#)
    
    // Bushes & Undergrowth
    DENSE_THICKET,      // Impenetrable undergrowth (#)
    BERRY_BUSH,         // Berry-producing bushes (●)
    WILD_ROSES,         // Thorny wild rose bushes (❀)
    
    // Flowers & Herbs  
    WILDFLOWER_MEADOW,  // Flowering meadow with flowing grass
    WILDFLOWERS,        // Scattered wildflowers (*)
    ROYAL_LILY,         // Elegant white lilies (♦)
    HERB_PATCH,         // Useful medicinal herbs (≈)
    
    // Rocks & Geological Features
    MOSSY_BOULDER,      // Large moss-covered boulders (O)
    STANDING_STONE,     // Ancient monolithic stones (‖)
    ROCK_OUTCROP,       // Natural rock formations (°)
    
    // Resource Deposits for Medieval Economy
    GOLD_VEIN,          // Precious gold deposits ($)
    SILVER_LODE,        // Valuable silver ore (§)
    IRON_ORE,           // Essential iron for tools/weapons (■)
    COPPER_DEPOSIT,     // Versatile copper deposits (◊)
    
    // Special Grass Animation
    FLOWING_GRASS       // Wind-animated grass with directional flow
};

enum class SlopeAspect { 
    FLAT, NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST, STEEP_PEAK 
};

struct Tile {
    BaseTileType base_type = BaseTileType::PLAINS;
    Core::ScreenCell display_cell;
    bool is_passable = true; 
    float height_val = 0.0f; 
    float slope_val = 0.0f;  
    SlopeAspect aspect_val = SlopeAspect::FLAT;
    int distance_to_land = -1; 
    int distance_to_water = -1; 
    float animation_offset = 0.0f;      // Used for wind direction in vegetation
    float wave_strand_intensity = 0.0f; 
    bool is_marsh_water_patch = false; 

    Tile() = default;

    // Utility function for color interpolation (used by all systems)
    static sf::Color interpolateColor(const sf::Color& c1, const sf::Color& c2, float t) {
        t = std::max(0.0f, std::min(1.0f, t));
        sf::Uint8 r = static_cast<sf::Uint8>(c1.r + (c2.r - c1.r) * t);
        sf::Uint8 g = static_cast<sf::Uint8>(c1.g + (c2.g - c1.g) * t);
        sf::Uint8 b = static_cast<sf::Uint8>(c1.b + (c2.b - c1.b) * t);
        return sf::Color(r, g, b);
    }

    // Main display determination - delegates to system-specific renderers
    static Core::ScreenCell determineDisplay(BaseTileType base_type, float height_val, float slope_val, 
                                             SlopeAspect aspect_val, int dist_to_land, 
                                             float global_water_animation_progress, 
                                             float tile_anim_offset, float tile_wave_strand_intensity,
                                             bool is_marsh_water_patch_flag, int tile_dist_to_water);

    // Factory method for creating tiles
    static Tile create(BaseTileType base_type, float height, float slope, SlopeAspect aspect, 
                       int distance_to_land_val = -1, int distance_to_water_val = -1, 
                       float anim_offset_val = 0.0f, float strand_intensity_val = 0.0f, 
                       bool is_marsh_water_patch_flag = false);

    // Special tile factory
    static Tile createSpecial(BaseTileType special_type);

private:
    // Helper to determine passability based on tile type and slope
    static bool determinePassability(BaseTileType base_type, float slope_val);
};

} // namespace World