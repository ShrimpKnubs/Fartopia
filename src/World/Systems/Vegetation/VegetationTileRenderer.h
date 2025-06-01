// File: EmergentKingdoms/src/World/Systems/Vegetation/VegetationTileRenderer.h
#pragma once
#include "../ITileRenderer.h"
#include "VegetationColors.h"
#include "VegetationConfig.h"
#include "../../../Core/BaseConfig.h"
#include <cmath>

namespace World {
namespace Systems {
namespace Vegetation {

class VegetationTileRenderer : public ITileRenderer {
public:
    bool canRender(BaseTileType tile_type) const override {
        return tile_type == BaseTileType::ANCIENT_OAK ||
               tile_type == BaseTileType::NOBLE_PINE ||
               tile_type == BaseTileType::SILVER_BIRCH ||
               tile_type == BaseTileType::WEEPING_WILLOW ||
               tile_type == BaseTileType::YOUNG_TREE ||
               tile_type == BaseTileType::TREE_GROVE ||
               tile_type == BaseTileType::DENSE_FOREST ||
               tile_type == BaseTileType::DENSE_THICKET ||
               tile_type == BaseTileType::BERRY_BUSH ||
               tile_type == BaseTileType::WILD_ROSES ||
               tile_type == BaseTileType::WILDFLOWER_MEADOW ||
               tile_type == BaseTileType::WILDFLOWERS ||
               tile_type == BaseTileType::ROYAL_LILY ||
               tile_type == BaseTileType::HERB_PATCH ||
               tile_type == BaseTileType::MOSSY_BOULDER ||
               tile_type == BaseTileType::STANDING_STONE ||
               tile_type == BaseTileType::ROCK_OUTCROP ||
               tile_type == BaseTileType::GOLD_VEIN ||
               tile_type == BaseTileType::SILVER_LODE ||
               tile_type == BaseTileType::IRON_ORE ||
               tile_type == BaseTileType::COPPER_DEPOSIT ||
               tile_type == BaseTileType::FLOWING_GRASS;
    }

    Core::ScreenCell render(
        BaseTileType base_type, 
        float height_val, 
        float slope_val, 
        SlopeAspect aspect_val, 
        int dist_to_land, 
        float global_water_animation_progress, 
        float tile_anim_offset, 
        float /* tile_wave_strand_intensity */,
        bool is_marsh_water_patch_flag, 
        int tile_dist_to_water
    ) const override {
        
        (void)dist_to_land;
        (void)is_marsh_water_patch_flag;
        (void)tile_dist_to_water;

        unsigned int tile_hash = static_cast<unsigned int>(height_val * 1000 + slope_val * 100 + 
                                                          static_cast<int>(aspect_val) + tile_anim_offset * 100);

        switch (base_type) {
            // ===== TREES =====
            case BaseTileType::ANCIENT_OAK:
                return renderAncientOak(tile_hash, height_val);
            case BaseTileType::NOBLE_PINE:
                return renderNoblePine(tile_hash, height_val);
            case BaseTileType::SILVER_BIRCH:
                return renderSilverBirch(tile_hash, height_val);
            case BaseTileType::WEEPING_WILLOW:
                return renderWeepingWillow(tile_hash, height_val);
            case BaseTileType::YOUNG_TREE:
                return renderYoungTree(tile_hash, height_val);
            case BaseTileType::TREE_GROVE:
                return renderTreeGrove(tile_hash, height_val);
            case BaseTileType::DENSE_FOREST:
                return renderDenseForest(tile_hash, height_val);
                
            // ===== BUSHES & UNDERGROWTH =====
            case BaseTileType::DENSE_THICKET:
                return renderDenseThicket(tile_hash, height_val);
            case BaseTileType::BERRY_BUSH:
                return renderBerryBush(tile_hash, height_val);
            case BaseTileType::WILD_ROSES:
                return renderWildRoses(tile_hash, height_val);
                
            // ===== FLOWERS & HERBS =====
            case BaseTileType::WILDFLOWER_MEADOW:
                return renderWildflowerMeadow(tile_hash, global_water_animation_progress, tile_anim_offset, height_val);
            case BaseTileType::WILDFLOWERS:
                return renderWildflowers(tile_hash, height_val);
            case BaseTileType::ROYAL_LILY:
                return renderRoyalLily(tile_hash, height_val);
            case BaseTileType::HERB_PATCH:
                return renderHerbPatch(tile_hash, height_val);
                
            // ===== ROCKS & STONES =====
            case BaseTileType::MOSSY_BOULDER:
                return renderMossyBoulder(tile_hash, height_val);
            case BaseTileType::STANDING_STONE:
                return renderStandingStone(tile_hash, height_val);
            case BaseTileType::ROCK_OUTCROP:
                return renderRockOutcrop(tile_hash, height_val);
                
            // ===== RESOURCE DEPOSITS =====
            case BaseTileType::GOLD_VEIN:
                return renderGoldVein(tile_hash, global_water_animation_progress, height_val);
            case BaseTileType::SILVER_LODE:
                return renderSilverLode(tile_hash, global_water_animation_progress, height_val);
            case BaseTileType::IRON_ORE:
                return renderIronOre(tile_hash, height_val);
            case BaseTileType::COPPER_DEPOSIT:
                return renderCopperDeposit(tile_hash, height_val);
                
            // ===== FLOWING GRASS =====
            case BaseTileType::FLOWING_GRASS:
                return renderFlowingGrass(tile_hash, global_water_animation_progress, tile_anim_offset, height_val);
                
            default:
                return {'?', Core::Colors::RED, Core::Colors::BLACK};
        }
    }

