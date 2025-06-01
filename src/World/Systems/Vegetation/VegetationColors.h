// File: EmergentKingdoms/src/World/Systems/Vegetation/VegetationColors.h
#pragma once
#include <SFML/Graphics.hpp>

namespace World {
namespace Systems {
namespace Vegetation {
namespace Colors {

// ===== TASTEFUL MEDIEVAL TREE COLORS =====
const sf::Color ANCIENT_OAK_TRUNK = sf::Color(62, 45, 32);        // Deep weathered brown
const sf::Color ANCIENT_OAK_CANOPY = sf::Color(45, 62, 28);       // Rich forest green
const sf::Color NOBLE_PINE_TRUNK = sf::Color(58, 42, 30);         // Dark bark
const sf::Color NOBLE_PINE_NEEDLES = sf::Color(35, 52, 32);       // Deep evergreen
const sf::Color SILVER_BIRCH_BARK = sf::Color(185, 175, 162);     // Pale silvery bark
const sf::Color SILVER_BIRCH_LEAVES = sf::Color(68, 85, 42);      // Soft green leaves
const sf::Color WEEPING_WILLOW_TRUNK = sf::Color(72, 58, 45);     // Grayish brown
const sf::Color WEEPING_WILLOW_FRONDS = sf::Color(82, 98, 55);    // Drooping green

// ===== BUSH & UNDERGROWTH COLORS =====
const sf::Color DENSE_THICKET_BG = sf::Color(25, 35, 18);         // Very dark green
const sf::Color DENSE_THICKET_FG = sf::Color(45, 65, 32);         // Medium forest green
const sf::Color BERRY_BUSH_BG = sf::Color(38, 52, 28);            // Rich bush green
const sf::Color BERRY_BUSH_BERRIES = sf::Color(85, 32, 45);       // Deep red berries
const sf::Color WILD_ROSE_BG = sf::Color(42, 58, 35);             // Thorny green
const sf::Color WILD_ROSE_BLOOM = sf::Color(125, 78, 85);         // Muted pink roses
const sf::Color FOREST_UNDERGROWTH = sf::Color(32, 45, 22);       // Dark undergrowth

// ===== FLOWING GRASS COLORS (RICH & MUTED) =====
const sf::Color MEADOW_GRASS_BASE = sf::Color(58, 78, 42);        // Rich dark grass
const sf::Color MEADOW_GRASS_WAVE = sf::Color(72, 95, 55);        // Lighter wave crests
const sf::Color MEADOW_GRASS_WIND = sf::Color(85, 110, 68);       // Wind-blown highlights
const sf::Color PLAIN_GRASS_BASE = sf::Color(52, 72, 38);         // Slightly drier grass
const sf::Color PLAIN_GRASS_SWAY = sf::Color(68, 88, 52);         // Swaying motion
const sf::Color HIGHLAND_GRASS = sf::Color(48, 68, 35);           // Mountain grass

// ===== FLOWER & HERB COLORS =====
const sf::Color WILDFLOWER_MEADOW_BG = sf::Color(55, 75, 45);     // Flower meadow base
const sf::Color ROYAL_LILY_WHITE = sf::Color(242, 238, 225);      // Elegant white lilies
const sf::Color ROYAL_LILY_GOLD = sf::Color(198, 162, 85);        // Golden centers
const sf::Color FOREST_VIOLET = sf::Color(98, 65, 128);           // Deep purple violets
const sf::Color MEADOW_BUTTERCUP = sf::Color(185, 155, 62);       // Golden buttercups
const sf::Color HERB_PATCH_GREEN = sf::Color(62, 82, 48);         // Herb foliage

// ===== ROCK & STONE COLORS =====
const sf::Color MOSSY_BOULDER_BASE = sf::Color(78, 72, 68);       // Gray stone base
const sf::Color MOSSY_BOULDER_MOSS = sf::Color(42, 58, 35);       // Moss covering
const sf::Color STANDING_STONE_ANCIENT = sf::Color(88, 82, 75);   // Weathered monolith
const sf::Color ROCK_OUTCROP_GRAY = sf::Color(105, 98, 92);       // Natural rock
const sf::Color GRANITE_BOULDER = sf::Color(112, 108, 102);       // Solid granite

// ===== MEDIEVAL RESOURCE DEPOSIT COLORS =====
// Gold - Subtle but recognizable
const sf::Color GOLD_VEIN_BG = sf::Color(85, 75, 52);             // Earth with gold traces
const sf::Color GOLD_VEIN_GLEAM = sf::Color(165, 142, 78);        // Muted gold gleam
const sf::Color GOLD_SPARKLE = sf::Color(198, 168, 92);           // Occasional sparkle

// Silver - Refined and subtle  
const sf::Color SILVER_LODE_BG = sf::Color(82, 82, 78);           // Silvery gray earth
const sf::Color SILVER_LODE_GLEAM = sf::Color(145, 145, 138);     // Silver gleam
const sf::Color SILVER_SPARKLE = sf::Color(175, 175, 168);        // Silver highlights

// Iron - Practical and common
const sf::Color IRON_ORE_BG = sf::Color(72, 62, 55);              // Rusty earth
const sf::Color IRON_ORE_METAL = sf::Color(95, 88, 82);           // Metallic tinge
const sf::Color IRON_RUST_HINT = sf::Color(118, 82, 65);          // Slight rust color

// Copper - Warm and distinctive
const sf::Color COPPER_DEPOSIT_BG = sf::Color(78, 68, 58);        // Coppery earth
const sf::Color COPPER_DEPOSIT_GLEAM = sf::Color(145, 112, 88);   // Copper gleam
const sf::Color COPPER_PATINA = sf::Color(98, 128, 108);          // Green patina hints

// ===== WIND ANIMATION CHARACTERS =====
const char GRASS_UPRIGHT = '|';                                   // Still grass
const char GRASS_LEAN_RIGHT = '/';                                // Wind from left
const char GRASS_LEAN_LEFT = '\\';                               // Wind from right
const char GRASS_BENT_STRONG = '_';                               // Strong wind
const char GRASS_WAVE_CREST = '^';                                // Wave peak
const char GRASS_WAVE_TROUGH = 'v';                               // Wave valley

// ===== TREE CHARACTERS (ASCII SAFE) =====
const char ANCIENT_OAK = 'T';                                     // Majestic ancient oak
const char NOBLE_PINE = 'A';                                      // Tall noble pine  
const char SILVER_BIRCH = '!';                                    // Slender birch
const char WEEPING_WILLOW = 'W';                                  // Drooping willow
const char YOUNG_TREE = 'Y';                                      // Smaller trees
const char TREE_GROVE = '%';                                      // Tree cluster

// ===== VEGETATION CHARACTERS (ASCII SAFE) =====
const char DENSE_THICKET = '#';                                   // Impenetrable undergrowth
const char BERRY_BUSH = 'o';                                      // Berry bushes
const char WILD_ROSES = '@';                                      // Rose bushes
const char WILDFLOWERS = '*';                                     // Scattered flowers
const char ROYAL_LILY = '&';                                      // Elegant lilies
const char HERB_PATCH = '~';                                      // Useful herbs

// ===== ROCK & RESOURCE CHARACTERS (ASCII SAFE) =====
const char MOSSY_BOULDER = 'O';                                   // Large boulder
const char STANDING_STONE = 'I';                                  // Ancient monolith
const char ROCK_OUTCROP = '.';                                    // Small rocks
const char GOLD_VEIN = '$';                                       // Gold deposit
const char SILVER_LODE = '=';                                     // Silver deposit  
const char IRON_ORE = '#';                                        // Iron ore
const char COPPER_DEPOSIT = '+';                                  // Copper deposit

} // namespace Colors
} // namespace Vegetation
} // namespace Systems
} // namespace World