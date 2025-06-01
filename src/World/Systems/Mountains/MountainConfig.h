// File: EmergentKingdoms/src/World/Systems/Mountains/MountainConfig.h
#pragma once
#include "../../../Core/BaseConfig.h"

namespace World {
namespace Systems {
namespace Mountains {

// ===== MOUNTAIN GENERATION =====
const float MOUNTAIN_MASSIF_RADIUS_FACTOR = 0.75f; 
const float MOUNTAIN_MASSIF_FALLOFF_STEEPNESS = 2.5f; 
const float MOUNTAIN_RANGE_NOISE_FREQUENCY = Core::BASE_NOISE_FREQUENCY * 2.8f; 
const int MOUNTAIN_RANGE_NOISE_OCTAVES = 5;      
const float MOUNTAIN_RANGE_NOISE_LACUNARITY = 2.1f; 
const float MOUNTAIN_RANGE_NOISE_GAIN = 0.45f;      
const float MOUNTAIN_RANGE_THRESHOLD_MIN = 0.45f;   
const float MOUNTAIN_RANGE_BASE_HEIGHT_MIN = Core::TERRAIN_ROLLING_HILLS_HIGH; 
const float MOUNTAIN_RANGE_PEAK_HEIGHT_MAX = 0.995f; 
const float MOUNTAIN_DETAIL_NOISE_FREQUENCY = Core::BASE_NOISE_FREQUENCY * 20.0f;
const int MOUNTAIN_DETAIL_NOISE_OCTAVES = 4;
const float MOUNTAIN_DETAIL_NOISE_STRENGTH = 0.07f; 

// ===== SNOWLINE ===== 
const float SNOWLINE_MIN_HEIGHT = Core::TERRAIN_MOUNTAIN_HIGH * 0.95f;

} // namespace Mountains
} // namespace Systems
} // namespace World