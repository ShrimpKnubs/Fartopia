// File: EmergentKingdoms/src/World/Systems/Rivers/RiverNetworkSimulator.cpp
#include "RiverNetworkSimulator.h"
#include "../../../Core/BaseConfig.h"
#include "../../GenerationSteps/WorldGenUtils.h"
#include <iostream>
#include <vector>
#include <random>
#include <set>
#include <algorithm>
#include <cmath>

namespace World {
namespace Systems {
namespace Rivers {

RiverNetworkSimulator::RiverNetworkSimulator() {
    num_sources_config = RIVER_NETWORK_NUM_SOURCES;
    river_start_min_elevation = RIVER_START_MIN_ELEVATION;
    river_start_max_elevation = RIVER_START_MAX_ELEVATION;
    river_max_length = RIVER_MAX_LENGTH;
    river_initial_volume = RIVER_INITIAL_VOLUME;
    river_width_tiles = RIVER_WIDTH_TILES;
    river_carve_strength_base = RIVER_CARVE_STRENGTH_BASE;
    river_carve_volume_scaling = RIVER_CARVE_VOLUME_SCALING;
    river_min_absolute_gradient = RIVER_MIN_ABSOLUTE_GRADIENT;
    river_max_stagnation_checks = RIVER_MAX_STAGNATION_CHECKS;
    river_volume_increase_per_step = RIVER_VOLUME_INCREASE_PER_STEP;
    river_max_volume = RIVER_MAX_VOLUME;
    terrain_river_bed_height = TERRAIN_RIVER_BED;
}

void RiverNetworkSimulator::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    unsigned int current_step_seed = base_world_seed + static_cast<unsigned int>(step_seed_offset);
    std::cout << "  Rivers: Simulating river networks..." << std::endl;

