// File: EmergentKingdoms/src/World/GenerationSteps/IGenerationStep.h
#pragma once
#include <string>
#include "World/WorldData.h" // Corrected include path relative to -Isrc

namespace World {
namespace Generation {

class IGenerationStep {
public:
    virtual ~IGenerationStep() = default;

    // Each step processes the world_data.
    // base_world_seed: The main seed for the world.
    // step_seed_offset: A unique offset for this step to ensure its RNG is different from others.
    virtual void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) = 0;
    virtual std::string getName() const = 0; // For logging
};

} // namespace Generation
} // namespace World