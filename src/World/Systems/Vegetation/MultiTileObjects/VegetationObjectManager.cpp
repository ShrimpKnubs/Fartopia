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
    // FIXED: Significantly increased density for much more vegetation
    config.tree_density = 0.25f;        // Increased from 0.05f to 0.25f for dense forests
    config.ancient_tree_rarity = 0.35f;  // Increased from 0.15f to 0.35f for more ancient trees
    config.min_tree_spacing = 3;        // Reduced from 4 to 3 for closer clusters
    config.boulder_density = 0.08f;     // Increased from 0.02f to 0.08f for more boulders
}

void VegetationObjectManager::generateObjects(WorldData& world_data, unsigned int base_seed) {
    clear();
    
    std::cout << "    Multi-Tile Objects: Generating dense clustered vegetation..." << std::endl;
    
    // Generate in order of priority (largest objects first to avoid conflicts)
    generateBoulders(world_data, base_seed + 1000);
    generateForestClusters(world_data, base_seed + 2000);  // NEW: Generate forests as clusters
    generateGrassFields(world_data, base_seed + 3000);
    
    // Rebuild spatial index for fast lookups
    rebuildSpatialIndex();
    
    printStats();
}

void VegetationObjectManager::generateForestClusters(WorldData& world_data, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist_0_1(0.0f, 1.0f);
    std::uniform_int_distribution<int> cluster_size_dist(8, 25);  // FIXED: Larger forest clusters
    std::uniform_int_distribution<int> grove_size_dist(4, 12);    // FIXED: Larger grove sizes
    
    // FIXED: Calculate much higher numbers of forest clusters based on increased density
    int total_forest_clusters = static_cast<int>(world_data.map_width * world_data.map_height * config.tree_density / 15000.0f); // Reduced divisor
    int total_groves = total_forest_clusters * 3;  // More groves than forests
    
    std::cout << "      Placing " << total_forest_clusters << " forest clusters and " 
              << total_groves << " tree groves..." << std::endl;
    
    // Generate forest clusters
    for (int cluster = 0; cluster < total_forest_clusters; ++cluster) {
        int cluster_size = cluster_size_dist(rng);
        generateSingleForestCluster(world_data, seed + cluster * 1000, cluster_size, true);
    }
    
    // Generate smaller groves
    for (int grove = 0; grove < total_groves; ++grove) {
        int grove_size = grove_size_dist(rng);
        generateSingleForestCluster(world_data, seed + grove * 2000 + 500000, grove_size, false);
    }
}

void VegetationObjectManager::generateSingleForestCluster(WorldData& world_data, unsigned int cluster_seed, 
                                                         int cluster_size, bool is_large_forest) {
    std::mt19937 rng(cluster_seed);
    std::uniform_real_distribution<float> dist_0_1(0.0f, 1.0f);
    
    // Find a suitable center location for the cluster
    std::pair<int, int> center = findClusterCenter(world_data, cluster_seed);
    if (center.first == -1) return; // No suitable location found
    
    int cluster_radius = is_large_forest ? 35 : 20;  // FIXED: Larger radius for more impressive forests
    float ancient_tree_chance = is_large_forest ? config.ancient_tree_rarity : config.ancient_tree_rarity * 0.7f;
    
    std::vector<std::pair<int, int>> tree_positions;
    
    // Generate tree positions in a natural cluster pattern
    for (int attempt = 0; attempt < cluster_size * 4; ++attempt) { // FIXED: More attempts for denser packing
        if (tree_positions.size() >= static_cast<size_t>(cluster_size)) break;
        
        // Use gaussian distribution for natural clustering
        float distance = std::abs(std::normal_distribution<float>(0.0f, cluster_radius * 0.35f)(rng)); // FIXED: Tighter clustering
        if (distance > cluster_radius) continue;
        
        float angle = dist_0_1(rng) * 2.0f * 3.14159f;
        int tree_x = center.first + static_cast<int>(distance * std::cos(angle));
        int tree_y = center.second + static_cast<int>(distance * std::sin(angle));
        
        // Check if this position is suitable and not too close to existing trees
        if (isValidTreePosition(tree_x, tree_y, world_data, tree_positions)) {
            tree_positions.emplace_back(tree_x, tree_y);
        }
    }
    
    // Place trees at the generated positions
    for (const auto& pos : tree_positions) {
        bool is_ancient = dist_0_1(rng) < ancient_tree_chance;
        
        std::unique_ptr<BaseVegetationObject> tree;
        if (is_ancient) {
            tree = std::make_unique<Trees::AncientOakTree>(pos.first, pos.second, rng());
        } else {
            tree = std::make_unique<Trees::YoungTree>(pos.first, pos.second, rng());
        }
        
        if (canPlaceObject(*tree, world_data)) {
            addObject(std::move(tree));
        }
    }
}

