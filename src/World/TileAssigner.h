// File: EmergentKingdoms/src/World/TileAssigner.h
#pragma once
#include "GenerationSteps/IGenerationStep.h"
#include <memory>
#include <vector>

namespace World {

/**
 * Modular TileAssigner that coordinates system-specific tile assigners
 * Each terrain system (Rivers, Lakes, Mountains, Land) handles its own tiles
 */
class TileAssigner : public Generation::IGenerationStep {
public:
    TileAssigner();
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Modular Tile Assigner Coordinator"; }

private:
    void initializeSystemAssigners();
    void performBaseTileClassification(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset);
    void calculateShorelineEffects(WorldData& world_data);
    
    std::vector<std::unique_ptr<Generation::IGenerationStep>> system_assigners;
};

} // namespace World