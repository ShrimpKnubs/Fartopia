// File: EmergentKingdoms/src/World/Systems/Rivers/RiverTileAssigner.h
#pragma once
#include "../../GenerationSteps/IGenerationStep.h"
#include "RiverConfig.h"

namespace World {
namespace Systems {
namespace Rivers {

class RiverTileAssigner : public Generation::IGenerationStep {
public:
    RiverTileAssigner();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "River Tile Assigner"; }

private:
    float terrain_river_bed_height;
};

} // namespace Rivers
} // namespace Systems
} // namespace World