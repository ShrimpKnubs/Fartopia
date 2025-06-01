// File: EmergentKingdoms/src/World/Systems/Land/LandColors.h
#pragma once
#include <SFML/Graphics.hpp>

namespace World {
namespace Systems {
namespace Land {
namespace Colors {

// ===== TASTEFUL SHORELINE DIRT COLORS =====
const sf::Color SHORE_WET_DIRT_BG = sf::Color(58, 45, 35);        // Deeper, richer wet soil
const sf::Color SHORE_WET_DIRT_FG = sf::Color(75, 62, 48);
const sf::Color SHORE_DAMP_DIRT_BG = sf::Color(82, 68, 52);       // Subtle earth tones
const sf::Color SHORE_DAMP_DIRT_FG = sf::Color(98, 82, 65);
const sf::Color SHORE_DRY_DIRT_BG = sf::Color(105, 88, 68);       // Warm dry earth
const sf::Color SHORE_DRY_DIRT_FG = sf::Color(125, 108, 85);

// ===== TASTEFUL SPECIALIZED BIOMES =====
const sf::Color MARSH_GRASS_BG = sf::Color(72, 82, 52);           // Muted marsh green
const sf::Color MARSH_WATER_PATCH_BG = sf::Color(58, 68, 62);     // Subtle greenish water
const sf::Color MARSH_REED_FG = sf::Color(95, 108, 68);           // Natural reed color
const sf::Color WHEAT_MARSH_BG = sf::Color(138, 125, 85);         // Golden marsh grass
const sf::Color WHEAT_MARSH_FG = sf::Color(165, 148, 98);         

const sf::Color MOOR_HEATH_BG_DARK = sf::Color(78, 72, 58);       // Rich heather tones
const sf::Color MOOR_HEATH_BG_LIGHT = sf::Color(95, 88, 72);
const sf::Color MOOR_GRASS_PATCH_FG = sf::Color(85, 98, 62);      // Moorland grass

const sf::Color PLATEAU_GRASS_BG = sf::Color(68, 92, 52);         // Highland grass
const sf::Color PLATEAU_ROCK_FG = sf::Color(108, 102, 95);        // Weathered stone

} // namespace Colors
} // namespace Land
} // namespace Systems
} // namespace World