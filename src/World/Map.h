// File: EmergentKingdoms/src/World/Map.h
#pragma once
#include "Tile.h"
#include "WorldData.h"
#include "GenerationSteps/IGenerationStep.h"
#include "../Core/Renderer.h"
#include <vector>
#include <memory>

// Forward declarations for vegetation objects
namespace World {
namespace Systems {
namespace Vegetation {
namespace MultiTileObjects {
    class VegetationObjectManager;
}
}
}
}

namespace World {

/**
 * Main world map containing all terrain tiles and coordinating world generation
 * Supports cylindrical wrapping on X-axis and integrates multi-tile vegetation objects
 */
class Map {
public:
    Map(int width, int height, unsigned int seed);
    ~Map();

    // Core map functionality
    void generate();
    
    const Tile& getTile(int x, int y) const;
    void setTile(int x, int y, const Tile& tile);
    
    // Map properties
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    unsigned int getSeed() const { return seed; }
    
    // ===== VEGETATION OBJECT SYSTEM INTEGRATION =====
    bool hasVegetationObjectAt(int x, int y) const;
    Core::ScreenCell getVegetationObjectDisplay(int x, int y, int entity_x = -1, int entity_y = -1) const;
    bool isVegetationPassable(int x, int y) const;
    void setVegetationObjectManager(Systems::Vegetation::MultiTileObjects::VegetationObjectManager* manager);
    
    // World generation data access (for generation steps)
    std::vector<Tile>& getTilesRef() { return tiles; }

private:
    // Map dimensions and properties
    int width, height;
    unsigned int seed;
    
    // Tile storage
    std::vector<Tile> tiles;
    
    // Generation pipeline
    std::vector<std::unique_ptr<Generation::IGenerationStep>> generation_steps;
    
    // ===== VEGETATION OBJECT MANAGER =====
    Systems::Vegetation::MultiTileObjects::VegetationObjectManager* vegetation_object_manager;
    
    // Internal methods
    void initializeGenerationSteps();
    void validateCoordinates(int x, int y) const;
    size_t getIndex(int x, int y) const;
    
    // World generation data structures (for generation pipeline)
    std::vector<float> heightmap_data;
    std::vector<bool> is_river_tile;
    std::vector<bool> is_lake_tile;
    std::vector<float> slope_map;
    std::vector<SlopeAspect> aspect_map;
    std::vector<bool> lake_has_waves_map;
    
    // Helper methods for generation
    void initializeWorldData();
    void runGenerationPipeline();
};

} // namespace World