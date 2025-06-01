// File: EmergentKingdoms/src/World/Systems/Mountains/MountainGenerator.h
#pragma once
#include "../../GenerationSteps/IGenerationStep.h"
#include "../../../Core/FastNoiseLite.h"
#include "MountainConfig.h"

namespace World {
namespace Systems {
namespace Mountains {

class MountainGenerator : public Generation::IGenerationStep {
public:
    MountainGenerator();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Mountain Range Generator"; }

private:
    FastNoiseLite range_noise_gen;    // For the main branching structure of ranges
    FastNoiseLite detail_noise_gen;   // For cragginess and smaller features on ranges

    // Massif parameters
    float massif_radius_factor;
    float massif_falloff_steepness;

    // Range Structure Noise parameters
    float range_noise_frequency;
    int   range_noise_octaves;
    float range_noise_lacunarity;
    float range_noise_gain;
    float range_threshold_min;

    // Height parameters
    float range_base_height_min;
    float range_peak_height_max;

    // Detail Noise parameters
    float detail_noise_frequency;
    int   detail_noise_octaves;
    float detail_noise_strength;
};

} // namespace Mountains
} // namespace Systems
} // namespace World