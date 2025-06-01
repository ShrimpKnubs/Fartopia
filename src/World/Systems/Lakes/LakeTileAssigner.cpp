// File: EmergentKingdoms/src/World/Systems/Lakes/LakeTileAssigner.cpp
#include "LakeTileAssigner.h"
#include "../../Map.h"
#include "../../Tile.h"
#include "../../GenerationSteps/WorldGenUtils.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <queue>
#include <vector>
#include <cmath>
#include <omp.h>

namespace World {
namespace Systems {
namespace Lakes {

LakeTileAssigner::LakeTileAssigner() {
    water_level_lake_max_height = WATER_LEVEL_LAKE_MAX;
    pond_max_surface_height = POND_MAX_SURFACE_HEIGHT;
    
    // Enhanced noise generators for masterpiece flowing waves
    wave_strand_noise_generator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    wave_strand_noise_generator.SetFrequency(WAVE_FLOW_NOISE_FREQ);
    wave_strand_noise_generator.SetFractalType(FastNoiseLite::FractalType_FBm);
    wave_strand_noise_generator.SetFractalOctaves(2);

    // Smooth animation phase noise for natural wave flow variation
    animation_phase_noise_generator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    animation_phase_noise_generator.SetFrequency(WAVE_TEXTURE_NOISE_FREQ);
    animation_phase_noise_generator.SetFractalType(FastNoiseLite::FractalType_FBm);
    animation_phase_noise_generator.SetFractalOctaves(1);
}

void LakeTileAssigner::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    unsigned int lake_assigner_seed = base_world_seed + static_cast<unsigned int>(step_seed_offset);
    std::mt19937 rng(lake_assigner_seed);
    std::uniform_real_distribution<float> dist_0_1(0.0f, 1.0f);

    wave_strand_noise_generator.SetSeed(static_cast<int>(lake_assigner_seed + 100)); 
    animation_phase_noise_generator.SetSeed(static_cast<int>(lake_assigner_seed + 200));

    std::cout << "  Lakes: Assigning lake tile types with professional wave animation..." << std::endl;
    
    const size_t map_total_size = static_cast<size_t>(world_data.map_width) * world_data.map_height;
    std::vector<int> temp_distance_to_land(map_total_size, -1);
    
    // 1. Adjust heights for lake/pond tiles and determine types
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            if (world_data.is_lake_tile[index]) {
                float h = world_data.heightmap_data[index];
                
                // FIXED: Removed unused lake_type variable - just adjust heights here
                // Tile creation happens later after distance calculations
                if (h < pond_max_surface_height && h < water_level_lake_max_height * 0.6f) {
                    world_data.heightmap_data[index] = std::min(h, pond_max_surface_height - 0.001f);
                } else {
                    world_data.heightmap_data[index] = std::min(h, water_level_lake_max_height - 0.005f);
                }
            }
        }
    }

    // 2. Calculate distance_to_land for LAKE_WATER tiles (BFS)
    std::cout << "  Lakes: Calculating distance to land for wave effects..." << std::endl;
    std::queue<std::pair<std::pair<int,int>, int>> dist_q; 
    
