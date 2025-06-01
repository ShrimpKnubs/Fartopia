// File: EmergentKingdoms/src/World/GenerationSteps/HydraulicEroder.cpp
#include "HydraulicEroder.h"
#include "../../Core/BaseConfig.h"
#include "WorldGenUtils.h" // For Utils::clamp_val
#include <iostream>
#include <vector>
#include <algorithm> // For std::min/max
#include <cmath>     // For std::fabs (not directly used but common)
#include <omp.h>

namespace World {
namespace Generation {

HydraulicEroder::HydraulicEroder() {
    iterations = Core::HYDRAULIC_EROSION_ITERATIONS;
    Kr = Core::Kr;
    Ks = Core::Ks;
    Ke = Core::Ke;
    Kd = Core::Kd;
}

void HydraulicEroder::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    (void)base_world_seed; // Not used
    (void)step_seed_offset; // Not used

    const size_t current_map_size = static_cast<size_t>(world_data.map_width) * static_cast<size_t>(world_data.map_height);
    if (iterations == 0) {
        std::cout << "  Skipping iterative hydraulic erosion (0 iterations)." << std::endl;
        return;
    }
    std::cout << "  Applying iterative hydraulic erosion..." << std::endl;
    
    std::vector<float> water_map(current_map_size, 0.0f);
    std::vector<float> sediment_map(current_map_size, 0.0f);
    std::vector<std::vector<float>> outflow_flux(current_map_size, std::vector<float>(4, 0.0f));
    std::vector<float> temp_height_change(current_map_size, 0.0f);

    const int dx4[] = {0, 1, 0, -1}; 
    const int dy4[] = {-1, 0, 1, 0}; 

