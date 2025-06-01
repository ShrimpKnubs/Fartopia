// File: EmergentKingdoms/src/World/GenerationSteps/HydraulicEroder.h
#pragma once
#include "IGenerationStep.h"

namespace World {
namespace Generation {

class HydraulicEroder : public IGenerationStep {
public:
    HydraulicEroder();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Iterative Hydraulic Eroder"; }

private:
    int iterations;
    float Kr, Ks, Ke, Kd; // Erosion constants
};

} // namespace Generation
} // namespace World