    std::string getSystemName() const override {
        return "Vegetation";
    }

private:
    // Helper to get proper terrain background color based on height
    sf::Color getTerrainBackground(float height_val) const {
        if (height_val < Core::TERRAIN_PLAINS_LOW) {
            return Core::LandColors::GRASS_DARK_VALLEY;
        } else if (height_val < Core::TERRAIN_PLAINS_HIGH) {
            return Core::LandColors::PLAINS_GRASS_BASE;
        } else if (height_val < Core::TERRAIN_ROLLING_HILLS_HIGH) {
            return Core::LandColors::GRASS_MID_SLOPE;
        } else {
            return Core::LandColors::GRASS_LIGHT_HILLTOP;
        }
    }

    // ===== TREE RENDERERS =====
    Core::ScreenCell renderAncientOak(unsigned int tile_hash, float height_val) const {
        char ch = 'T';
        sf::Color bg = getTerrainBackground(height_val);
        sf::Color fg = Colors::ANCIENT_OAK_CANOPY;
        
        // Variation in ancient oak appearance
        if (tile_hash % 3 == 0) {
            bg = Tile::interpolateColor(bg, Colors::ANCIENT_OAK_TRUNK, 0.3f);
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderNoblePine(unsigned int tile_hash, float height_val) const {
        char ch = 'A';
        sf::Color bg = getTerrainBackground(height_val);
        sf::Color fg = Colors::NOBLE_PINE_NEEDLES;
        
        if (tile_hash % 4 == 0) {
            fg = Tile::interpolateColor(fg, sf::Color(45, 65, 40), 0.2f);
            bg = Tile::interpolateColor(bg, Colors::NOBLE_PINE_TRUNK, 0.2f);
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderSilverBirch(unsigned int tile_hash, float height_val) const {
        char ch = '!';
        sf::Color bg = getTerrainBackground(height_val);
        sf::Color fg = Colors::SILVER_BIRCH_LEAVES;
        
        if (tile_hash % 5 == 0) {
            bg = Tile::interpolateColor(bg, Colors::SILVER_BIRCH_BARK, 0.2f);
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderWeepingWillow(unsigned int tile_hash, float height_val) const {
        char ch = 'W';
        sf::Color bg = getTerrainBackground(height_val);
        sf::Color fg = Colors::WEEPING_WILLOW_FRONDS;
        
        if (tile_hash % 3 == 0) {
            fg = Tile::interpolateColor(fg, Colors::MEADOW_GRASS_WAVE, 0.2f);
            bg = Tile::interpolateColor(bg, Colors::WEEPING_WILLOW_TRUNK, 0.15f);
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderYoungTree(unsigned int tile_hash, float height_val) const {
        char ch = 'Y';
        sf::Color bg = getTerrainBackground(height_val);
        sf::Color fg = Colors::SILVER_BIRCH_LEAVES;
        
        if (tile_hash % 2 == 0) {
            fg = Colors::NOBLE_PINE_NEEDLES;
            bg = Tile::interpolateColor(bg, Colors::ANCIENT_OAK_TRUNK, 0.15f);
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderTreeGrove(unsigned int tile_hash, float height_val) const {
        char ch = '%';
        sf::Color bg = getTerrainBackground(height_val);
        sf::Color fg = Tile::interpolateColor(Colors::ANCIENT_OAK_CANOPY, 
                                             Colors::NOBLE_PINE_NEEDLES, 0.5f);
        
        if (tile_hash % 4 == 0) {
            fg = Tile::interpolateColor(fg, Colors::SILVER_BIRCH_LEAVES, 0.3f);
            bg = Tile::interpolateColor(bg, Colors::FOREST_UNDERGROWTH, 0.3f);
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderDenseForest(unsigned int tile_hash, float height_val) const {
        char ch = '#';
        sf::Color bg = Tile::interpolateColor(getTerrainBackground(height_val), Colors::DENSE_THICKET_BG, 0.5f);
        sf::Color fg = Colors::ANCIENT_OAK_CANOPY;
        
        // Very dense, dark forest
        if (tile_hash % 3 == 0) {
            bg = Tile::interpolateColor(bg, sf::Color(15, 25, 12), 0.3f);
        }
        
        return {ch, fg, bg};
    }
    
    // ===== BUSH RENDERERS =====
    Core::ScreenCell renderDenseThicket(unsigned int tile_hash, float height_val) const {
        char ch = '#';
        sf::Color bg = Tile::interpolateColor(getTerrainBackground(height_val), Colors::DENSE_THICKET_BG, 0.4f);
        sf::Color fg = Colors::DENSE_THICKET_FG;
        
        if (tile_hash % 4 == 0) {
            fg = Tile::interpolateColor(fg, Colors::BERRY_BUSH_BG, 0.3f);
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderBerryBush(unsigned int tile_hash, float height_val) const {
        char ch = 'o';
        sf::Color bg = getTerrainBackground(height_val);
        sf::Color fg = Colors::BERRY_BUSH_BERRIES;
        
        // Sometimes show the green bush instead of berries
        if (tile_hash % 3 == 0) {
            fg = Tile::interpolateColor(Colors::BERRY_BUSH_BG, Colors::MEADOW_GRASS_WAVE, 0.4f);
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderWildRoses(unsigned int tile_hash, float height_val) const {
        char ch = '@';
        sf::Color bg = getTerrainBackground(height_val); // FIXED: Always use terrain background
        sf::Color fg = Colors::WILD_ROSE_BLOOM;
        
        if (tile_hash % 4 == 0) {
            fg = Tile::interpolateColor(Colors::WILD_ROSE_BG, fg, 0.6f);
        }
        
        return {ch, fg, bg};
    }
    
    // ===== FLOWER RENDERERS =====
    Core::ScreenCell renderWildflowerMeadow(unsigned int tile_hash, float animation_progress, 
                                           float wind_offset, float height_val) const {
        // Flowing grass with wildflowers
        char ch = getWindGrassCharacter(tile_hash, animation_progress, wind_offset);
        sf::Color bg = getTerrainBackground(height_val); // FIXED: Always use terrain background
        sf::Color fg = Colors::MEADOW_GRASS_WAVE;
        
        // Occasional flowers
        if (tile_hash % 6 == 0) {
            ch = '*';
            fg = (tile_hash % 12 < 4) ? Colors::MEADOW_BUTTERCUP : 
                 (tile_hash % 12 < 8) ? Colors::FOREST_VIOLET : Colors::ROYAL_LILY_WHITE;
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderWildflowers(unsigned int tile_hash, float height_val) const {
        char ch = '*';
        sf::Color bg = getTerrainBackground(height_val); // FIXED: Always use terrain background
        sf::Color fg;
        
        // Different flower colors
        switch (tile_hash % 4) {
            case 0: fg = Colors::MEADOW_BUTTERCUP; break;
            case 1: fg = Colors::FOREST_VIOLET; break;
            case 2: fg = Colors::WILD_ROSE_BLOOM; break;
            default: fg = Colors::ROYAL_LILY_WHITE; break;
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderRoyalLily(unsigned int tile_hash, float height_val) const {
        char ch = '&';
        sf::Color bg = getTerrainBackground(height_val); // FIXED: Always use terrain background
        sf::Color fg = (tile_hash % 2 == 0) ? Colors::ROYAL_LILY_WHITE : Colors::ROYAL_LILY_GOLD;
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderHerbPatch(unsigned int tile_hash, float height_val) const {
        char ch = '~';
        sf::Color bg = getTerrainBackground(height_val); // FIXED: Always use terrain background
        sf::Color fg = Tile::interpolateColor(Colors::HERB_PATCH_GREEN, Colors::MEADOW_GRASS_WAVE, 0.4f);
        
        if (tile_hash % 3 == 0) {
            fg = Tile::interpolateColor(fg, Colors::ROYAL_LILY_GOLD, 0.2f);
        }
        
        return {ch, fg, bg};
    }
    
    // ===== ROCK RENDERERS =====
    Core::ScreenCell renderMossyBoulder(unsigned int tile_hash, float height_val) const {
        char ch = 'O';
        sf::Color bg = Tile::interpolateColor(getTerrainBackground(height_val), Colors::MOSSY_BOULDER_BASE, 0.6f);
        sf::Color fg = Colors::MOSSY_BOULDER_MOSS;
        
        if (tile_hash % 4 == 0) {
            bg = Tile::interpolateColor(bg, Colors::GRANITE_BOULDER, 0.2f);
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderStandingStone(unsigned int tile_hash, float height_val) const {
        char ch = 'I';
        sf::Color bg = Tile::interpolateColor(getTerrainBackground(height_val), Colors::STANDING_STONE_ANCIENT, 0.6f);
        sf::Color fg = Tile::interpolateColor(bg, sf::Color(120, 115, 108), 0.3f);
        
        if (tile_hash % 3 == 0) {
            bg = Tile::interpolateColor(bg, Colors::MOSSY_BOULDER_MOSS, 0.15f);
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderRockOutcrop(unsigned int tile_hash, float height_val) const {
        char ch = '.';
        sf::Color bg = getTerrainBackground(height_val); // Small rocks use terrain background
        sf::Color fg = Colors::ROCK_OUTCROP_GRAY;
        
        if (tile_hash % 5 == 0) {
            fg = Tile::interpolateColor(fg, Colors::MOSSY_BOULDER_MOSS, 0.15f);
        }
        
        return {ch, fg, bg};
    }
    
    // ===== RESOURCE RENDERERS =====
    Core::ScreenCell renderGoldVein(unsigned int tile_hash, float animation_progress, float height_val) const {
        char ch = '$';
        sf::Color bg = getTerrainBackground(height_val); // FIXED: Always use terrain background
        sf::Color fg = Colors::GOLD_VEIN_GLEAM;
        
        // Subtle sparkle animation
        float sparkle_phase = std::sin(animation_progress * 3.14159f * 2.0f) * 0.5f + 0.5f;
        if (tile_hash % 8 == 0 && sparkle_phase > 0.7f) {
            fg = Colors::GOLD_SPARKLE;
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderSilverLode(unsigned int tile_hash, float animation_progress, float height_val) const {
        char ch = '=';
        sf::Color bg = getTerrainBackground(height_val); // FIXED: Always use terrain background
        sf::Color fg = Colors::SILVER_LODE_GLEAM;
        
        // Silver sparkle
        float sparkle_phase = std::sin(animation_progress * 3.14159f * 1.5f) * 0.5f + 0.5f;
        if (tile_hash % 6 == 0 && sparkle_phase > 0.8f) {
            fg = Colors::SILVER_SPARKLE;
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderIronOre(unsigned int tile_hash, float height_val) const {
        char ch = '#';
        sf::Color bg = getTerrainBackground(height_val); // FIXED: Always use terrain background
        sf::Color fg = Colors::IRON_ORE_METAL;
        
        if (tile_hash % 3 == 0) {
            fg = Tile::interpolateColor(fg, Colors::IRON_RUST_HINT, 0.3f);
        }
        
        return {ch, fg, bg};
    }
    
    Core::ScreenCell renderCopperDeposit(unsigned int tile_hash, float height_val) const {
        char ch = '+';
        sf::Color bg = getTerrainBackground(height_val); // FIXED: Always use terrain background
        sf::Color fg = Colors::COPPER_DEPOSIT_GLEAM;
        
        if (tile_hash % 4 == 0) {
            fg = Tile::interpolateColor(fg, Colors::COPPER_PATINA, 0.4f);
        }
        
        return {ch, fg, bg};
    }
    
    // ===== FLOWING GRASS RENDERER =====
    Core::ScreenCell renderFlowingGrass(unsigned int tile_hash, float animation_progress, 
                                       float wind_offset, float height_val) const {
        char ch = getWindGrassCharacter(tile_hash, animation_progress, wind_offset);
        sf::Color bg = getTerrainBackground(height_val); // FIXED: Always use terrain background
        sf::Color fg = Colors::MEADOW_GRASS_WAVE;
        
        // Wind animation affects color intensity
        float wind_intensity = std::sin((animation_progress + wind_offset) * 3.14159f * 2.0f) * 0.3f + 0.7f;
        fg = Tile::interpolateColor(bg, Colors::MEADOW_GRASS_WIND, wind_intensity);
        
        return {ch, fg, bg};
    }
    
    // ===== HELPER METHODS =====
    char getWindGrassCharacter(unsigned int tile_hash, float animation_progress, float wind_offset) const {
        // Create flowing wind pattern
        float wind_phase = std::sin((animation_progress * GRASS_WAVE_SPEED + wind_offset) * 3.14159f * 2.0f);
        float wind_strength = std::abs(wind_phase) * GRASS_WAVE_INTENSITY;
        
        // Determine grass character based on wind strength and direction
        if (wind_strength < 0.2f) {
            return '|';
        } else if (wind_strength < 0.4f) {
            return (wind_phase > 0) ? '/' : '\\';
        } else if (wind_strength < 0.7f) {
            return (tile_hash % 2 == 0) ? '^' : 'v';
        } else {
            return '_';
        }
    }
};

} // namespace Vegetation
} // namespace Systems
} // namespace World