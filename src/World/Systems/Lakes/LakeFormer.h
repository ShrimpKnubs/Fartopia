// File: EmergentKingdoms/src/World/Systems/Lakes/LakeFormer.h
#pragma once
#include "../../GenerationSteps/IGenerationStep.h"
#include "LakeConfig.h"

namespace World {
namespace Systems {
namespace Lakes {

class LakeFormer : public Generation::IGenerationStep {
public:
    LakeFormer();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Lake Former"; }

private:
    float water_level_lake_max;
    float lake_min_effective_depth;
};

} // namespace Lakes
} // namespace Systems
} // namespace World