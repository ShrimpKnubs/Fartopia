// File: EmergentKingdoms/src/World/Map.cpp
#include "Map.h"
#include "Systems/Vegetation/MultiTileObjects/VegetationObjectManager.h"
#include "GenerationSteps/BaseHeightGenerator.h"
#include "GenerationSteps/ThermalEroder.h"
#include "GenerationSteps/HydraulicEroder.h"
#include "GenerationSteps/SlopeAspectCalculator.h"
#include "GenerationSteps/BorderWallPlacer.h"
#include "Systems/Mountains/MountainGenerator.h"
#include "Systems/Rivers/RiverNetworkSimulator.h"
#include "Systems/Lakes/LakeFormer.h"
#include "TileAssigner.h"
#include "../Core/BaseConfig.h"
#include <iostream>
#include <stdexcept>
#include <cassert>

namespace World {

Map::Map(int width, int height, unsigned int seed)
    : width(width), height(height), seed(seed), vegetation_object_manager(nullptr) {
    
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Map dimensions must be positive");
    }
    
    tiles.resize(static_cast<size_t>(width) * height);
    
    // Initialize world data structures
    initializeWorldData();
    
    // Set up the generation pipeline
    initializeGenerationSteps();
    
    std::cout << "Map initialized: " << width << "x" << height << " (seed: " << seed << ")" << std::endl;
}

Map::~Map() {
    // Vegetation object manager is owned externally, so don't delete it
}

void Map::initializeWorldData() {
    size_t map_size = static_cast<size_t>(width) * height;
    
    heightmap_data.resize(map_size, 0.0f);
    is_river_tile.resize(map_size, false);
    is_lake_tile.resize(map_size, false);
    slope_map.resize(map_size, 0.0f);
    aspect_map.resize(map_size, SlopeAspect::FLAT);
    lake_has_waves_map.resize(map_size, false);
    
    std::cout << "World data structures initialized for " << map_size << " tiles." << std::endl;
}

void Map::initializeGenerationSteps() {
    std::cout << "Setting up world generation pipeline..." << std::endl;
    
    // Clear any existing steps
    generation_steps.clear();
    
    // Core terrain generation
    generation_steps.push_back(std::make_unique<Generation::BaseHeightGenerator>());
    generation_steps.push_back(std::make_unique<Generation::ThermalEroder>());
    generation_steps.push_back(std::make_unique<Generation::HydraulicEroder>());
    generation_steps.push_back(std::make_unique<Generation::SlopeAspectCalculator>());
    
    // Terrain feature generation
    generation_steps.push_back(std::make_unique<Systems::Mountains::MountainGenerator>());
    generation_steps.push_back(std::make_unique<Systems::Rivers::RiverNetworkSimulator>());
    generation_steps.push_back(std::make_unique<Systems::Lakes::LakeFormer>());
    
    // Final tile assignment (includes vegetation)
    generation_steps.push_back(std::make_unique<TileAssigner>());
    
    // Border walls (last step)
    generation_steps.push_back(std::make_unique<Generation::BorderWallPlacer>());
    
    std::cout << "Generation pipeline configured with " << generation_steps.size() << " steps." << std::endl;
}

void Map::generate() {
    std::cout << "Starting world generation..." << std::endl;
    
    runGenerationPipeline();
    
    std::cout << "World generation completed successfully." << std::endl;
}

void Map::runGenerationPipeline() {
    // Create world data wrapper for generation steps - FIXED: Include lake_has_waves_map
    WorldData world_data(
        heightmap_data, is_river_tile, is_lake_tile,
        slope_map, aspect_map, lake_has_waves_map,  // FIXED: Added lake_has_waves_map
        width, height, this
    );
    
    // Run each generation step
    int step_offset = 0;
    for (auto& step : generation_steps) {
        std::cout << "Running generation step: " << step->getName() << std::endl;
        
        try {
            step->process(world_data, seed, step_offset);
            step_offset += 1000; // Ensure unique seeds for each step
        } catch (const std::exception& e) {
            std::cerr << "Error in generation step '" << step->getName() << "': " << e.what() << std::endl;
            throw;
        }
    }
}

const Tile& Map::getTile(int x, int y) const {
    validateCoordinates(x, y);
    
    // Handle cylindrical wrapping on X-axis
    x = ((x % width) + width) % width;
    
    size_t index = getIndex(x, y);
    return tiles[index];
}

void Map::setTile(int x, int y, const Tile& tile) {
    validateCoordinates(x, y);
    
    // Handle cylindrical wrapping on X-axis
    x = ((x % width) + width) % width;
    
    size_t index = getIndex(x, y);
    tiles[index] = tile;
}

// ===== VEGETATION OBJECT SYSTEM INTEGRATION =====

void Map::setVegetationObjectManager(Systems::Vegetation::MultiTileObjects::VegetationObjectManager* manager) {
    vegetation_object_manager = manager;
    std::cout << "Vegetation object manager registered with map." << std::endl;
}

bool Map::hasVegetationObjectAt(int x, int y) const {
    if (!vegetation_object_manager) {
        return false;
    }
    
    // Handle cylindrical wrapping on X-axis
    x = ((x % width) + width) % width;
    
    // Clamp Y coordinate (no wrapping on Y-axis)
    if (y < 0 || y >= height) {
        return false;
    }
    
    return vegetation_object_manager->hasTileAt(x, y);
}

Core::ScreenCell Map::getVegetationObjectDisplay(int x, int y, int entity_x, int entity_y) const {
    if (!vegetation_object_manager) {
        return {' ', sf::Color::Black, sf::Color::Black};
    }
    
    // Handle cylindrical wrapping on X-axis
    x = ((x % width) + width) % width;
    
    // Clamp Y coordinate (no wrapping on Y-axis)
    if (y < 0 || y >= height) {
        return {' ', sf::Color::Black, sf::Color::Black};
    }
    
    return vegetation_object_manager->getTileDisplay(x, y, entity_x, entity_y);
}

bool Map::isVegetationPassable(int x, int y) const {
    if (!vegetation_object_manager) {
        return true; // No vegetation objects, so passable
    }
    
    // Handle cylindrical wrapping on X-axis
    x = ((x % width) + width) % width;
    
    // Clamp Y coordinate (no wrapping on Y-axis)
    if (y < 0 || y >= height) {
        return false; // Out of bounds
    }
    
    return vegetation_object_manager->isPassable(x, y);
}

// ===== HELPER METHODS =====

void Map::validateCoordinates(int /* x */, int y) const {
    // X coordinates are handled by cylindrical wrapping, so we don't validate them
    // Only validate Y coordinates since they have hard boundaries
    if (y < 0 || y >= height) {
        throw std::out_of_range("Y coordinate " + std::to_string(y) + 
                               " is out of range [0, " + std::to_string(height - 1) + "]");
    }
}

size_t Map::getIndex(int x, int y) const {
    // Ensure coordinates are valid after any wrapping
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    
    return static_cast<size_t>(y) * width + x;
}

} // namespace World