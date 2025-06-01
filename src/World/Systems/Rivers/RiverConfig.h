// File: EmergentKingdoms/src/World/Systems/Rivers/RiverConfig.h
#pragma once
#include "../../../Core/BaseConfig.h"

namespace World {
namespace Systems {
namespace Rivers {

// ===== RIVER NETWORK CONFIGURATION =====
const int RIVER_NETWORK_NUM_SOURCES = Core::MAP_WIDTH / 80; 
const int RIVER_MAX_LENGTH = Core::MAP_WIDTH + Core::MAP_HEIGHT; 
const int RIVER_MAX_STAGNATION_CHECKS = 10;
const float RIVER_MIN_ABSOLUTE_GRADIENT = 0.0000005f;
const float RIVER_INITIAL_VOLUME = 1.0f;
const float RIVER_VOLUME_INCREASE_PER_STEP = 0.05f;
const float RIVER_MAX_VOLUME = 300.0f;
const float RIVER_CARVE_STRENGTH_BASE = 0.0003f; 
const float RIVER_CARVE_VOLUME_SCALING = 0.0007f;
const float RIVER_START_MIN_ELEVATION = Core::TERRAIN_ROLLING_HILLS_LOW; 
const float RIVER_START_MAX_ELEVATION = Core::TERRAIN_MOUNTAIN_MID; 
const int RIVER_WIDTH_TILES = 3;

// ===== RIVER HEIGHT DEFINITIONS =====
const float TERRAIN_RIVER_BED = 0.05f;

} // namespace Rivers
} // namespace Systems
} // namespace World