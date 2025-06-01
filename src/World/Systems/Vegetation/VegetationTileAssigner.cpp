// File: EmergentKingdoms/src/World/Systems/Vegetation/VegetationTileAssigner.cpp
#include "VegetationTileAssigner.h"
#include "../../Map.h"
#include "../../Tile.h"
#include "VegetationConfig.h"
#include "MultiTileObjects/VegetationObjectManager.h"
#include "MultiTileObjects/Trees/AncientOakTree.h"
#include "MultiTileObjects/Trees/YoungTree.h"
#include "MultiTileObjects/Boulders/ResourceBoulder.h"
#include <iostream>
#include <random>
#include <algorithm>

namespace World {
namespace Systems {
namespace Vegetation {

VegetationTileAssigner::VegetationTileAssigner() 
    : object_manager(std::make_unique<MultiTileObjects::VegetationObjectManager>()) {
}

VegetationTileAssigner::~VegetationTileAssigner() = default;

void VegetationTileAssigner::process(WorldData& world_data, unsigned int base_world_seed, int step_seed_offset) {
    unsigned int vegetation_seed = base_world_seed + static_cast<unsigned int>(step_seed_offset);
    std::cout << "  Vegetation: Creating dense multi-tile medieval landscape..." << std::endl;
    
    // Generate all multi-tile objects first
    object_manager->generateObjects(world_data, vegetation_seed);
    
    // ===== NEW: REGISTER OBJECT MANAGER WITH MAP FOR RENDERING =====
    if (world_data.map_context) {
        world_data.map_context->setVegetationObjectManager(object_manager.get());
    }
    
    // Then apply single-tile vegetation to remaining areas (with reduced density since we have multi-tile objects)
    applySingleTileVegetation(world_data, vegetation_seed);
    
    // Finally, apply enhanced grass animation to all grass areas
    applyGrassAnimation(world_data, vegetation_seed);
    
    std::cout << "  Vegetation: Created " << object_manager->getObjectCount() 
              << " multi-tile objects with flowing grass fields." << std::endl;
}

void VegetationTileAssigner::applySingleTileVegetation(WorldData& world_data, unsigned int seed) {
    std::mt19937 rng(seed + 5000);
    std::uniform_real_distribution<float> dist_0_1(0.0f, 1.0f);
    
    // Apply single-tile vegetation to areas not occupied by multi-tile objects
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            if (index >= world_data.map_context->getTilesRef().size()) continue;
            
            // Skip if this tile is occupied by a multi-tile object
            if (object_manager->hasTileAt(x, y)) {
                continue;
            }
            
            BaseTileType current_type = world_data.map_context->getTilesRef()[index].base_type;
            
            // Only place single-tile vegetation on suitable base terrain
            if (!isSuitableForVegetation(current_type)) {
                continue;
            }
            
            // Determine what single-tile vegetation to place
            BaseTileType vegetation_type = determineSingleTileVegetationType(x, y, world_data, rng, dist_0_1);
            
            if (vegetation_type != current_type) {
                replaceSingleTileVegetation(world_data, index, vegetation_type, x, y);
            }
        }
    }
}

void VegetationTileAssigner::applyGrassAnimation(WorldData& world_data, unsigned int seed) {
    // Generate wind patterns for enhanced grass animation
    
    for (int y = 0; y < world_data.map_height; ++y) {
        for (int x = 0; x < world_data.map_width; ++x) {
            size_t index = static_cast<size_t>(y) * world_data.map_width + x;
            
            if (index >= world_data.map_context->getTilesRef().size()) continue;
            
            BaseTileType current_type = world_data.map_context->getTilesRef()[index].base_type;
            
            // Apply wind animation to grass tiles
            if (current_type == BaseTileType::MEADOW || 
                current_type == BaseTileType::PLAINS ||
                current_type == BaseTileType::DRY_PLAINS) {
                
                // Create obvious wind patterns
                float wind_noise = getWindNoise(x, y, seed);
                float wind_strength = std::abs(std::sin(wind_noise * 6.28f)) * 0.8f + 0.2f;
                
                // Store wind animation data
                world_data.map_context->getTilesRef()[index].animation_offset = wind_noise;
                world_data.map_context->getTilesRef()[index].wave_strand_intensity = wind_strength;
                
                // Mark as animated grass
                if (wind_strength > 0.5f) {
                    world_data.map_context->getTilesRef()[index].base_type = BaseTileType::FLOWING_GRASS;
                }
            }
        }
    }
}

BaseTileType VegetationTileAssigner::determineSingleTileVegetationType(int x, int y, WorldData& world_data, 
                                                                       std::mt19937& rng, 
                                                                       std::uniform_real_distribution<float>& dist) {
    size_t index = static_cast<size_t>(y) * world_data.map_width + x;
    float height = world_data.heightmap_data[index];
    BaseTileType current_type = world_data.map_context->getTilesRef()[index].base_type;
    
    // FIXED: Very sparse single-tile placement since we have large multi-tile objects now
    // Reduced all probabilities significantly
    
    // Very sparse small flowers and herbs only (reduced from 0.02f to 0.005f)
    if (current_type == BaseTileType::MEADOW && dist(rng) < 0.005f) {
        float flower_roll = dist(rng);
        if (flower_roll < 0.3f) {
            return BaseTileType::WILDFLOWERS;
        } else if (flower_roll < 0.6f) {
            return BaseTileType::HERB_PATCH;
        }
    }
    
    // Very sparse small bushes (reduced from 0.01f to 0.003f)
    if ((current_type == BaseTileType::HILLS || current_type == BaseTileType::MOOR) && dist(rng) < 0.003f) {
        return (dist(rng) < 0.5f) ? BaseTileType::BERRY_BUSH : BaseTileType::WILD_ROSES;
    }
    
    // Very sparse small rock outcrops (reduced from 0.005f to 0.001f)
    if (height > 0.3f && dist(rng) < 0.001f) {
        return BaseTileType::ROCK_OUTCROP;
    }
    
    return current_type; // No change
}

void VegetationTileAssigner::replaceSingleTileVegetation(WorldData& world_data, size_t index, 
                                                        BaseTileType vegetation_type, int /* x */, int /* y */) {
    if (index >= world_data.map_context->getTilesRef().size()) return;
    
    // Get current tile properties
    float height = world_data.heightmap_data[index];
    float slope = world_data.slope_map[index];
    SlopeAspect aspect = (index < world_data.aspect_map.size()) ? 
                        world_data.aspect_map[index] : SlopeAspect::FLAT;
    
    // Create the new vegetation tile
    world_data.map_context->getTilesRef()[index] = Tile::create(
        vegetation_type,
        height,
        slope,
        aspect,
        -1,  // distance_to_land (not applicable)
        world_data.map_context->getTilesRef()[index].distance_to_water,
        0.0f,  // animation_offset
        0.0f,  // wave_strand_intensity 
        false  // is_marsh_water_patch
    );
}

float VegetationTileAssigner::getWindNoise(int x, int y, unsigned int seed) const {
    // Generate obvious wind patterns using multiple noise octaves
    unsigned int noise_seed = seed + x * 73856093 + y * 19349663;
    std::mt19937 rng(noise_seed);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    float noise = 0.0f;
    float amplitude = 1.0f;
    float frequency = 0.01f;
    
    // Multiple octaves for complex wind patterns
    for (int octave = 0; octave < 3; ++octave) {
        float octave_noise = dist(rng);
        noise += octave_noise * amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
        rng.seed(noise_seed + octave * 12345);
    }
    
    return noise * 0.5f + 0.5f; // Normalize to 0-1
}

bool VegetationTileAssigner::isSuitableForVegetation(BaseTileType base_type) const {
    // Single-tile vegetation can grow on these terrain types
    return base_type == BaseTileType::MEADOW ||
           base_type == BaseTileType::PLAINS ||
           base_type == BaseTileType::DRY_PLAINS ||
           base_type == BaseTileType::HILLS ||
           base_type == BaseTileType::MOOR ||
           base_type == BaseTileType::PLATEAU_GRASS;
}

Core::ScreenCell VegetationTileAssigner::getMultiTileObjectDisplay(int world_x, int world_y, 
                                                                  int entity_x, int entity_y) const {
    return object_manager->getTileDisplay(world_x, world_y, entity_x, entity_y);
}

bool VegetationTileAssigner::hasMultiTileObjectAt(int world_x, int world_y) const {
    return object_manager->hasTileAt(world_x, world_y);
}

bool VegetationTileAssigner::isPassable(int world_x, int world_y) const {
    return object_manager->isPassable(world_x, world_y);
}

} // namespace Vegetation
} // namespace Systems
} // namespace World