// File: EmergentKingdoms/src/World/GenerationSteps/BaseHeightGenerator.h
#pragma once
#include "IGenerationStep.h"
#include "../../Core/FastNoiseLite.h"

namespace World {
namespace Generation {

class BaseHeightGenerator : public IGenerationStep {
public:
    BaseHeightGenerator();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Base Height Generator"; }

private:
    FastNoiseLite base_height_noise;
    FastNoiseLite detail_noise;
    FastNoiseLite basin_carving_noise; 

    // Config parameters for base/detail
    float base_noise_frequency_param; 
    int base_noise_octaves_param;
    float base_noise_lacunarity_param;
    float base_noise_persistence_param;
    float detail_noise_frequency_multiplier_param;
    int detail_noise_octaves_param;
    // Renamed these to be more generic for base height generation range
    float terrain_generation_min_height_param; 
    float terrain_generation_max_height_param;


    // Config parameters for basin carving
    float basin_carving_noise_frequency_param;
    int   basin_carving_noise_octaves_param;
    float basin_carving_strength_param;
    float basin_carving_height_threshold_max_param;
    float basin_carving_noise_trigger_min_param;
    float basin_carving_noise_trigger_max_param;
};

} // namespace Generation
} // namespace World