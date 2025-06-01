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
 * Manages all multi-tile vegetation objects in the world
 * Handles placement, collision detection, rendering, and animation
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
    void removeObjectAt(int world_x, int world_y);
    
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
    
    // Generation parameters
    struct GenerationConfig {
        // Tree generation - UPDATED for better clustering
        float tree_density = 0.05f;        // Reduced for clusters
        float ancient_tree_rarity = 0.15f;  // Higher chance in forests
        int min_tree_spacing = 4;           // Closer spacing for clusters
        
        // Boulder generation  
        float boulder_density = 0.02f;      // Reduced density
        float large_boulder_rarity = 0.3f;  // More large boulders
        int min_boulder_spacing = 15;       // Wider spacing
        
        // Resource generation
        float resource_boulder_chance = 0.3f;
        float gold_vein_rarity = 0.05f;
        float silver_vein_rarity = 0.08f;
        
        // Grass field generation
        float grass_field_density = 0.25f;
        int min_grass_field_size = 20;
        int max_grass_field_size = 80;
    };
    GenerationConfig config;
    
    // Generation methods - UPDATED for clustering
    void generateForestClusters(WorldData& world_data, unsigned int seed); // NEW: Main forest generation
    void generateSingleForestCluster(WorldData& world_data, unsigned int cluster_seed, 
                                    int cluster_size, bool is_large_forest); // NEW: Individual cluster
    void generateTrees(WorldData& world_data, unsigned int seed);         // UPDATED: Scattered trees only
    void generateBoulders(WorldData& world_data, unsigned int seed);
    void generateGrassFields(WorldData& world_data, unsigned int seed);
    
    // Placement validation
    bool canPlaceObject(const BaseVegetationObject& object, 
                       WorldData& world_data) const;
    bool hasCollision(const BaseVegetationObject& object) const;
    
    // Forest clustering helpers - NEW
    std::pair<int, int> findClusterCenter(const WorldData& world_data, unsigned int seed) const;
    bool isValidTreePosition(int x, int y, const WorldData& world_data, 
                           const std::vector<std::pair<int, int>>& existing_positions) const;
    
    // Spatial indexing
    uint64_t getSpatialKey(int world_x, int world_y) const;
    void addToSpatialIndex(BaseVegetationObject* object);
    void removeFromSpatialIndex(BaseVegetationObject* object);
    void rebuildSpatialIndex();
    
    // Object factory methods
    std::unique_ptr<BaseVegetationObject> createRandomTree(int x, int y, unsigned int seed,
                                                          const WorldData& world_data) const;
    std::unique_ptr<BaseVegetationObject> createRandomBoulder(int x, int y, unsigned int seed,
                                                            const WorldData& world_data) const;
    std::unique_ptr<BaseVegetationObject> createGrassField(int x, int y, int width, int height,
                                                          unsigned int seed) const;
    
    // Utility methods
    float getTerrainSuitability(int x, int y, const WorldData& world_data, 
                               const std::string& object_type) const;
    std::vector<std::pair<int, int>> findSuitableLocations(const WorldData& world_data,
                                                          const std::string& object_type,
                                                          int count, unsigned int seed) const;
};

} // namespace MultiTileObjects
} // namespace Vegetation
} // namespace Systems
} // namespace World