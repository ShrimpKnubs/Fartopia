// File: EmergentKingdoms/src/World/Systems/Lakes/LakeTileAssigner.h
#pragma once
#include "../../GenerationSteps/IGenerationStep.h"
#include "../../../Core/FastNoiseLite.h"
#include "LakeConfig.h"

namespace World {
namespace Systems {
namespace Lakes {

class LakeTileAssigner : public Generation::IGenerationStep {
public:
    LakeTileAssigner();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Lake Tile Assigner"; }

private:
    float water_level_lake_max_height;
    float pond_max_surface_height;
    
    // Noise generators for professional wave animation
    FastNoiseLite wave_strand_noise_generator; 
    FastNoiseLite animation_phase_noise_generator;
};

} // namespace Lakes
} // namespace Systems
} // namespace World