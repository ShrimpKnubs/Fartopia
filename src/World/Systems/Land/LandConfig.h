// File: EmergentKingdoms/src/World/Systems/Land/LandConfig.h
#pragma once
#include "../../../Core/BaseConfig.h"

namespace World {
namespace Systems {
namespace Land {

// ===== MARSH CONFIGURATION =====
const float MARSH_MAX_HEIGHT = Core::TERRAIN_PLAINS_LOW - 0.01f; 
const float MARSH_WATER_COVERAGE_CHANCE = 0.3f; 

// ===== PLATEAU DEFINITION =====
const float PLATEAU_MIN_HEIGHT = Core::TERRAIN_UPLANDS_LOW; 
const float PLATEAU_MAX_SLOPE = Core::SLOPE_THRESHOLD_GENTLE * 1.5f; 

// ===== MOOR DEFINITION =====
const float MOOR_MIN_HEIGHT = Core::TERRAIN_ROLLING_HILLS_HIGH; 
const float MOOR_MAX_HEIGHT = Core::TERRAIN_UPLANDS_HIGH;
const float MOOR_MAX_SLOPE = Core::SLOPE_THRESHOLD_MODERATE;

// ===== SHORELINE EFFECTS =====
const int SHORELINE_MAX_DISTANCE = 6;

} // namespace Land
} // namespace Systems
} // namespace World