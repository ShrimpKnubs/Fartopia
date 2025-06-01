// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/VegetationObjectManager.cpp
#include "VegetationObjectManager.h"
#include "Trees/AncientOakTree.h"
#include "Trees/YoungTree.h"
#include "Boulders/ResourceBoulder.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <map>

namespace World {
namespace Systems {
namespace Vegetation {
namespace MultiTileObjects {

VegetationObjectManager::VegetationObjectManager() {
    // OPTIMIZED: Higher density, faster generation
    config.tree_density = 0.4f;         // High density for lush forests
    config.ancient_tree_rarity = 0.3f;   // Good mix of ancient and young trees
    config.min_tree_spacing = 8;         // Reasonable spacing for performance
    config.boulder_density = 0.15f;      // More impressive boulder formations
    config.large_boulder_rarity = 0.35f; // Good mix of sizes
    config.min_boulder_spacing = 15;     // Reasonable boulder spacing
    config.resource_boulder_chance = 0.5f; // Good resource opportunities
}

void VegetationObjectManager::generateObjects(WorldData& world_data, unsigned int base_seed) {
    clear();
    
    std::cout << "    Multi-Tile Objects: Generating optimized medieval landscape..." << std::endl;
    
    // OPTIMIZED: Generate in parallel where possible, simpler algorithms
    generateOptimizedBoulders(world_data, base_seed + 1000);
    generateOptimizedTrees(world_data, base_seed + 2000);
    
    // Rebuild spatial index for fast lookups
    rebuildSpatialIndex();
    
    printStats();
}

void VegetationObjectManager::generateOptimizedBoulders(WorldData& world_data, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist_0_1(0.0f, 1.0f);
    
    // OPTIMIZED: Calculate boulder count more efficiently
    int total_boulder_count = static_cast<int>(world_data.map_width * world_data.map_height * config.boulder_density / 2000.0f);
    
    std::cout << "      Placing " << total_boulder_count << " boulders (optimized)..." << std::endl;
    
    // OPTIMIZED: Simple grid-based placement with random offset
    int grid_size = static_cast<int>(std::sqrt(world_data.map_width * world_data.map_height / total_boulder_count));
    grid_size = std::max(config.min_boulder_spacing, grid_size);
    
    int boulders_placed = 0;
    
    for (int grid_y = 0; grid_y < world_data.map_height; grid_y += grid_size) {
        for (int grid_x = 0; grid_x < world_data.map_width; grid_x += grid_size) {
            if (boulders_placed >= total_boulder_count) break;
            
            // Random position within grid cell
            int boulder_x = grid_x + static_cast<int>(dist_0_1(rng) * grid_size);
            int boulder_y = grid_y + static_cast<int>(dist_0_1(rng) * grid_size);
            
            // Keep within bounds
            boulder_x = std::max(15, std::min(boulder_x, world_data.map_width - 35));
            boulder_y = std::max(15, std::min(boulder_y, world_data.map_height - 35));
            
            // Quick terrain check
            if (isValidBoulderLocation(boulder_x, boulder_y, world_data)) {
                // Determine boulder properties
                float size_roll = dist_0_1(rng);
                Boulders::ResourceBoulder::BoulderSize size;
                if (size_roll < 0.2f) size = Boulders::ResourceBoulder::BoulderSize::SMALL;
                else if (size_roll < 0.5f) size = Boulders::ResourceBoulder::BoulderSize::MEDIUM;
                else if (size_roll < 0.8f) size = Boulders::ResourceBoulder::BoulderSize::LARGE;
                else size = Boulders::ResourceBoulder::BoulderSize::MASSIVE;
                
                // Determine resources
                Boulders::ResourceBoulder::ResourceType resource = Boulders::ResourceBoulder::ResourceType::NONE;
                if (dist_0_1(rng) < config.resource_boulder_chance) {
                    float resource_roll = dist_0_1(rng);
                    if (resource_roll < 0.1f) {
                        resource = Boulders::ResourceBoulder::ResourceType::GOLD_VEINS;
                    } else if (resource_roll < 0.25f) {
                        resource = Boulders::ResourceBoulder::ResourceType::SILVER_VEINS;
                    } else if (resource_roll < 0.6f) {
                        resource = Boulders::ResourceBoulder::ResourceType::IRON_DEPOSITS;
                    } else {
                        resource = Boulders::ResourceBoulder::ResourceType::COPPER_DEPOSITS;
                    }
                }
                
                auto boulder = std::make_unique<Boulders::ResourceBoulder>(
                    boulder_x, boulder_y, rng(), size, resource);
                
                if (canPlaceObjectFast(*boulder, world_data)) {
                    addObject(std::move(boulder));
                    boulders_placed++;
                }
            }
        }
        if (boulders_placed >= total_boulder_count) break;
    }
}

void VegetationObjectManager::generateOptimizedTrees(WorldData& world_data, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist_0_1(0.0f, 1.0f);
    
    // OPTIMIZED: Calculate tree count more efficiently
    int total_tree_count = static_cast<int>(world_data.map_width * world_data.map_height * config.tree_density / 1500.0f);
    
    std::cout << "      Placing " << total_tree_count << " trees (optimized)..." << std::endl;
    
    // OPTIMIZED: Use cluster-based placement for natural forest appearance
    int clusters = total_tree_count / 15; // Each cluster has ~15 trees
    int trees_placed = 0;
    
    for (int cluster = 0; cluster < clusters && trees_placed < total_tree_count; ++cluster) {
        // Find cluster center
        int center_x = 50 + static_cast<int>(dist_0_1(rng) * (world_data.map_width - 100));
        int center_y = 50 + static_cast<int>(dist_0_1(rng) * (world_data.map_height - 100));
        
        if (!isValidTreeLocation(center_x, center_y, world_data)) continue;
        
        // Place trees in cluster
        int cluster_radius = 20 + static_cast<int>(dist_0_1(rng) * 20);
        int trees_in_cluster = 10 + static_cast<int>(dist_0_1(rng) * 10);
        
        for (int tree_in_cluster = 0; tree_in_cluster < trees_in_cluster && trees_placed < total_tree_count; ++tree_in_cluster) {
            // Random position within cluster
            float angle = dist_0_1(rng) * 2.0f * 3.14159f;
            float distance = dist_0_1(rng) * cluster_radius;
            
            int tree_x = center_x + static_cast<int>(distance * std::cos(angle));
            int tree_y = center_y + static_cast<int>(distance * std::sin(angle));
            
            // Keep within bounds
            tree_x = std::max(20, std::min(tree_x, world_data.map_width - 20));
            tree_y = std::max(20, std::min(tree_y, world_data.map_height - 20));
            
            if (isValidTreeLocation(tree_x, tree_y, world_data)) {
                // Check spacing from existing objects (simplified)
                bool too_close = false;
                for (const auto& existing : objects) {
                    int dx = tree_x - existing->getOriginX();
                    int dy = tree_y - existing->getOriginY();
                    float dist = std::sqrt(dx * dx + dy * dy);
                    if (dist < config.min_tree_spacing) {
                        too_close = true;
                        break;
                    }
                }
                
                if (!too_close) {
                    // Create tree
                    bool is_ancient = dist_0_1(rng) < config.ancient_tree_rarity;
                    
                    std::unique_ptr<BaseVegetationObject> tree;
                    if (is_ancient) {
                        tree = std::make_unique<Trees::AncientOakTree>(tree_x, tree_y, rng());
                    } else {
                        tree = std::make_unique<Trees::YoungTree>(tree_x, tree_y, rng());
                    }
                    
                    if (canPlaceObjectFast(*tree, world_data)) {
                        addObject(std::move(tree));
                        trees_placed++;
                    }
                }
            }
        }
    }
}

bool VegetationObjectManager::isValidBoulderLocation(int x, int y, const WorldData& world_data) const {
    if (x < 0 || x >= world_data.map_width || y < 0 || y >= world_data.map_height) {
        return false;
    }
    
    size_t index = static_cast<size_t>(y) * world_data.map_width + x;
    if (index >= world_data.heightmap_data.size()) return false;
    
    float height = world_data.heightmap_data[index];
    float slope = (index < world_data.slope_map.size()) ? world_data.slope_map[index] : 0.0f;
    
    // FIXED: Don't place on water or very steep slopes
    if (world_data.is_river_tile[index] || world_data.is_lake_tile[index]) {
        return false;
    }
    
    // Boulders prefer hilly terrain but not cliffs
    return height >= 0.03f && height <= 0.85f && slope <= 0.12f;
}

bool VegetationObjectManager::isValidTreeLocation(int x, int y, const WorldData& world_data) const {
    if (x < 0 || x >= world_data.map_width || y < 0 || y >= world_data.map_height) {
        return false;
    }
    
    size_t index = static_cast<size_t>(y) * world_data.map_width + x;
    if (index >= world_data.heightmap_data.size()) return false;
    
    float height = world_data.heightmap_data[index];
    float slope = (index < world_data.slope_map.size()) ? world_data.slope_map[index] : 0.0f;
    
    // FIXED: Don't place on water, mountains, or steep slopes
    if (world_data.is_river_tile[index] || world_data.is_lake_tile[index]) {
        return false;
    }
    
    // Trees prefer gentler terrain
    return height >= 0.05f && height <= 0.7f && slope <= 0.03f;
}

bool VegetationObjectManager::canPlaceObjectFast(const BaseVegetationObject& object, WorldData& world_data) const {
    // OPTIMIZED: Faster placement check
    if (!object.canPlaceAt(object.getOriginX(), object.getOriginY(), 
                          world_data.heightmap_data, world_data.slope_map,
                          world_data.map_width, world_data.map_height)) {
        return false;
    }
    
    // Simple overlap check - just check if any existing object's center is too close
    for (const auto& existing : objects) {
        int dx = object.getOriginX() - existing->getOriginX();
        int dy = object.getOriginY() - existing->getOriginY();
        float distance = std::sqrt(dx * dx + dy * dy);
        
        // Simple distance check based on object sizes
        float min_distance = (object.getWidth() + existing->getWidth()) / 2.0f + 2.0f;
        if (distance < min_distance) {
            return false;
        }
    }
    
    return true;
}

// [Keep all the existing helper methods unchanged]
bool VegetationObjectManager::canPlaceObject(const BaseVegetationObject& object, WorldData& world_data) const {
    if (!object.canPlaceAt(object.getOriginX(), object.getOriginY(), 
                          world_data.heightmap_data, world_data.slope_map,
                          world_data.map_width, world_data.map_height)) {
        return false;
    }
    return !hasCollision(object);
}

bool VegetationObjectManager::hasCollision(const BaseVegetationObject& object) const {
    for (const auto& existing : objects) {
        if (object.overlaps(*existing)) {
            return true;
        }
    }
    return false;
}

void VegetationObjectManager::addObject(std::unique_ptr<BaseVegetationObject> object) {
    BaseVegetationObject* obj_ptr = object.get();
    objects.push_back(std::move(object));
    addToSpatialIndex(obj_ptr);
}

Core::ScreenCell VegetationObjectManager::getTileDisplay(int world_x, int world_y, 
                                                        int entity_x, int entity_y) const {
    auto objects_here = getObjectsAt(world_x, world_y);
    
    if (objects_here.empty()) {
        return {' ', sf::Color::Black, sf::Color::Black};
    }
    
    BaseVegetationObject* object = objects_here[0];
    
    BaseVegetationObject::EntityContext context;
    context.entity_underneath = (entity_x == world_x && entity_y == world_y);
    context.entity_x = entity_x;
    context.entity_y = entity_y;
    
    int rel_x = world_x - object->getOriginX();
    int rel_y = world_y - object->getOriginY();
    
    auto tile_data = object->getTileAt(rel_x, rel_y, context);
    
    return {tile_data.character, tile_data.foreground, tile_data.background};
}

bool VegetationObjectManager::hasTileAt(int world_x, int world_y) const {
    return !getObjectsAt(world_x, world_y).empty();
}

bool VegetationObjectManager::isPassable(int world_x, int world_y) const {
    auto objects_here = getObjectsAt(world_x, world_y);
    
    for (auto* object : objects_here) {
        int rel_x = world_x - object->getOriginX();
        int rel_y = world_y - object->getOriginY();
        auto tile_data = object->getTileAt(rel_x, rel_y);
        
        if (tile_data.blocks_movement) {
            return false;
        }
    }
    
    return true;
}

std::vector<BaseVegetationObject*> VegetationObjectManager::getObjectsAt(int world_x, int world_y) const {
    std::vector<BaseVegetationObject*> result;
    
    uint64_t key = getSpatialKey(world_x, world_y);
    auto it = spatial_index.find(key);
    
    if (it != spatial_index.end()) {
        for (auto* object : it->second.objects) {
            if (object->containsPoint(world_x, world_y)) {
                result.push_back(object);
            }
        }
    }
    
    return result;
}

uint64_t VegetationObjectManager::getSpatialKey(int world_x, int world_y) const {
    int cell_x = world_x / SPATIAL_CELL_SIZE;
    int cell_y = world_y / SPATIAL_CELL_SIZE;
    return (static_cast<uint64_t>(cell_x) << 32) | static_cast<uint64_t>(cell_y);
}

void VegetationObjectManager::addToSpatialIndex(BaseVegetationObject* object) {
    int start_cell_x = object->getOriginX() / SPATIAL_CELL_SIZE;
    int start_cell_y = object->getOriginY() / SPATIAL_CELL_SIZE;
    int end_cell_x = (object->getOriginX() + object->getWidth() - 1) / SPATIAL_CELL_SIZE;
    int end_cell_y = (object->getOriginY() + object->getHeight() - 1) / SPATIAL_CELL_SIZE;
    
    for (int cell_y = start_cell_y; cell_y <= end_cell_y; ++cell_y) {
        for (int cell_x = start_cell_x; cell_x <= end_cell_x; ++cell_x) {
            uint64_t key = (static_cast<uint64_t>(cell_x) << 32) | static_cast<uint64_t>(cell_y);
            spatial_index[key].objects.push_back(object);
        }
    }
}

void VegetationObjectManager::rebuildSpatialIndex() {
    spatial_index.clear();
    for (const auto& object : objects) {
        addToSpatialIndex(object.get());
    }
}

void VegetationObjectManager::updateAnimations(float time_delta) {
    for (auto& object : objects) {
        if (object->isAnimated()) {
            object->updateAnimation(time_delta);
        }
    }
}

void VegetationObjectManager::clear() {
    objects.clear();
    spatial_index.clear();
}

void VegetationObjectManager::printStats() const {
    std::cout << "    Multi-Tile Objects: Generated " << objects.size() << " objects:" << std::endl;
    
    std::map<std::string, int> type_counts;
    for (const auto& object : objects) {
        type_counts[object->getObjectType()]++;
    }
    
    for (const auto& pair : type_counts) {
        std::cout << "      " << pair.first << ": " << pair.second << std::endl;
    }
}

} // namespace MultiTileObjects
} // namespace Vegetation
} // namespace Systems
} // namespace World