// File: EmergentKingdoms/src/World/GenerationSteps/BorderWallPlacer.h
#pragma once
#include "IGenerationStep.h"
#include "../Map.h" // For map_context

namespace World {
namespace Generation {

class BorderWallPlacer : public IGenerationStep {
public:
    BorderWallPlacer() = default; // No specific config needed from constructor
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Border Wall Placer"; }
};

} // namespace Generation
} // namespace World