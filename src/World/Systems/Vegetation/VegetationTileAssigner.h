// File: EmergentKingdoms/src/World/Systems/Vegetation/VegetationTileAssigner.h
#pragma once
#include "../../GenerationSteps/IGenerationStep.h"
#include <random>
#include <memory>

namespace World {
namespace Systems {
namespace Vegetation {

// Forward declarations
namespace MultiTileObjects {
    class VegetationObjectManager;
}

/**
 * Enhanced Vegetation Tile Assigner with Multi-Tile Object Support
 * Creates large trees, boulders, and grass fields that span multiple tiles
 * Also handles single-tile vegetation and enhanced grass animation
 */
class VegetationTileAssigner : public Generation::IGenerationStep {
public:
    VegetationTileAssigner();
    ~VegetationTileAssigner();
    
    void process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) override;
    std::string getName() const override { return "Multi-Tile Vegetation Assigner"; }
    
    // Multi-tile object interface
    Core::ScreenCell getMultiTileObjectDisplay(int world_x, int world_y, 
                                              int entity_x = -1, int entity_y = -1) const;
    bool hasMultiTileObjectAt(int world_x, int world_y) const;
    bool isPassable(int world_x, int world_y) const;

private:
    std::unique_ptr<MultiTileObjects::VegetationObjectManager> object_manager;
    
    // Single-tile vegetation methods
    void applySingleTileVegetation(WorldData& world_data, unsigned int seed);
    void applyGrassAnimation(WorldData& world_data, unsigned int seed);
    
    BaseTileType determineSingleTileVegetationType(int x, int y, WorldData& world_data, 
                                                  std::mt19937& rng, 
                                                  std::uniform_real_distribution<float>& dist);
    
    void replaceSingleTileVegetation(WorldData& world_data, size_t index, 
                                    BaseTileType vegetation_type, int x, int y);
    
    // Wind and animation
    float getWindNoise(int x, int y, unsigned int seed) const;
    
    // Helper methods
    bool isSuitableForVegetation(BaseTileType base_type) const;
};

} // namespace Vegetation
} // namespace Systems
} // namespace World