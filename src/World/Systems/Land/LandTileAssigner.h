// File: EmergentKingdoms/src/World/Systems/Land/LandTileAssigner.h
#pragma once
#include "../../GenerationSteps/IGenerationStep.h"
#include "LandConfig.h"

namespace World {
namespace Systems {
namespace Land {

class LandTileAssigner : public Generation::IGenerationStep {
public:
    LandTileAssigner();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Land Tile Assigner"; }

private:
    float marsh_water_coverage_chance;
};

} // namespace Land
} // namespace Systems
} // namespace World