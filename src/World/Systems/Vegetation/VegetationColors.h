// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/VegetationColors.h
#pragma once
#include <SFML/Graphics.hpp>

namespace World {
namespace Systems {
namespace Vegetation {
namespace Colors {

// ===== MAGNIFICENT MEDIEVAL TREE COLORS FOR TOP-DOWN VIEW =====
const sf::Color ANCIENT_OAK_TRUNK = sf::Color(72, 55, 40);        // Rich weathered brown
const sf::Color ANCIENT_OAK_CANOPY = sf::Color(55, 75, 35);       // Lush forest green
const sf::Color NOBLE_PINE_TRUNK = sf::Color(68, 52, 38);         // Dark conifer bark
const sf::Color NOBLE_PINE_NEEDLES = sf::Color(45, 65, 40);       // Deep evergreen
const sf::Color SILVER_BIRCH_BARK = sf::Color(195, 185, 172);     // Bright silvery bark
const sf::Color SILVER_BIRCH_LEAVES = sf::Color(78, 95, 52);      // Fresh green leaves
const sf::Color WEEPING_WILLOW_TRUNK = sf::Color(82, 68, 55);     // Gentle gray-brown
const sf::Color WEEPING_WILLOW_FRONDS = sf::Color(92, 108, 65);   // Flowing green

// ===== DENSE BUSH & UNDERGROWTH COLORS =====
const sf::Color DENSE_THICKET_BG = sf::Color(35, 45, 25);         // Deep forest undergrowth
const sf::Color DENSE_THICKET_FG = sf::Color(55, 75, 42);         // Thick foliage
const sf::Color BERRY_BUSH_BG = sf::Color(48, 62, 38);            // Rich berry bush base
const sf::Color BERRY_BUSH_BERRIES = sf::Color(105, 42, 55);      // Vibrant berries
const sf::Color WILD_ROSE_BG = sf::Color(52, 68, 45);             // Thorny green base
const sf::Color WILD_ROSE_BLOOM = sf::Color(145, 88, 95);         // Beautiful pink roses
const sf::Color FOREST_UNDERGROWTH = sf::Color(42, 55, 32);       // Rich forest floor

// ===== FLOWING GRASS COLORS (LUSH & VIBRANT) =====
const sf::Color MEADOW_GRASS_BASE = sf::Color(68, 88, 52);        // Rich meadow base
const sf::Color MEADOW_GRASS_WAVE = sf::Color(82, 105, 65);       // Flowing grass waves
const sf::Color MEADOW_GRASS_WIND = sf::Color(95, 120, 78);       // Wind-swept highlights
const sf::Color PLAIN_GRASS_BASE = sf::Color(62, 82, 48);         // Plains grass base
const sf::Color PLAIN_GRASS_SWAY = sf::Color(78, 98, 62);         // Swaying motion
const sf::Color HIGHLAND_GRASS = sf::Color(58, 78, 45);           // Mountain grass

// ===== BEAUTIFUL FLOWER & HERB COLORS =====
const sf::Color WILDFLOWER_MEADOW_BG = sf::Color(65, 85, 55);     // Flower meadow base
const sf::Color ROYAL_LILY_WHITE = sf::Color(252, 248, 235);      // Pristine white lilies
const sf::Color ROYAL_LILY_GOLD = sf::Color(208, 172, 95);        // Golden lily centers
const sf::Color FOREST_VIOLET = sf::Color(108, 75, 138);          // Deep purple violets
const sf::Color MEADOW_BUTTERCUP = sf::Color(195, 165, 72);       // Golden buttercups
const sf::Color HERB_PATCH_GREEN = sf::Color(72, 92, 58);         // Rich herb foliage

// ===== MAGNIFICENT ROCK & BOULDER COLORS =====
const sf::Color MOSSY_BOULDER_BASE = sf::Color(88, 82, 75);       // Rich stone base
const sf::Color MOSSY_BOULDER_MOSS = sf::Color(52, 68, 45);       // Lush moss covering
const sf::Color STANDING_STONE_ANCIENT = sf::Color(98, 92, 85);   // Ancient weathered stone
const sf::Color ROCK_OUTCROP_GRAY = sf::Color(115, 108, 102);     // Natural rock formation
const sf::Color GRANITE_BOULDER = sf::Color(122, 118, 112);       // Solid granite

// ===== PRECIOUS MEDIEVAL RESOURCE COLORS =====
// Gold - Gleaming and beautiful
const sf::Color GOLD_VEIN_BG = sf::Color(95, 85, 62);             // Rich gold-bearing earth
const sf::Color GOLD_VEIN_GLEAM = sf::Color(185, 152, 88);        // Beautiful gold gleam
const sf::Color GOLD_SPARKLE = sf::Color(218, 178, 102);          // Brilliant sparkle

// Silver - Lustrous and elegant  
const sf::Color SILVER_LODE_BG = sf::Color(92, 92, 88);           // Silver-bearing stone
const sf::Color SILVER_LODE_GLEAM = sf::Color(165, 165, 158);     // Silver luster
const sf::Color SILVER_SPARKLE = sf::Color(195, 195, 188);        // Silver brilliance

// Iron - Strong and practical
const sf::Color IRON_ORE_BG = sf::Color(82, 72, 65);              // Iron-rich earth
const sf::Color IRON_ORE_METAL = sf::Color(105, 98, 92);          // Metallic iron
const sf::Color IRON_RUST_HINT = sf::Color(128, 92, 75);          // Rust coloration

// Copper - Warm and rich
const sf::Color COPPER_DEPOSIT_BG = sf::Color(88, 78, 68);        // Copper-bearing stone
const sf::Color COPPER_DEPOSIT_GLEAM = sf::Color(165, 122, 98);   // Copper shine
const sf::Color COPPER_PATINA = sf::Color(108, 138, 118);         // Green patina

// ===== TOP-DOWN WIND ANIMATION CHARACTERS FOR FLOWING GRASS =====
const char GRASS_UPRIGHT = '|';                                   // Still grass
const char GRASS_LEAN_RIGHT = '/';                                // Wind from left
const char GRASS_LEAN_LEFT = '\\';                               // Wind from right
const char GRASS_BENT_STRONG = '_';                               // Strong wind
const char GRASS_WAVE_CREST = '^';                                // Wave peak
const char GRASS_WAVE_TROUGH = 'v';                               // Wave valley

// ===== TOP-DOWN TREE CHARACTERS (What trees look like from above) =====
const char ANCIENT_OAK = 'T';                                     // Large oak canopy from above
const char NOBLE_PINE = 'A';                                      // Coniferous canopy from above  
const char SILVER_BIRCH = '!';                                    // Slender birch canopy from above
const char WEEPING_WILLOW = 'W';                                  // Drooping willow canopy from above
const char YOUNG_TREE = 'Y';                                      // Small tree canopy from above
const char TREE_GROVE = '%';                                      // Multiple tree canopies

// ===== TOP-DOWN VEGETATION CHARACTERS (View from directly above) =====
const char DENSE_THICKET = '#';                                   // Dense undergrowth from above
const char BERRY_BUSH = 'o';                                      // Round berry bush from above
const char WILD_ROSES = '@';                                      // Rose bush cluster from above
const char WILDFLOWERS = '*';                                     // Scattered flower clusters
const char ROYAL_LILY = '&';                                      // Elegant lily patches
const char HERB_PATCH = '~';                                      // Herb garden areas

// ===== TOP-DOWN ROCK & RESOURCE CHARACTERS (Aerial view) =====
const char MOSSY_BOULDER = 'O';                                   // Large boulder from above
const char STANDING_STONE = 'I';                                  // Monolithic stone from above
const char ROCK_OUTCROP = '.';                                    // Small rock formation from above
const char GOLD_VEIN = '$';                                       // Gold vein exposure
const char SILVER_LODE = '=';                                     // Silver lode exposure  
const char IRON_ORE = '#';                                        // Iron ore outcrop
const char COPPER_DEPOSIT = '+';                                  // Copper deposit exposure

} // namespace Colors
} // namespace Vegetation
} // namespace Systems
} // namespace World