// File: EmergentKingdoms/src/World/Systems/Lakes/LakeConfig.h
#pragma once
#include "../../../Core/BaseConfig.h"

namespace World {
namespace Systems {
namespace Lakes {

// ===== LAKE FORMATION =====
const float WATER_LEVEL_LAKE_MAX = Core::TERRAIN_PLAINS_HIGH + 0.01f; 
const float LAKE_MIN_EFFECTIVE_DEPTH = 0.01f; 
const unsigned int LAKE_MIN_SIZE_FOR_WAVES = 800;
const int SHORELINE_MAX_DISTANCE = 6;

// ===== POND CONFIGURATION =====
const float POND_MAX_SURFACE_HEIGHT = Core::TERRAIN_PLAINS_LOW + 0.02f; 
const float POND_MIN_EFFECTIVE_DEPTH = 0.005f;

// ===== MASTERPIECE WAVE ANIMATION SYSTEM ===== 
const int WAVE_MAX_DISTANCE_FROM_SHORE = 25;
const float WAVE_CYCLE_TIME = 12.0f;
const float WAVE_SPEED = 8.0f;
const float WAVE_FREQUENCY = 0.08f;
const int WAVE_SET_COUNT = 3;

// Wave breaking parameters  
const float WAVE_BREAK_DISTANCE = 6.0f;
const float WAVE_FOAM_DISTANCE = 3.0f;
const float WAVE_CREST_HEIGHT = 0.8f;
const float WAVE_TROUGH_DEPTH = 0.3f;

// Flow and timing parameters
const float WAVE_DAMPING = 0.15f;
const float WAVE_ACCELERATION = 1.2f;
const float WAVE_VARIATION = 0.3f;
const float TIDE_CYCLE_TIME = 45.0f;

// Smooth noise parameters for natural flow
const float WAVE_FLOW_NOISE_FREQ = 0.005f;
const float WAVE_TEXTURE_NOISE_FREQ = 0.02f;

} // namespace Lakes
} // namespace Systems
} // namespace World