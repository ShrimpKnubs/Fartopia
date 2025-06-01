// File: EmergentKingdoms/src/World/Systems/Lakes/LakeFormer.cpp
#include "LakeFormer.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <limits>
#include <queue>

namespace World {
namespace Systems {
namespace Lakes {

LakeFormer::LakeFormer() {
    water_level_lake_max = WATER_LEVEL_LAKE_MAX;
    lake_min_effective_depth = LAKE_MIN_EFFECTIVE_DEPTH;
}

void LakeFormer::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    (void)base_world_seed; 
    (void)step_seed_offset;
    std::cout << "  Lakes: Forming lakes (Max Lake Surface H: " << water_level_lake_max 
              << ", Min Effective Depth: " << lake_min_effective_depth << ")..." << std::endl;

    const size_t map_size = static_cast<size_t>(world_data.map_width) * static_cast<size_t>(world_data.map_height);
    std::vector<bool> globally_visited_cells(map_size, false);
    int lakes_formed_count = 0;

    for (int y_start = 0; y_start < world_data.map_height; ++y_start) {
        for (int x_start = 0; x_start < world_data.map_width; ++x_start) {
            size_t start_idx = static_cast<size_t>(y_start) * world_data.map_width + x_start;

            if (globally_visited_cells[start_idx] || 
                world_data.heightmap_data[start_idx] >= water_level_lake_max || 
                world_data.is_lake_tile[start_idx]) {
                continue;
            }

            std::vector<std::pair<int, int>> basin_cells_this_bfs;
            std::queue<std::pair<int, int>> q;
            std::vector<bool> visited_in_current_bfs(map_size, false);

            q.push({x_start, y_start});
            visited_in_current_bfs[start_idx] = true; 

            float lowest_point_in_basin = std::numeric_limits<float>::max();
            float min_spill_height_on_rim = std::numeric_limits<float>::max();

            while(!q.empty()){
                std::pair<int, int> current_pair = q.front();
                q.pop();
                int current_x = current_pair.first;
                int current_y = current_pair.second;
                size_t current_bfs_idx = static_cast<size_t>(current_y) * world_data.map_width + current_x;

                basin_cells_this_bfs.push_back(current_pair);
                lowest_point_in_basin = std::min(lowest_point_in_basin, world_data.heightmap_data[current_bfs_idx]);

                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;

                        int nx_abs = current_x + dx;
                        int ny_abs = current_y + dy;
                        
                        if (ny_abs < 0 || ny_abs >= world_data.map_height) {
                            min_spill_height_on_rim = std::min(min_spill_height_on_rim, 0.0f);
                            continue; 
                        }
                        int nx_wrapped = (nx_abs + world_data.map_width) % world_data.map_width;
                        size_t neighbor_idx = static_cast<size_t>(ny_abs) * world_data.map_width + nx_wrapped;

                        if (visited_in_current_bfs[neighbor_idx]) continue;

                        bool is_potential_basin_tile = world_data.heightmap_data[neighbor_idx] < water_level_lake_max &&
                                                       !world_data.is_lake_tile[neighbor_idx]; 
                        
                        if (is_potential_basin_tile) {
                            visited_in_current_bfs[neighbor_idx] = true;
                            q.push({nx_wrapped, ny_abs});
                        } else {
                            min_spill_height_on_rim = std::min(min_spill_height_on_rim, world_data.heightmap_data[neighbor_idx]);
                        }
                    }
                }
            }

            // Mark all cells explored in this BFS as globally visited
            for(const auto& p : basin_cells_this_bfs) {
                globally_visited_cells[static_cast<size_t>(p.second) * world_data.map_width + p.first] = true;
            }
            
            if (basin_cells_this_bfs.empty()) continue;

            if (min_spill_height_on_rim > lowest_point_in_basin) {
                float lake_surface_h = std::min(min_spill_height_on_rim, water_level_lake_max);
                float actual_depth = lake_surface_h - lowest_point_in_basin;

                if (actual_depth > lake_min_effective_depth) {
                    int tiles_filled = 0;
                    for (const auto& p : basin_cells_this_bfs) {
                        size_t p_idx = static_cast<size_t>(p.second) * world_data.map_width + p.first;
                        if (world_data.heightmap_data[p_idx] < lake_surface_h) {
                            world_data.heightmap_data[p_idx] = lake_surface_h; 
                            world_data.is_lake_tile[p_idx] = true;
                            tiles_filled++;
                        }
                    }
                    if (tiles_filled > 0) {
                        lakes_formed_count++;
                         std::cout << "    Lakes: ***** FORMED LAKE #" << lakes_formed_count << " (seed " << x_start << "," << y_start 
                                  << ") covering " << tiles_filled << " tiles. Surface=" << lake_surface_h 
                                  << ", LowestPt=" << lowest_point_in_basin << ", Depth=" << actual_depth << " *****" << std::endl;
                    }
                } 
            }
        }
    }
    std::cout << "  Lakes: Finished forming lakes. Total lakes formed: " << lakes_formed_count << std::endl;
}

} // namespace Lakes
} // namespace Systems
} // namespace World