// File: EmergentKingdoms/src/World/Systems/Rivers/RiverNetworkSimulator.h
#pragma once
#include "../../GenerationSteps/IGenerationStep.h"
#include "RiverConfig.h"

namespace World {
namespace Systems {
namespace Rivers {

class RiverNetworkSimulator : public Generation::IGenerationStep {
public:
    RiverNetworkSimulator();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "River Network Simulator"; }

private:
    int num_sources_config;
    float river_start_min_elevation;
    float river_start_max_elevation;
    int river_max_length;
    float river_initial_volume;
    int river_width_tiles;
    float river_carve_strength_base;
    float river_carve_volume_scaling;
    float river_min_absolute_gradient;
    int river_max_stagnation_checks;
    float river_volume_increase_per_step;
    float river_max_volume;
    float terrain_river_bed_height;
};

} // namespace Rivers
} // namespace Systems
} // namespace World