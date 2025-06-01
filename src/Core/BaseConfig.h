// File: EmergentKingdoms/src/Core/BaseConfig.h
#pragma once
#include <string>
#include <SFML/Graphics.hpp>

namespace Core {

// ===== ENGINE/WINDOW CONFIGURATION =====
const int WINDOW_WIDTH_PX = 1920;
const int WINDOW_HEIGHT_PX = 1080;
const float MINIMAP_OVERLAY_WIDTH_FACTOR = 0.18f;
const float MINIMAP_OVERLAY_HEIGHT_FACTOR = 0.28f;
const float MINIMAP_MARGIN_PX = 10.0f;
const unsigned int INITIAL_SFML_FONT_SIZE = 16;
const int INITIAL_CHAR_CELL_WIDTH_PX = 10;
const int INITIAL_CHAR_CELL_HEIGHT_PX = 10;
const std::string FONT_PATH = "assets/font.ttf";
const float ZOOM_STEP_MULTIPLIER = 1.2f;
const float MIN_ZOOM_FACTOR = 0.02f; 
const float MAX_ZOOM_FACTOR = 2.0f;
const float CAMERA_SCROLL_SPEED_TILES_PER_SEC = 150.0f;
const float LOD_ACTIVATION_ZOOM_FACTOR = 0.10f;
const int LOD_AGGREGATION_SCALE = 5;

// ===== MAP DIMENSIONS =====
const int MAP_WIDTH = 5000;
const int MAP_HEIGHT = 5000;

// ===== GAME TIMING =====
const int TICKS_PER_SECOND = 30;
const int MS_PER_TICK = 1000 / TICKS_PER_SECOND;

// ===== BASE TERRAIN NOISE (Shared by all systems) =====
const float BASE_NOISE_FREQUENCY = 0.0007f; 
const int BASE_NOISE_OCTAVES = 6;        
const float BASE_NOISE_LACUNARITY = 2.0f;
const float BASE_NOISE_PERSISTENCE = 0.5f; 

// ===== CORE TERRAIN HEIGHT DEFINITIONS =====
// These are fundamental heights that multiple systems reference
const float TERRAIN_VERY_LOW_LAND = 0.03f;   
const float TERRAIN_PLAINS_LOW = 0.06f;      
const float TERRAIN_PLAINS_HIGH = 0.25f;     
const float TERRAIN_ROLLING_HILLS_LOW = 0.26f;
const float TERRAIN_ROLLING_HILLS_HIGH = 0.45f; 
const float TERRAIN_UPLANDS_LOW = 0.46f;     
const float TERRAIN_UPLANDS_HIGH = 0.60f;    
const float TERRAIN_STEEP_SLOPES = 0.61f;    
const float TERRAIN_MOUNTAIN_BASE = 0.65f;   
const float TERRAIN_MOUNTAIN_MID = 0.80f;
const float TERRAIN_MOUNTAIN_HIGH = 0.90f;
const float TERRAIN_MOUNTAIN_PEAK_ZONE = 0.95f;

// ===== CORE SLOPE THRESHOLDS =====
const float SLOPE_THRESHOLD_GENTLE = 0.005f;
const float SLOPE_THRESHOLD_MODERATE = 0.015f;
const float SLOPE_THRESHOLD_STEEP = 0.04f; 
const float SLOPE_THRESHOLD_VERY_STEEP = 0.08f;

// ===== EROSION PARAMETERS (Shared) =====
const int THERMAL_EROSION_ITERATIONS = 3;
const float THERMAL_EROSION_TALUS_ANGLE_FACTOR = 0.02f; 
const float THERMAL_EROSION_STRENGTH = 0.015f;
const int HYDRAULIC_EROSION_ITERATIONS = 3;
const float Kr = 0.01f; 
const float Ks = 0.05f; 
const float Ke = 0.3f; 
const float Kd = 0.01f;

// ===== BASIC UI COLORS =====
namespace Colors {
    const sf::Color BLACK = sf::Color::Black;
    const sf::Color WHITE = sf::Color::White;
    const sf::Color RED = sf::Color(220, 50, 50);
    const sf::Color DEFAULT_BG = sf::Color(10, 10, 15);
    const sf::Color BORDER_COLOR_FG = sf::Color(100, 90, 40);
    const sf::Color BORDER_COLOR_BG = sf::Color(50, 45, 20);
}

// ===== TASTEFUL MEDIEVAL LAND COLORS (Richer & Darker) =====
namespace LandColors {
    // Rich, darker earth tones
    const sf::Color EARTH_DARK = sf::Color(75, 58, 42);             // Deep rich soil
    const sf::Color EARTH_MID = sf::Color(98, 78, 58);              // Medium earth tone
    const sf::Color EARTH_LIGHT = sf::Color(125, 102, 78);          // Lighter earth
    
    // Richer, muted grass colors with more depth
    const sf::Color GRASS_DARK_VALLEY = sf::Color(42, 58, 32);      // Deep valley grass
    const sf::Color GRASS_MID_SLOPE = sf::Color(58, 78, 42);        // Mid-slope grass  
    const sf::Color GRASS_LIGHT_HILLTOP = sf::Color(72, 95, 52);    // Hilltop grass
    const sf::Color PLAINS_GRASS_BASE = sf::Color(52, 72, 38);      // Base plains grass
    const sf::Color PLAINS_GRASS_HIGHLIGHT = sf::Color(68, 88, 52); // Plains highlights
    const sf::Color DRY_GRASS_BG = sf::Color(108, 98, 65);          // Dry grass base
    const sf::Color DRY_GRASS_FG = sf::Color(132, 122, 82);         // Dry grass highlights
}

} // namespace Core