    std::mt19937 rng(current_step_seed);
    std::uniform_int_distribution<int> x_dist(0, world_data.map_width - 1);
    std::uniform_int_distribution<int> y_dist(0, world_data.map_height - 1);
    const int dx8[] = {0, 1, 1, 1, 0, -1, -1, -1};
    const int dy8[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int total_start_attempts = 0;
    const int MAX_TOTAL_START_ATTEMPTS = num_sources_config * std::max(200, world_data.map_width / 5);

    int print_frequency_rivers = std::max(1, num_sources_config / 10);
    if (num_sources_config < 10) print_frequency_rivers = 1;

    for (int river_idx = 0; river_idx < num_sources_config; ++river_idx) {
        if (river_idx > 0 && river_idx % print_frequency_rivers == 0) {
            std::cout << "    Rivers: Simulating river " << river_idx << "/" << num_sources_config 
                      << " (attempts: " << total_start_attempts << ")" << std::endl;
        }
        total_start_attempts++;
        if (total_start_attempts > MAX_TOTAL_START_ATTEMPTS) {
            std::cerr << "    Rivers: Warning: Exceeded max attempts (" << MAX_TOTAL_START_ATTEMPTS 
                      << ") to find river starting points. Generated " << river_idx << " rivers." << std::endl;
            break;
        }

        int start_x = x_dist(rng);
        int start_y = y_dist(rng);
        size_t start_map_idx = static_cast<size_t>(start_y) * world_data.map_width + start_x;

        bool start_on_valid_slope = true;
        if (!world_data.slope_map.empty() && start_map_idx < world_data.slope_map.size()) {
            start_on_valid_slope = world_data.slope_map[start_map_idx] > 0.001f; 
        }

        if (world_data.heightmap_data[start_map_idx] < river_start_min_elevation ||
            world_data.heightmap_data[start_map_idx] > river_start_max_elevation ||
            world_data.is_river_tile[start_map_idx] || world_data.is_lake_tile[start_map_idx] || 
            !start_on_valid_slope) {
            river_idx--; 
            continue;
        }

        int current_x_abs = start_x;
        int current_y_abs = start_y;
        float river_volume = river_initial_volume;
        int stagnation_counter = 0;
        std::set<size_t> visited_on_this_path;

        for (int len = 0; len < river_max_length; ++len) {
            int wrapped_x = (current_x_abs % world_data.map_width + world_data.map_width) % world_data.map_width;
            size_t current_map_idx = static_cast<size_t>(current_y_abs) * world_data.map_width + wrapped_x;

            if (visited_on_this_path.count(current_map_idx)) break;
            visited_on_this_path.insert(current_map_idx);

            if (world_data.is_lake_tile[current_map_idx]) break; 

            for (int w = -river_width_tiles / 2; w <= river_width_tiles / 2; ++w) {
                int river_part_x_abs = current_x_abs + w;
                int river_part_x_wrapped = (river_part_x_abs % world_data.map_width + world_data.map_width) % world_data.map_width;
                size_t river_part_idx = static_cast<size_t>(current_y_abs) * world_data.map_width + river_part_x_wrapped;
                world_data.is_river_tile[river_part_idx] = true;

                float carve_strength = river_carve_strength_base + (river_volume * river_carve_volume_scaling);
                if (w == 0) carve_strength *= 1.5f; 
                else carve_strength *= 0.7f;
                carve_strength = std::min(carve_strength, 0.05f); // Max carve depth per step
                world_data.heightmap_data[river_part_idx] = std::max(world_data.heightmap_data[river_part_idx] - carve_strength, 0.0f);
            }

            float original_h_at_step = world_data.heightmap_data[current_map_idx];
            int best_next_x_abs = -1, best_next_y_abs = -1;
            float lowest_neighbor_h = original_h_at_step;

            for (int j = 0; j < 8; ++j) {
                int nx_abs = current_x_abs + dx8[j];
                int ny_abs_neighbor = current_y_abs + dy8[j];

                if (ny_abs_neighbor < 0 || ny_abs_neighbor >= world_data.map_height) continue;

                int nx_wrapped_neighbor = (nx_abs % world_data.map_width + world_data.map_width) % world_data.map_width;
                size_t neighbor_idx = static_cast<size_t>(ny_abs_neighbor) * world_data.map_width + nx_wrapped_neighbor;

                if (world_data.heightmap_data[neighbor_idx] < lowest_neighbor_h) {
                    if (visited_on_this_path.count(neighbor_idx) && 
                        world_data.heightmap_data[neighbor_idx] >= original_h_at_step - river_min_absolute_gradient * 5.0f) {
                        continue;
                    }
                    lowest_neighbor_h = world_data.heightmap_data[neighbor_idx];
                    best_next_x_abs = nx_abs;
                    best_next_y_abs = ny_abs_neighbor;
                }
            }

            if (best_next_x_abs == -1 || lowest_neighbor_h >= original_h_at_step - river_min_absolute_gradient) {
                stagnation_counter++;
                if (stagnation_counter >= river_max_stagnation_checks) break;
                 if (best_next_x_abs == -1 || lowest_neighbor_h >= original_h_at_step) break;
            } else {
                stagnation_counter = 0;
                current_x_abs = best_next_x_abs;
                current_y_abs = best_next_y_abs;
            }
            
            if (best_next_x_abs == -1 && best_next_y_abs == -1) break;

            int next_tile_x_wrapped = (current_x_abs % world_data.map_width + world_data.map_width) % world_data.map_width;
            size_t next_tile_idx = static_cast<size_t>(current_y_abs) * world_data.map_width + next_tile_x_wrapped;

            if (world_data.heightmap_data[next_tile_idx] < terrain_river_bed_height + 0.001f && 
                !world_data.is_lake_tile[next_tile_idx]) {
                world_data.is_river_tile[next_tile_idx] = true; 
                break;
            }
            river_volume = std::min(river_volume + river_volume_increase_per_step, river_max_volume);
        }
    }
    std::cout << "    Rivers: Finished simulating rivers. Total attempts for sources: " << total_start_attempts << std::endl;
}

} // namespace Rivers
} // namespace Systems
} // namespace World