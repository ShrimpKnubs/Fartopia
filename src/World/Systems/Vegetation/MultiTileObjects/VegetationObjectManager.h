// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/VegetationObjectManager.h
#pragma once
#include "BaseVegetationObject.h"
#include "../../../WorldData.h"
#include "../../../../Core/Renderer.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace World {
namespace Systems {
namespace Vegetation {
namespace MultiTileObjects {

/**
 * OPTIMIZED Vegetation Object Manager
 * Fast generation of large trees, boulders, and grass fields
 * Uses grid-based placement and simplified algorithms for speed
 */
class VegetationObjectManager {
public:
    VegetationObjectManager();
    ~VegetationObjectManager() = default;

    // Object lifecycle
    void generateObjects(WorldData& world_data, unsigned int base_seed);
    void updateAnimations(float time_delta);
    void clear();

    // Rendering
    Core::ScreenCell getTileDisplay(int world_x, int world_y, 
                                   int entity_x = -1, int entity_y = -1) const;
    bool hasTileAt(int world_x, int world_y) const;
    
    // Collision and interaction
    bool isPassable(int world_x, int world_y) const;
    std::vector<BaseVegetationObject*> getObjectsAt(int world_x, int world_y) const;
    
    // Object management
    void addObject(std::unique_ptr<BaseVegetationObject> object);
    
    // Statistics and debugging
    size_t getObjectCount() const { return objects.size(); }
    void printStats() const;

private:
    // Object storage
    std::vector<std::unique_ptr<BaseVegetationObject>> objects;
    
    // Spatial indexing for fast lookups
    struct SpatialCell {
        std::vector<BaseVegetationObject*> objects;
    };
    std::unordered_map<uint64_t, SpatialCell> spatial_index;
    static constexpr int SPATIAL_CELL_SIZE = 32; // Tiles per spatial cell
    
    // OPTIMIZED Generation parameters
    struct GenerationConfig {
        // Tree generation - OPTIMIZED for performance and density
        float tree_density = 0.4f;         // High density for lush forests
        float ancient_tree_rarity = 0.3f;  // Good mix of ancient/young
        int min_tree_spacing = 8;          // Reasonable spacing for performance
        
        // Boulder generation - OPTIMIZED  
        float boulder_density = 0.15f;     // More impressive formations
        float large_boulder_rarity = 0.35f; // Good mix of sizes
        int min_boulder_spacing = 15;      // Reasonable spacing
        
        // Resource generation
        float resource_boulder_chance = 0.5f; // Good resource opportunities
    };
    GenerationConfig config;
    
    // OPTIMIZED Generation methods - Fast algorithms
    void generateOptimizedBoulders(WorldData& world_data, unsigned int seed);
    void generateOptimizedTrees(WorldData& world_data, unsigned int seed);
    
    // OPTIMIZED Placement validation - Fast checks
    bool canPlaceObjectFast(const BaseVegetationObject& object, WorldData& world_data) const;
    bool isValidBoulderLocation(int x, int y, const WorldData& world_data) const;
    bool isValidTreeLocation(int x, int y, const WorldData& world_data) const;
    
    // Original placement validation (for fallback)
    bool canPlaceObject(const BaseVegetationObject& object, WorldData& world_data) const;
    bool hasCollision(const BaseVegetationObject& object) const;
    
    // Spatial indexing
    uint64_t getSpatialKey(int world_x, int world_y) const;
    void addToSpatialIndex(BaseVegetationObject* object);
    void rebuildSpatialIndex();
};

} // namespace MultiTileObjects
} // namespace Vegetation
} // namespace Systems
} // namespace World