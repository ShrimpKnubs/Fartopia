// File: EmergentKingdoms/src/World/Tile.cpp
#include "Tile.h"
#include "Systems/Rivers/RiverTileRenderer.h"
#include "Systems/Lakes/LakeTileRenderer.h"
#include "Systems/Mountains/MountainTileRenderer.h"
#include "Systems/Land/LandTileRenderer.h"
#include "Systems/Vegetation/VegetationTileRenderer.h"
#include "../Core/BaseConfig.h"
#include <vector>
#include <memory>

namespace World {

// Static registry of tile renderers
std::vector<std::unique_ptr<Systems::ITileRenderer>> createTileRenderers() {
    std::vector<std::unique_ptr<Systems::ITileRenderer>> renderers;
    
    // Order matters - first match wins
    renderers.push_back(std::make_unique<Systems::Rivers::RiverTileRenderer>());
    renderers.push_back(std::make_unique<Systems::Lakes::LakeTileRenderer>());
    renderers.push_back(std::make_unique<Systems::Mountains::MountainTileRenderer>());
    renderers.push_back(std::make_unique<Systems::Vegetation::VegetationTileRenderer>()); // NEW!
    renderers.push_back(std::make_unique<Systems::Land::LandTileRenderer>());
    
    return renderers;
}

Core::ScreenCell Tile::determineDisplay(BaseTileType base_type, float height_val, float slope_val, 
                                        SlopeAspect aspect_val, int dist_to_land, 
                                        float global_water_animation_progress, 
                                        float tile_anim_offset, float tile_wave_strand_intensity,
                                        bool is_marsh_water_patch_flag, int tile_dist_to_water) {
    
    static auto renderers = createTileRenderers();
    
    // Find the appropriate renderer for this tile type
    for (const auto& renderer : renderers) {
        if (renderer->canRender(base_type)) {
            return renderer->render(base_type, height_val, slope_val, aspect_val, dist_to_land,
                                  global_water_animation_progress, tile_anim_offset, tile_wave_strand_intensity,
                                  is_marsh_water_patch_flag, tile_dist_to_water);
        }
    }
    
    // Fallback if no renderer found
    return {' ', Core::Colors::WHITE, Core::Colors::BLACK};
}

Tile Tile::create(BaseTileType base_type, float height, float slope, SlopeAspect aspect, 
                  int distance_to_land_val, int distance_to_water_val, 
                  float anim_offset_val, float strand_intensity_val, 
                  bool is_marsh_water_patch_flag) { 
    Tile t;
    t.base_type = base_type;
    t.height_val = height;
    t.slope_val = slope;
    t.aspect_val = aspect;
    t.distance_to_land = distance_to_land_val;
    t.distance_to_water = distance_to_water_val; 
    t.animation_offset = anim_offset_val;
    t.wave_strand_intensity = strand_intensity_val;
    t.is_marsh_water_patch = is_marsh_water_patch_flag; 

    t.display_cell = determineDisplay(base_type, height, slope, aspect, t.distance_to_land, 0.0f, 
                                     t.animation_offset, t.wave_strand_intensity, t.is_marsh_water_patch, t.distance_to_water);

    t.is_passable = determinePassability(base_type, slope);

    return t;
}

Tile Tile::createSpecial(BaseTileType special_type) { 
    Tile t;
    t.base_type = special_type;
    t.distance_to_land = -1; 
    t.distance_to_water = -1; 
    t.animation_offset = 0.0f;
    t.wave_strand_intensity = 0.0f;
    t.is_marsh_water_patch = false;

    if (special_type == BaseTileType::BORDER_WALL) { 
        t.height_val = 1.0f; 
        t.slope_val = 1.0f; 
        t.aspect_val = SlopeAspect::STEEP_PEAK; 
        t.is_passable = false; 
    } else { 
        t.height_val = 0.0f; 
        t.slope_val = 0.0f; 
        t.aspect_val = SlopeAspect::FLAT; 
        t.is_passable = false; 
    }
    
    t.display_cell = determineDisplay(t.base_type, t.height_val, t.slope_val, t.aspect_val, 
                                     t.distance_to_land, 0.0f, t.animation_offset, t.wave_strand_intensity, 
                                     t.is_marsh_water_patch, t.distance_to_water);
    return t;
}

bool Tile::determinePassability(BaseTileType base_type, float slope_val) {
    // Impassable tiles
    if (base_type == BaseTileType::MOUNTAIN_PEAK_SNOW || 
        base_type == BaseTileType::CLIFF_FACE || 
        base_type == BaseTileType::RIVER_WATER || 
        base_type == BaseTileType::LAKE_WATER || 
        base_type == BaseTileType::POND_WATER || 
        base_type == BaseTileType::BORDER_WALL || 
        base_type == BaseTileType::VOID) { 
        return false; 
    }
    
    // ===== VEGETATION PASSABILITY =====
    // Impassable vegetation
    if (base_type == BaseTileType::DENSE_THICKET ||
        base_type == BaseTileType::DENSE_FOREST ||
        base_type == BaseTileType::STANDING_STONE ||
        base_type == BaseTileType::MOSSY_BOULDER) {
        return false;
    }
    
    // Trees slow movement but are passable
    if (base_type == BaseTileType::ANCIENT_OAK ||
        base_type == BaseTileType::NOBLE_PINE ||
        base_type == BaseTileType::SILVER_BIRCH ||
        base_type == BaseTileType::WEEPING_WILLOW ||
        base_type == BaseTileType::YOUNG_TREE ||
        base_type == BaseTileType::TREE_GROVE) {
        return true;  // Passable but could affect movement speed
    }
    
    // Bushes are passable but may slow movement
    if (base_type == BaseTileType::BERRY_BUSH ||
        base_type == BaseTileType::WILD_ROSES) {
        return true;  // Passable, thorns might cause damage
    }
    
    // Flowers, herbs, small rocks are easily passable
    if (base_type == BaseTileType::WILDFLOWER_MEADOW ||
        base_type == BaseTileType::WILDFLOWERS ||
        base_type == BaseTileType::ROYAL_LILY ||
        base_type == BaseTileType::HERB_PATCH ||
        base_type == BaseTileType::ROCK_OUTCROP ||
        base_type == BaseTileType::FLOWING_GRASS) {
        return true;
    }
    
    // Resource deposits are passable (for mining)
    if (base_type == BaseTileType::GOLD_VEIN ||
        base_type == BaseTileType::SILVER_LODE ||
        base_type == BaseTileType::IRON_ORE ||
        base_type == BaseTileType::COPPER_DEPOSIT) {
        return true;
    }
    
    // Slope-dependent passability for existing terrain
    if ((base_type == BaseTileType::MOUNTAIN_UPPER || base_type == BaseTileType::MOUNTAIN_MID) && 
        slope_val > Core::SLOPE_THRESHOLD_VERY_STEEP) { 
        return false; 
    }
    
    if (base_type == BaseTileType::ROCKY_SLOPE && slope_val > Core::SLOPE_THRESHOLD_STEEP * 1.5f) { 
         return false; 
    }

    return true;
}

} // namespace World