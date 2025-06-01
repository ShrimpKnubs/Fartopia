// File: EmergentKingdoms/src/World/GenerationSteps/SlopeAspectCalculator.h
#pragma once
#include "IGenerationStep.h"
// No FastNoiseLite needed here

namespace World {
namespace Generation {

class SlopeAspectCalculator : public IGenerationStep {
public:
    SlopeAspectCalculator();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Slope & Aspect Calculator"; }

private:
    // Config params if needed, e.g., for STEEP_PEAK thresholds
    float slope_threshold_very_steep;
    float terrain_mountain_mid_height;
};

} // namespace Generation
} // namespace World