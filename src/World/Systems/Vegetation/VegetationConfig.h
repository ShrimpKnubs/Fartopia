// File: EmergentKingdoms/src/World/Systems/Vegetation/VegetationConfig.h
#pragma once
#include "../../../Core/BaseConfig.h"

namespace World {
namespace Systems {
namespace Vegetation {

// ===== VEGETATION DENSITY & DISTRIBUTION =====
const float FOREST_DENSITY_THRESHOLD = 0.7f;           // Dense forest formation
const float WOODLAND_DENSITY_THRESHOLD = 0.5f;         // Scattered woodland
const float GROVE_DENSITY_THRESHOLD = 0.3f;            // Small tree groups

// ===== TREE PLACEMENT RULES =====
const float TREE_MIN_HEIGHT = Core::TERRAIN_PLAINS_LOW;
const float TREE_MAX_HEIGHT = Core::TERRAIN_STEEP_SLOPES;
const float TREE_MAX_SLOPE = Core::SLOPE_THRESHOLD_MODERATE * 1.5f;

// Ancient trees prefer higher, more dramatic locations
const float ANCIENT_TREE_MIN_HEIGHT = Core::TERRAIN_ROLLING_HILLS_LOW;
const float NOBLE_TREE_SPACING = 3.0f;                 // Minimum distance between noble trees

// ===== BUSH & UNDERGROWTH =====
const float BUSH_DENSITY = 0.4f;                       // Chance of bush clusters
const float UNDERGROWTH_COVERAGE = 0.6f;               // Dense undergrowth in forests
const float THICKET_DENSITY_THRESHOLD = 0.8f;          // Impenetrable thickets

// ===== FLOWER & HERB MEADOWS =====
const float FLOWER_MEADOW_CHANCE = 0.3f;               // Beautiful flowering meadows
const float WILDFLOWER_SCATTER = 0.2f;                 // Random wildflowers
const float HERB_PATCH_SIZE = 2.5f;                    // Clustered herb areas

// ===== ROCK & BOULDER FORMATION =====
const float BOULDER_CHANCE = 0.15f;                    // Scattered boulders
const float STANDING_STONE_RARITY = 0.05f;             // Ancient monoliths
const float ROCK_OUTCROP_MIN_SLOPE = Core::SLOPE_THRESHOLD_GENTLE;

// ===== RESOURCE DEPOSIT CONFIGURATION =====
const float GOLD_VEIN_RARITY = 0.02f;                  // Rare and precious
const float SILVER_LODE_RARITY = 0.03f;                // Uncommon but valuable  
const float IRON_ORE_CHANCE = 0.08f;                   // Common and useful
const float COPPER_DEPOSIT_CHANCE = 0.06f;             // Moderately common

// Resource placement preferences
const float GOLD_MIN_HEIGHT = Core::TERRAIN_ROLLING_HILLS_HIGH;  // In hills and mountains
const float IRON_MAX_HEIGHT = Core::TERRAIN_UPLANDS_LOW;        // Lower elevations
const float COPPER_NEAR_WATER_BONUS = 2.0f;           // Higher chance near water

// ===== WIND GRASS ANIMATION =====
const float GRASS_WAVE_SPEED = 0.8f;                   // Gentle swaying speed
const float GRASS_WAVE_INTENSITY = 0.6f;               // How pronounced the waves are
const int WIND_PATTERN_VARIATIONS = 8;                 // Different wind directions

// ===== SHORELINE VEGETATION =====
const int SHORE_VEGETATION_DISTANCE = 4;               // How far from water vegetation grows
const float WATER_LOVING_PLANT_CHANCE = 0.7f;          // Willows, reeds near water

// ===== VEGETATION EXCLUSION ZONES =====
const float MARSH_VEGETATION_LIMIT = 0.3f;             // Limited vegetation in marshes
const float MOUNTAIN_VEGETATION_CUTOFF = Core::TERRAIN_MOUNTAIN_BASE; // No vegetation above this

// ===== MEDIEVAL RESOURCE YIELD (for future gathering) =====
const int GOLD_VEIN_YIELD = 50;                        // High value, low quantity
const int SILVER_LODE_YIELD = 75;                      // Good value, moderate quantity
const int IRON_ORE_YIELD = 150;                        // Essential for tools/weapons
const int COPPER_DEPOSIT_YIELD = 125;                  // Useful for many purposes

} // namespace Vegetation
} // namespace Systems
} // namespace World