std::pair<int, int> VegetationObjectManager::findClusterCenter(const WorldData& world_data, unsigned int seed) const {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> x_dist(50, world_data.map_width - 50);
    std::uniform_int_distribution<int> y_dist(50, world_data.map_height - 50);
    
    int attempts = 0;
    const int max_attempts = 50; // FIXED: Reduced attempts for faster generation
    
    while (attempts < max_attempts) {
        int x = x_dist(rng);
        int y = y_dist(rng);
        
        float suitability = getTerrainSuitability(x, y, world_data, "forest_center");
        if (suitability > 0.4f) { // FIXED: Reduced threshold from 0.6f to 0.4f for more placement locations
            return {x, y};
        }
        attempts++;
    }
    
    return {-1, -1}; // No suitable location found
}

bool VegetationObjectManager::isValidTreePosition(int x, int y, const WorldData& world_data, 
                                                 const std::vector<std::pair<int, int>>& existing_positions) const {
    // Check terrain suitability
    float suitability = getTerrainSuitability(x, y, world_data, "ancient_tree");
    if (suitability < 0.3f) return false; // FIXED: Reduced threshold from 0.4f to 0.3f
    
    // Check distance from existing trees (allow closer spacing in clusters)
    int min_distance = 4;  // FIXED: Reduced from 6 to 4 for denser clustering
    for (const auto& existing : existing_positions) {
        int dx = x - existing.first;
        int dy = y - existing.second;
        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance < min_distance) return false;
    }
    
    // Check distance from existing objects (reduced spacing)
    for (const auto& existing_object : objects) {
        int dx = x - existing_object->getOriginX();
        int dy = y - existing_object->getOriginY();
        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance < config.min_tree_spacing) return false;
    }
    
    return true;
}

void VegetationObjectManager::generateTrees(WorldData& world_data, unsigned int seed) {
    // This method is now replaced by generateForestClusters
    // Keep it for compatibility but make it generate more scattered trees
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist_0_1(0.0f, 1.0f);
    
    // FIXED: Increased scattered trees significantly
    int scattered_trees = static_cast<int>(world_data.map_width * world_data.map_height * 0.02f / 10000.0f); // Increased from 0.005f
    
    std::cout << "      Placing " << scattered_trees << " scattered individual trees..." << std::endl;
    
    auto scattered_locations = findSuitableLocations(world_data, "young_tree", scattered_trees, seed);
    for (const auto& location : scattered_locations) {
        auto tree = std::make_unique<Trees::YoungTree>(location.first, location.second, rng());
        if (canPlaceObject(*tree, world_data)) {
            addObject(std::move(tree));
        }
    }
}

void VegetationObjectManager::generateBoulders(WorldData& world_data, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist_0_1(0.0f, 1.0f);
    
    // FIXED: Calculate boulder placement - significantly more boulders
    int total_boulder_count = static_cast<int>(world_data.map_width * world_data.map_height * config.boulder_density / 5000.0f); // Reduced divisor from 10000 to 5000
    
    std::cout << "      Placing " << total_boulder_count << " boulder formations..." << std::endl;
    
    auto boulder_locations = findSuitableLocations(world_data, "boulder", total_boulder_count, seed);
    
    for (const auto& location : boulder_locations) {
        // Determine boulder size - more variety with larger boulders
        float size_roll = dist_0_1(rng);
        Boulders::ResourceBoulder::BoulderSize size;
        if (size_roll < 0.2f) size = Boulders::ResourceBoulder::BoulderSize::SMALL;      // Reduced from 0.3f
        else if (size_roll < 0.5f) size = Boulders::ResourceBoulder::BoulderSize::MEDIUM; // Increased chance
        else if (size_roll < 0.8f) size = Boulders::ResourceBoulder::BoulderSize::LARGE;  // Increased chance
        else size = Boulders::ResourceBoulder::BoulderSize::MASSIVE;                      // More massive boulders
        
        // Determine if it has resources - FIXED: Increased resource chance
        Boulders::ResourceBoulder::ResourceType resource = Boulders::ResourceBoulder::ResourceType::NONE;
        if (dist_0_1(rng) < 0.5f) { // FIXED: Increased from config.resource_boulder_chance to 0.5f for more resources
            float resource_roll = dist_0_1(rng);
            if (resource_roll < 0.08f) { // FIXED: Increased gold chance
                resource = Boulders::ResourceBoulder::ResourceType::GOLD_VEINS;
            } else if (resource_roll < 0.18f) { // FIXED: Increased silver chance
                resource = Boulders::ResourceBoulder::ResourceType::SILVER_VEINS;
            } else if (resource_roll < 0.5f) { // Iron more common
                resource = Boulders::ResourceBoulder::ResourceType::IRON_DEPOSITS;
            } else if (resource_roll < 0.8f) { // Copper quite common
                resource = Boulders::ResourceBoulder::ResourceType::COPPER_DEPOSITS;
            }
        }
        
        auto boulder = std::make_unique<Boulders::ResourceBoulder>(
            location.first, location.second, rng(), size, resource);
        
        if (canPlaceObject(*boulder, world_data)) {
            addObject(std::move(boulder));
        }
    }
}

