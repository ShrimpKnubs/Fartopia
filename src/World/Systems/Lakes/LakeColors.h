// File: EmergentKingdoms/src/World/Systems/Lakes/LakeColors.h
#pragma once
#include <SFML/Graphics.hpp>

namespace World {
namespace Systems {
namespace Lakes {
namespace Colors {

// ===== TASTEFUL MEDIEVAL LAKE COLORS WITH SUBTLE GREEN TINTS =====
const sf::Color WATER_DEEP_MYSTICAL = sf::Color(18, 32, 45);      // Deep mysterious waters
const sf::Color WATER_DEEP_LAKE_CORE = sf::Color(22, 38, 52);     // Core lake depths
const sf::Color WATER_MID_LAKE = sf::Color(28, 48, 62);           // Mid-depth with green hint
const sf::Color WATER_SHALLOW_LAKE = sf::Color(38, 58, 72);       // Shallow areas
const sf::Color WATER_SHORE_LAKE = sf::Color(48, 68, 82);         // Near shoreline
const sf::Color WATER_LAKE_SHALLOWS = sf::Color(58, 78, 92);      // Very shallow waters

// Legacy lake colors (updated for consistency)
const sf::Color WATER_LAKE = sf::Color(25, 45, 65);               // Standard lake water
const sf::Color WATER_LAKE_SHORE = sf::Color(55, 75, 95);         // Lake shore water

// ===== TASTEFUL POND COLORS =====
const sf::Color POND_WATER_BG = sf::Color(42, 62, 78);            // Small pond water
const sf::Color POND_WATER_FG = sf::Color(68, 88, 105);           // Pond highlights

// ===== REFINED WAVE ANIMATION COLORS =====
const sf::Color WAVE_FOAM_PRIMARY = sf::Color(235, 235, 235);     // Softer foam
const sf::Color WAVE_FOAM_PRIMARY_BG = sf::Color(185, 205, 225);  // Muted foam background
const sf::Color WAVE_FOAM_SECONDARY = sf::Color(218, 228, 238);   // Secondary foam
const sf::Color WAVE_FOAM_SECONDARY_BG = sf::Color(165, 185, 205); // Secondary background
const sf::Color WAVE_SPARKLE_FG = sf::Color(245, 245, 245);       // Gentle sparkles
const sf::Color WAVE_SPARKLE_BG = sf::Color(95, 125, 155);        // Sparkle background
const sf::Color WAVE_CREST_FG = sf::Color(195, 205, 218);         // Wave crests
const sf::Color WAVE_CREST_BG = sf::Color(68, 98, 128);           // Wave background

// ===== WAVE CHARACTER CONSTANTS =====
const char WAVE_FOAM_HEAVY = '*';
const char WAVE_FOAM_MEDIUM = 'o';
const char WAVE_FOAM_LIGHT = '.';
const char WAVE_CREST = '^';
const char WAVE_SWELL = '~';
const char WAVE_TROUGH = '-';
const char WAVE_RIPPLE = '\'';
const char WATER_CALM = '=';
const char WATER_DEEP = '~';
const char SHORE_WATER_CHAR = '-';
const char DEEP_WATER_CHAR = '~';
const char CALM_WATER_CHAR = '=';

} // namespace Colors
} // namespace Lakes
} // namespace Systems
} // namespace World