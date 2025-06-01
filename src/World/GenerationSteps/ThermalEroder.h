// File: EmergentKingdoms/src/World/GenerationSteps/ThermalEroder.h
#pragma once
#include "IGenerationStep.h"

namespace World {
namespace Generation {

class ThermalEroder : public IGenerationStep {
public:
    ThermalEroder();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Thermal Eroder"; }

private:
    int iterations;
    float talus_angle_factor;
    float strength;
};

} // namespace Generation
} // namespace World