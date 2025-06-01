// File: EmergentKingdoms/src/World/GenerationSteps/ThermalEroder.cpp
#include "ThermalEroder.h"
#include "../../Core/BaseConfig.h"
#include "WorldGenUtils.h" // For Utils::clamp_val
#include <iostream>
#include <vector>
#include <cmath>    // For std::fabs
#include <algorithm> // For std::min/max
#include <omp.h>

namespace World {
namespace Generation {

ThermalEroder::ThermalEroder() {
    iterations = Core::THERMAL_EROSION_ITERATIONS;
    talus_angle_factor = Core::THERMAL_EROSION_TALUS_ANGLE_FACTOR;
    strength = Core::THERMAL_EROSION_STRENGTH;
}

void ThermalEroder::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    (void)base_world_seed; // Not used directly for seeding this deterministic process
    (void)step_seed_offset;

    if (iterations == 0) {
        std::cout << "  Skipping thermal erosion (0 iterations)." << std::endl;
        return;
    }
    std::cout << "  Applying thermal erosion..." << std::endl;

    const size_t current_map_size = static_cast<size_t>(world_data.map_width) * static_cast<size_t>(world_data.map_height);
    std::vector<float> temp_heightmap_read; 
    std::vector<float> temp_heightmap_write(current_map_size); 

    for (int i = 0; i < iterations; ++i) {
        temp_heightmap_read = world_data.heightmap_data; 

        #pragma omp parallel for
        for (int y = 0; y < world_data.map_height; ++y) {
            for (int x = 0; x < world_data.map_width; ++x) {
                size_t current_idx = static_cast<size_t>(y) * world_data.map_width + x;
                float current_h = temp_heightmap_read[current_idx];
                float new_h_current = current_h; 

                if (world_data.is_lake_tile[current_idx] || world_data.is_river_tile[current_idx]) {
                     temp_heightmap_write[current_idx] = current_h;
                     continue;
                }
                
                for (int dy_offset = -1; dy_offset <= 1; ++dy_offset) {
                    for (int dx_offset = -1; dx_offset <= 1; ++dx_offset) {
                        if (dx_offset == 0 && dy_offset == 0) continue;

                        int ny_abs = y + dy_offset;
                        int nx_abs = x + dx_offset; 

                        if (ny_abs >= 0 && ny_abs < world_data.map_height) { 
                            // Use WorldData's getWrappedHeight for reading from temp_heightmap_read
                            float neighbor_h = world_data.getWrappedHeight(temp_heightmap_read, nx_abs, ny_abs);

                            float height_diff = current_h - neighbor_h;
                            if (height_diff > talus_angle_factor) {
                                float material_to_move = (height_diff - talus_angle_factor) * strength;
                                material_to_move = std::min(material_to_move, height_diff / 2.1f); 
                                material_to_move = std::max(0.0f, material_to_move);
                                new_h_current -= material_to_move;
                            }
                        }
                    }
                }
                temp_heightmap_write[current_idx] = new_h_current;
            }
        }
        
        std::vector<float> material_added(current_map_size, 0.0f);
        #pragma omp parallel for
        for (int y = 0; y < world_data.map_height; ++y) {
            for (int x = 0; x < world_data.map_width; ++x) {
                size_t current_idx_read = static_cast<size_t>(y) * world_data.map_width + x;
                float original_h_at_read = temp_heightmap_read[current_idx_read];

                if (world_data.is_lake_tile[current_idx_read] || world_data.is_river_tile[current_idx_read]) continue;

                for (int dy_offset = -1; dy_offset <= 1; ++dy_offset) {
                    for (int dx_offset = -1; dx_offset <= 1; ++dx_offset) {
                        if (dx_offset == 0 && dy_offset == 0) continue;
                        
                        int neighbor_y_abs = y + dy_offset;
                        if (neighbor_y_abs >= 0 && neighbor_y_abs < world_data.map_height) {
                            int neighbor_x_abs = x + dx_offset;
                            float neighbor_original_h = world_data.getWrappedHeight(temp_heightmap_read, neighbor_x_abs, neighbor_y_abs);
                            float height_diff_from_neighbor = neighbor_original_h - original_h_at_read;

                            if (height_diff_from_neighbor > talus_angle_factor) {
                                float material_received = (height_diff_from_neighbor - talus_angle_factor) * strength;
                                material_received = std::min(material_received, height_diff_from_neighbor / 2.1f);
                                material_received = std::max(0.0f, material_received);
                                
                                #pragma omp atomic
                                material_added[current_idx_read] += material_received;
                            }
                        }
                    }
                }
            }
        }

        #pragma omp parallel for
        for(size_t k=0; k < current_map_size; ++k) {
            temp_heightmap_write[k] += material_added[k]; 
            temp_heightmap_write[k] = Utils::clamp_val(temp_heightmap_write[k], 0.0f, 1.0f);
        }
        world_data.heightmap_data = temp_heightmap_write; 
        std::cout << "  Thermal erosion iteration " << i + 1 << "/" << iterations << " done." << std::endl;
    }
}

} // namespace Generation
} // namespace World