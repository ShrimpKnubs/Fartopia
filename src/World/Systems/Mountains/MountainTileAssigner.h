// File: EmergentKingdoms/src/World/Systems/Mountains/MountainTileAssigner.h
#pragma once
#include "../../GenerationSteps/IGenerationStep.h"
#include "MountainConfig.h"

namespace World {
namespace Systems {
namespace Mountains {

class MountainTileAssigner : public Generation::IGenerationStep {
public:
    MountainTileAssigner();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Mountain Tile Assigner"; }

private:
    float snowline_min_height;
};

} // namespace Mountains
} // namespace Systems
} // namespace World