    for (int iter = 0; iter < iterations; ++iter) {
        std::cout << "    Hydraulic erosion iteration " << iter + 1 << "/" << iterations << "..." << std::endl;

        // 1. Add water (rain)
        #pragma omp parallel for
        for (size_t i = 0; i < current_map_size; ++i) {
            if (world_data.is_lake_tile[i]) water_map[i] += 0.001f;
            else water_map[i] += 0.01f; 
        }

        // 2. Calculate water outflow flux
        #pragma omp parallel for
        for (int y = 0; y < world_data.map_height; ++y) {
            for (int x = 0; x < world_data.map_width; ++x) {
                size_t current_idx = static_cast<size_t>(y) * world_data.map_width + x;
                float h_total_current = world_data.heightmap_data[current_idx] + water_map[current_idx];
                float total_dH_positive = 0.0f;
                std::vector<float> dH(4, 0.0f);

                for(int i=0; i<4; ++i) {
                    int ny = y + dy4[i];
                    int nx = (x + dx4[i] + world_data.map_width) % world_data.map_width;

                    if(ny >= 0 && ny < world_data.map_height) {
                        size_t neighbor_idx = static_cast<size_t>(ny) * world_data.map_width + nx;
                        float h_total_neighbor = world_data.heightmap_data[neighbor_idx] + water_map[neighbor_idx];
                        dH[i] = h_total_current - h_total_neighbor;
                        if (dH[i] > 0) {
                            total_dH_positive += dH[i];
                        }
                    } else { 
                        dH[i] = h_total_current; 
                        if (dH[i] > 0) total_dH_positive += dH[i];
                    }
                }

                for(int i=0; i<4; ++i) {
                    if(dH[i] > 0 && total_dH_positive > 1e-6f) {
                        outflow_flux[current_idx][i] = std::min(water_map[current_idx], dH[i]) * (dH[i] / total_dH_positive);
                    } else {
                        outflow_flux[current_idx][i] = 0.0f;
                    }
                    outflow_flux[current_idx][i] = std::max(0.0f, outflow_flux[current_idx][i]);
                }
            }
        }
        
        // 3. Update water levels and transport sediment
        std::vector<float> next_water_map(current_map_size, 0.0f);
        std::vector<float> next_sediment_map(current_map_size, 0.0f);
        temp_height_change.assign(current_map_size, 0.0f);

        #pragma omp parallel for
        for (int y = 0; y < world_data.map_height; ++y) {
            for (int x = 0; x < world_data.map_width; ++x) {
                size_t current_idx = static_cast<size_t>(y) * world_data.map_width + x;
                float water_out = 0.0f;
                for(int dir=0; dir<4; ++dir) water_out += outflow_flux[current_idx][dir];
                
                float water_in = 0.0f;
                int neighbor_x_coords[] = {x, (x + 1 + world_data.map_width)%world_data.map_width, x, (x - 1 + world_data.map_width)%world_data.map_width};
                int neighbor_y_coords[] = {y - 1, y, y + 1, y};
                int inflow_directions[] = {2, 3, 0, 1}; 

                for(int i=0; i<4; ++i) {
                    int ny = neighbor_y_coords[i];
                    int nx = neighbor_x_coords[i];
                    if(ny >=0 && ny < world_data.map_height) {
                         size_t neighbor_idx = static_cast<size_t>(ny) * world_data.map_width + nx;
                         water_in += outflow_flux[neighbor_idx][inflow_directions[i]];
                    }
                }
                
                next_water_map[current_idx] = water_map[current_idx] - water_out + water_in;

                float current_water_safe = std::max(1e-6f, water_map[current_idx]); 
                float sediment_ratio_out = water_out / current_water_safe;
                float sed_out = sediment_map[current_idx] * sediment_ratio_out;

                float sed_in = 0.0f;
                 for(int i=0; i<4; ++i) {
                    int ny = neighbor_y_coords[i];
                    int nx = neighbor_x_coords[i];
                    if(ny >=0 && ny < world_data.map_height) {
                        size_t neighbor_idx = static_cast<size_t>(ny) * world_data.map_width + nx;
                        float neighbor_water_safe = std::max(1e-6f, water_map[neighbor_idx]);
                        sed_in += sediment_map[neighbor_idx] * (outflow_flux[neighbor_idx][inflow_directions[i]] / neighbor_water_safe);
                    }
                }
                next_sediment_map[current_idx] = sediment_map[current_idx] - sed_out + sed_in;
                next_sediment_map[current_idx] = std::max(0.0f, next_sediment_map[current_idx]);
            }
        }
        water_map = next_water_map;
        sediment_map = next_sediment_map;

        // 4. Erosion and Deposition
        #pragma omp parallel for
        for (int y = 0; y < world_data.map_height; ++y) {
            for (int x = 0; x < world_data.map_width; ++x) {
                size_t current_idx = static_cast<size_t>(y) * world_data.map_width + x;
                if (world_data.is_lake_tile[current_idx]) continue;

                float slope_val = world_data.slope_map[current_idx]; 
                float C = Ks * slope_val * water_map[current_idx]; 
                C = std::max(0.0f, C);

                if (sediment_map[current_idx] < C) { 
                    float erode_amount = Kr * slope_val * water_map[current_idx];
                    erode_amount = std::min(erode_amount, C - sediment_map[current_idx]);
                    erode_amount = std::min(erode_amount, world_data.heightmap_data[current_idx] * 0.01f); 
                    temp_height_change[current_idx] -= erode_amount;
                    sediment_map[current_idx] += erode_amount;
                } else { 
                    float deposit_amount = Kd * (sediment_map[current_idx] - C);
                    deposit_amount = std::min(deposit_amount, sediment_map[current_idx]);
                    temp_height_change[current_idx] += deposit_amount;
                    sediment_map[current_idx] -= deposit_amount;
                }
            }
        }
        
        #pragma omp parallel for
        for(size_t i=0; i<current_map_size; ++i) {
            world_data.heightmap_data[i] += temp_height_change[i];
            world_data.heightmap_data[i] = Utils::clamp_val(world_data.heightmap_data[i], 0.0f, 1.0f);
        }

        // 5. Evaporation
        #pragma omp parallel for
        for (size_t i = 0; i < current_map_size; ++i) {
            water_map[i] *= (1.0f - Ke);
            sediment_map[i] = std::max(0.0f, sediment_map[i] * (1.0f - Ke*0.1f)); 
        }
    }
    std::cout << "  Finished iterative hydraulic erosion." << std::endl;
}

} // namespace Generation
} // namespace World