void VegetationObjectManager::generateGrassFields(WorldData& /* world_data */, unsigned int /* seed */) {
    // TODO: Implement large grass field generation
    // For now, grass animation is handled by the tile assigner
    std::cout << "      Grass fields: Delegated to tile-level wind animation" << std::endl;
}

std::vector<std::pair<int, int>> VegetationObjectManager::findSuitableLocations(
    const WorldData& world_data, const std::string& object_type, int count, unsigned int seed) const {
    
    std::vector<std::pair<int, int>> locations;
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> x_dist(20, world_data.map_width - 40);
    std::uniform_int_distribution<int> y_dist(20, world_data.map_height - 40);
    
    int attempts = 0;
    const int max_attempts = count * 3; // FIXED: Reduced attempts for faster generation
    
    while (locations.size() < static_cast<size_t>(count) && attempts < max_attempts) {
        int x = x_dist(rng);
        int y = y_dist(rng);
        
        float suitability = getTerrainSuitability(x, y, world_data, object_type);
        
        if (suitability > 0.3f) { // FIXED: Reduced threshold from 0.5f to 0.3f for more placement
            // Check minimum spacing from existing objects
            bool too_close = false;
            int min_spacing = (object_type == "boulder") ? config.min_boulder_spacing : config.min_tree_spacing;
            
            for (const auto& existing : locations) {
                int dx = x - existing.first;
                int dy = y - existing.second;
                float distance = std::sqrt(dx * dx + dy * dy);
                if (distance < min_spacing) {
                    too_close = true;
                    break;
                }
            }
            
            if (!too_close) {
                locations.emplace_back(x, y);
            }
        }
        
        attempts++;
    }
    
    return locations;
}

float VegetationObjectManager::getTerrainSuitability(int x, int y, const WorldData& world_data, 
                                                    const std::string& object_type) const {
    if (x < 0 || x >= world_data.map_width || y < 0 || y >= world_data.map_height) {
        return 0.0f;
    }
    
    size_t index = static_cast<size_t>(y) * world_data.map_width + x;
    if (index >= world_data.heightmap_data.size() || index >= world_data.slope_map.size()) {
        return 0.0f;
    }
    
    float height = world_data.heightmap_data[index];
    float slope = world_data.slope_map[index];
    
    // Check if it's water
    if (world_data.is_river_tile[index] || world_data.is_lake_tile[index]) {
        return 0.0f;
    }
    
    if (object_type == "ancient_tree" || object_type == "forest_center") {
        // Ancient trees and forest centers prefer mid-elevations, gentle slopes
        if (height < 0.03f || height > 0.8f || slope > 0.04f) return 0.0f; // FIXED: More lenient requirements
        return 0.7f + (0.5f - std::abs(height - 0.3f)) * 0.3f; // Prefer ~30% elevation
    } else if (object_type == "young_tree") {
        // Young trees are more adaptable
        if (height < 0.02f || height > 0.85f || slope > 0.06f) return 0.0f; // FIXED: More lenient requirements
        return 0.5f + (1.0f - slope * 15.0f) * 0.3f; // Prefer gentler slopes
    } else if (object_type == "boulder") {
        // Boulders can be anywhere above water, prefer higher/steeper areas
        if (height < 0.01f) return 0.0f;
        return 0.3f + height * 0.4f + slope * 1.5f; // Prefer hills and slopes
    }
    
    return 0.4f; // Default moderate suitability (increased from 0.5f)
}

bool VegetationObjectManager::canPlaceObject(const BaseVegetationObject& object, WorldData& world_data) const {
    // Check if object can place at its location
    if (!object.canPlaceAt(object.getOriginX(), object.getOriginY(), 
                          world_data.heightmap_data, world_data.slope_map,
                          world_data.map_width, world_data.map_height)) {
        return false;
    }
    
    // Check for collisions with existing objects
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
    // Find objects at this location
    auto objects_here = getObjectsAt(world_x, world_y);
    
    if (objects_here.empty()) {
        return {' ', sf::Color::Black, sf::Color::Black}; // No object
    }
    
    // Use the first object (highest priority)
    BaseVegetationObject* object = objects_here[0];
    
    // Set up entity context
    BaseVegetationObject::EntityContext context;
    context.entity_underneath = (entity_x == world_x && entity_y == world_y);
    context.entity_x = entity_x;
    context.entity_y = entity_y;
    
    // Get relative coordinates within the object
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
    
    // Use spatial indexing for fast lookup
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
    // Add object to all spatial cells it touches
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