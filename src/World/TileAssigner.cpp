// File: EmergentKingdoms/src/World/TileAssigner.cpp
#include "TileAssigner.h"
#include "Map.h" // ADDED: Include full Map definition for tiles access
#include "Systems/Rivers/RiverTileAssigner.h"
#include "Systems/Lakes/LakeTileAssigner.h"
#include "Systems/Mountains/MountainTileAssigner.h"
#include "Systems/Land/LandTileAssigner.h"
#include "Systems/Vegetation/VegetationTileAssigner.h"  // NEW!
#include "../Core/BaseConfig.h"
#include "../Core/FastNoiseLite.h"
#include "GenerationSteps/WorldGenUtils.h"
#include <iostream>
#include <random>
#include <queue>
#include <omp.h>

namespace World {

TileAssigner::TileAssigner() {
    initializeSystemAssigners();
}

void TileAssigner::initializeSystemAssigners() {
    system_assigners.clear();
    
    // Order matters for delegation - first match wins processing
    system_assigners.push_back(std::make_unique<Systems::Rivers::RiverTileAssigner>());
    system_assigners.push_back(std::make_unique<Systems::Lakes::LakeTileAssigner>());
    system_assigners.push_back(std::make_unique<Systems::Mountains::MountainTileAssigner>());
    system_assigners.push_back(std::make_unique<Systems::Land::LandTileAssigner>());
    system_assigners.push_back(std::make_unique<Systems::Vegetation::VegetationTileAssigner>()); // NEW!
}

void TileAssigner::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    std::cout << "  Modular Tile Assignment: Starting coordinated tile assignment..." << std::endl;
    
    // 1. Perform base tile classification (determines what goes where)
    performBaseTileClassification(world_data, base_world_seed, step_seed_offset);
    
    // 2. Calculate shoreline effects for land tiles
    calculateShorelineEffects(world_data);
    
    // 3. Delegate to system-specific assigners
    int sub_step_offset = 100; // Ensure unique seeds for each system
    for (const auto& assigner : system_assigners) {
        std::cout << "    Delegating to: " << assigner->getName() << std::endl;
        assigner->process(world_data, base_world_seed, step_seed_offset + sub_step_offset);
        sub_step_offset += 100;
    }
    
    std::cout << "  Modular Tile Assignment: Completed coordinated tile assignment." << std::endl;
}

void TileAssigner::performBaseTileClassification(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    unsigned int classification_seed = base_world_seed + static_cast<unsigned int>(step_seed_offset) + 50;
    std::cout << "    Performing base tile classification..." << std::endl;
    
    FastNoiseLite dry_patch_noise;
    dry_patch_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    dry_patch_noise.SetFrequency(0.03f); 
    dry_patch_noise.SetSeed(static_cast<int>(classification_seed));

    // Classify tiles into basic categories based on height, slope, and special conditions
    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            float h = world_data.heightmap_data[index]; 
            float s = world_data.slope_map[index];
            
            BaseTileType determined_base_type = BaseTileType::VOID;
            
            // Priority order: Water > Mountains > Specialized Land > Basic Land
            if (world_data.is_river_tile[index]) { 
                determined_base_type = BaseTileType::RIVER_WATER; 
            } else if (world_data.is_lake_tile[index]) {
                // Lakes will be further refined by LakeTileAssigner
                determined_base_type = BaseTileType::LAKE_WATER;
            } else if (h < Systems::Land::MARSH_MAX_HEIGHT && s < Core::SLOPE_THRESHOLD_GENTLE * 1.3f) { 
                determined_base_type = BaseTileType::MARSH;
            } else if (h >= Systems::Mountains::SNOWLINE_MIN_HEIGHT) { 
                determined_base_type = BaseTileType::MOUNTAIN_PEAK_SNOW;
            } else if (h >= Core::TERRAIN_MOUNTAIN_HIGH) { 
                determined_base_type = BaseTileType::MOUNTAIN_UPPER;
            } else if (h >= Core::TERRAIN_MOUNTAIN_MID) { 
                determined_base_type = BaseTileType::MOUNTAIN_MID;
            } else if (h >= Core::TERRAIN_MOUNTAIN_BASE) { 
                determined_base_type = BaseTileType::MOUNTAIN_LOWER;
            } else if (h >= Systems::Land::PLATEAU_MIN_HEIGHT && h < Core::TERRAIN_MOUNTAIN_BASE && s <= Systems::Land::PLATEAU_MAX_SLOPE) { 
                determined_base_type = BaseTileType::PLATEAU_GRASS;
            } else if (s >= Core::SLOPE_THRESHOLD_STEEP * 1.1f && h > Core::TERRAIN_ROLLING_HILLS_LOW) { 
                determined_base_type = BaseTileType::CLIFF_FACE;
            } else if (h >= Systems::Land::MOOR_MIN_HEIGHT && h <= Systems::Land::MOOR_MAX_HEIGHT && 
                       s <= Systems::Land::MOOR_MAX_SLOPE && s > Core::SLOPE_THRESHOLD_GENTLE * 0.8f) { 
                determined_base_type = BaseTileType::MOOR;
            } else if (h >= Core::TERRAIN_STEEP_SLOPES) { 
                 if (s > Core::SLOPE_THRESHOLD_MODERATE * 1.2f) {  
                    determined_base_type = BaseTileType::ROCKY_SLOPE;
                 } else {
                    determined_base_type = BaseTileType::STEEP_SLOPE; 
                 }
            } else if (h >= Core::TERRAIN_ROLLING_HILLS_LOW) { 
                determined_base_type = BaseTileType::HILLS;
            } else if (h >= Core::TERRAIN_PLAINS_LOW) {
                float dry_noise_val = (Generation::Utils::getCylindricalWrappedNoise(dry_patch_noise, 
                                                                                   static_cast<float>(x), 
                                                                                   static_cast<float>(y), 
                                                                                   static_cast<float>(world_data.map_width)) + 1.0f) / 2.0f;
                if (dry_noise_val > 0.65f && h < Core::TERRAIN_PLAINS_HIGH * 0.7f) { 
                    determined_base_type = BaseTileType::DRY_PLAINS;
                } else {
                    determined_base_type = BaseTileType::PLAINS;
                }
            } else if (h >= Core::TERRAIN_VERY_LOW_LAND) { 
                determined_base_type = BaseTileType::MEADOW;
            } else { 
                determined_base_type = BaseTileType::MEADOW; 
            }
            
            // Store the classification in the tile (temporary storage)
            // Each system assigner will process only tiles of their type
            if (index < world_data.map_context->getTilesRef().size()) {
                world_data.map_context->getTilesRef()[index].base_type = determined_base_type;
            }
        }
    }
}