    for (int y = 0; y < world_data.map_height; ++y) { 
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t current_idx = static_cast<size_t>(y) * world_data.map_width + x;
            if (world_data.is_lake_tile[current_idx]) {
                bool is_truly_shore = false;
                for (int dy = -1; dy <= 1; ++dy) { 
                    for (int dx = -1; dx <= 1; ++dx) { 
                        if (dx == 0 && dy == 0) continue; 
                        int ny = y + dy; 
                        int nx = (x + dx + world_data.map_width) % world_data.map_width; 
                        if (ny >= 0 && ny < world_data.map_height) { 
                            size_t neighbor_idx = static_cast<size_t>(ny) * world_data.map_width + nx; 
                            if (!world_data.is_lake_tile[neighbor_idx] && 
                                !world_data.is_river_tile[neighbor_idx]) { 
                                is_truly_shore = true; break; 
                            } 
                        } else { 
                            is_truly_shore = true; break; 
                        } 
                    } 
                    if (is_truly_shore) break; 
                }
                if (is_truly_shore) { 
                    temp_distance_to_land[current_idx] = 0; 
                    dist_q.push({{x, y}, 0}); 
                }
            }
        }
    }
    
    while(!dist_q.empty()){ 
        std::pair<int,int> pos = dist_q.front().first; 
        int dist = dist_q.front().second; 
        dist_q.pop(); 
        if (dist >= WAVE_MAX_DISTANCE_FROM_SHORE) continue; 
        
        for (int dy = -1; dy <= 1; ++dy) { 
            for (int dx = -1; dx <= 1; ++dx) { 
                if (abs(dx) == abs(dy)) continue; // Only cardinal neighbors
                int ny = pos.second + dy; 
                int nx = (pos.first + dx + world_data.map_width) % world_data.map_width; 
                if (ny >= 0 && ny < world_data.map_height) { 
                    size_t neighbor_idx = static_cast<size_t>(ny) * world_data.map_width + nx; 
                    if (world_data.is_lake_tile[neighbor_idx] && temp_distance_to_land[neighbor_idx] == -1) { 
                        temp_distance_to_land[neighbor_idx] = dist + 1; 
                        dist_q.push({{nx, ny}, dist + 1}); 
                    } 
                } 
            } 
        }
    }

    // 3. Identify lake bodies and their sizes for conditional waves
    std::cout << "  Lakes: Identifying lake bodies for wave animation..." << std::endl;
    std::vector<bool> visited_lake_bfs(map_total_size, false);

    for (int y_start_lake = 0; y_start_lake < world_data.map_height; ++y_start_lake) {
        for (int x_start_lake = 0; x_start_lake < world_data.map_width; ++x_start_lake) {
            size_t start_idx = static_cast<size_t>(y_start_lake) * world_data.map_width + x_start_lake;
            if (world_data.is_lake_tile[start_idx] && !visited_lake_bfs[start_idx]) {
                std::vector<size_t> current_lake_body_indices;
                std::queue<std::pair<int, int>> lake_bfs_q;

                lake_bfs_q.push({x_start_lake, y_start_lake});
                visited_lake_bfs[start_idx] = true;
                unsigned int current_lake_size = 0;

                while(!lake_bfs_q.empty()) {
                    std::pair<int,int> curr = lake_bfs_q.front();
                    lake_bfs_q.pop();
                    size_t current_bfs_idx = static_cast<size_t>(curr.second) * world_data.map_width + curr.first;
                    
                    current_lake_body_indices.push_back(current_bfs_idx);
                    current_lake_size++;

                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            if (dx == 0 && dy == 0) continue;
                            int nx = curr.first + dx;
                            int ny = curr.second + dy;
                            int nx_wrapped = (nx + world_data.map_width) % world_data.map_width;

                            if (ny >= 0 && ny < world_data.map_height) {
                                size_t neighbor_idx = static_cast<size_t>(ny) * world_data.map_width + nx_wrapped;
                                if (world_data.is_lake_tile[neighbor_idx] && !visited_lake_bfs[neighbor_idx]) {
                                    visited_lake_bfs[neighbor_idx] = true;
                                    lake_bfs_q.push({nx_wrapped, ny});
                                }
                            }
                        }
                    }
                }
                
                // Post-BFS for this lake body
                bool this_lake_gets_waves = (current_lake_size >= LAKE_MIN_SIZE_FOR_WAVES);
                for (size_t lake_tile_idx : current_lake_body_indices) {
                    world_data.lake_has_waves_map[lake_tile_idx] = this_lake_gets_waves;
                }
            }
        }
    }

    // 4. Create final lake/pond tiles with professional animation
    std::cout << "  Lakes: Creating final tiles with wave animation..." << std::endl;
    #pragma omp parallel for
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            if (world_data.is_lake_tile[index]) {
                float h = world_data.heightmap_data[index];
                BaseTileType lake_type;
                
                if (h < pond_max_surface_height && h < water_level_lake_max_height * 0.6f) {
                    lake_type = BaseTileType::POND_WATER;
                } else {
                    lake_type = BaseTileType::LAKE_WATER;
                }
                
                // Professional flowing wave animation offset calculation
                float flow_noise = Generation::Utils::getCylindricalWrappedNoise(animation_phase_noise_generator, 
                                                                          static_cast<float>(x), 
                                                                          static_cast<float>(y), 
                                                                          static_cast<float>(world_data.map_width));
                float anim_offset = (flow_noise + 1.0f) / 2.0f; // Normalize to 0-1
                
                // Add gentle shore-distance variation for wave flow
                if (temp_distance_to_land[index] >= 0) {
                    float distance_flow = static_cast<float>(temp_distance_to_land[index]) * WAVE_FREQUENCY;
                    anim_offset = std::fmod(anim_offset + distance_flow, 1.0f);
                }
                
                // Enhanced strand intensity for masterpiece flowing waves
                float strand_intensity = 0.0f;
                if (lake_type == BaseTileType::LAKE_WATER) { 
                    bool lake_tile_gets_waves = world_data.lake_has_waves_map[index];
                    if (lake_tile_gets_waves && temp_distance_to_land[index] >= 0 && 
                        temp_distance_to_land[index] < WAVE_MAX_DISTANCE_FROM_SHORE) {
                        
                        // Use flowing wave noise for natural wave distribution
                        float flow_intensity = Generation::Utils::getCylindricalWrappedNoise(wave_strand_noise_generator, 
                                                                                     static_cast<float>(x), 
                                                                                     static_cast<float>(y), 
                                                                                     static_cast<float>(world_data.map_width));
                        strand_intensity = (flow_intensity + 1.0f) / 2.0f; 
                        
                        // Apply natural wave strength based on distance to shore
                        float shore_factor = static_cast<float>(temp_distance_to_land[index]) / static_cast<float>(WAVE_MAX_DISTANCE_FROM_SHORE);
                        float wave_strength = 1.0f - (shore_factor * shore_factor * WAVE_DAMPING);
                        strand_intensity *= std::max(0.1f, wave_strength);
                        
                        // Ensure clean wave patterns with proper threshold
                        if (strand_intensity < 0.2f) {
                            strand_intensity = 0.0f;
                        }
                    }
                }
                
                World::SlopeAspect aspect = (index < world_data.aspect_map.size()) ? 
                                          world_data.aspect_map[index] : World::SlopeAspect::FLAT;
                
                // FIXED: Use getTilesRef() instead of accessing tiles directly
                world_data.map_context->getTilesRef()[index] = Tile::create(
                    lake_type,
                    world_data.heightmap_data[index],
                    world_data.slope_map[index],
                    aspect,
                    temp_distance_to_land[index],
                    -1, // distance_to_water (not applicable for water tiles)
                    anim_offset,
                    strand_intensity,
                    false // is_marsh_water_patch
                );
            }
        }
    }
    
    std::cout << "  Lakes: Finished assigning lake tiles with professional wave animation." << std::endl;
}

} // namespace Lakes
} // namespace Systems
} // namespace World