void TileAssigner::calculateShorelineEffects(WorldData& world_data) {
    std::cout << "    Calculating shoreline effects for land tiles..." << std::endl;
    
    const size_t map_total_size = static_cast<size_t>(world_data.map_width) * world_data.map_height;
    std::vector<int> temp_distance_to_water(map_total_size, -1);
    std::queue<std::pair<std::pair<int,int>, int>> dist_q;

    // Find all land tiles adjacent to water
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t current_idx = static_cast<size_t>(y) * world_data.map_width + x;
            
            // Check if current tile is land
            if (!world_data.is_lake_tile[current_idx] &&
                !world_data.is_river_tile[current_idx]) {
                
                bool is_adj_to_water = false;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        int ny = y + dy;
                        int nx_wrapped = (x + dx + world_data.map_width) % world_data.map_width;
                        if (ny >= 0 && ny < world_data.map_height) {
                            size_t neighbor_idx = static_cast<size_t>(ny) * world_data.map_width + nx_wrapped;
                            if (world_data.is_lake_tile[neighbor_idx]) {
                                is_adj_to_water = true; break;
                            }
                        }
                    }
                    if (is_adj_to_water) break;
                }
                if (is_adj_to_water) {
                    temp_distance_to_water[current_idx] = 0;
                    dist_q.push({{x,y}, 0});
                }
            }
        }
    }

    // BFS to propagate distance from water
    while(!dist_q.empty()){
        std::pair<int,int> pos = dist_q.front().first; 
        int dist = dist_q.front().second; 
        dist_q.pop();
        if (dist >= Systems::Land::SHORELINE_MAX_DISTANCE - 1) continue;

        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (abs(dx) == abs(dy)) continue; // Cardinal neighbors only

                int ny = pos.second + dy;
                int nx_wrapped = (pos.first + dx + world_data.map_width) % world_data.map_width;

                if (ny >= 0 && ny < world_data.map_height) {
                    size_t neighbor_idx = static_cast<size_t>(ny) * world_data.map_width + nx_wrapped;
                    
                    if (!world_data.is_lake_tile[neighbor_idx] &&
                        !world_data.is_river_tile[neighbor_idx] &&
                        temp_distance_to_water[neighbor_idx] == -1) {
                        temp_distance_to_water[neighbor_idx] = dist + 1;
                        dist_q.push({{nx_wrapped, ny}, dist + 1});
                    }
                }
            }
        }
    }

    // Store distance_to_water in tiles for land system to use
    for (size_t i = 0; i < map_total_size; ++i) {
        if (i < world_data.map_context->getTilesRef().size()) {
            world_data.map_context->getTilesRef()[i].distance_to_water = temp_distance_to_water[i];
        }
    }
}

} // namespace World