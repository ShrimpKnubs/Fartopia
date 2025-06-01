// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/Boulders/ResourceBoulder.cpp
#include "ResourceBoulder.h"
#include "../../../../Core/BaseConfig.h"
#include <cmath>
#include <random>

namespace World {
namespace Systems {
namespace Vegetation {
namespace MultiTileObjects {
namespace Boulders {

ResourceBoulder::ResourceBoulder(int origin_x, int origin_y, unsigned int seed, 
                                BoulderSize size, ResourceType resource)
    : BaseVegetationObject(origin_x, origin_y, seed), sparkle_phase(0.0f), 
      moss_growth_phase(0.0f), resource_type(resource), boulder_size(size) {
    
    // Set dimensions based on boulder size
    int size_tiles = getSizeInTiles(size);
    setDimensions(size_tiles, size_tiles);
    
    has_animation = (resource != ResourceType::NONE); // Animate if has resources
    
    // Initialize boulder parameters
    determineBoulderSize();
    
    // FIXED: Generate the boulder pattern immediately after construction
    generatePattern();
}

void ResourceBoulder::determineBoulderSize() {
    std::mt19937 rng(random_seed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    params.size = boulder_size;
    params.resource_type = resource_type;
    
    // Set base colors
    params.base_stone_color = varyColor(Colors::MOSSY_BOULDER_BASE, 0.1f);
    params.moss_color = varyColor(Colors::MOSSY_BOULDER_MOSS, 0.08f);
    
    // Resource-specific colors
    switch (resource_type) {
        case ResourceType::GOLD_VEINS:
            params.resource_color = Colors::GOLD_VEIN_GLEAM;
            params.resource_density = 0.1f + dist(rng) * 0.1f;
            params.vein_count = 2 + static_cast<int>(dist(rng) * 3);
            break;
        case ResourceType::SILVER_VEINS:
            params.resource_color = Colors::SILVER_LODE_GLEAM;
            params.resource_density = 0.12f + dist(rng) * 0.1f;
            params.vein_count = 2 + static_cast<int>(dist(rng) * 4);
            break;
        case ResourceType::IRON_DEPOSITS:
            params.resource_color = Colors::IRON_ORE_METAL;
            params.resource_density = 0.15f + dist(rng) * 0.1f;
            params.vein_count = 3 + static_cast<int>(dist(rng) * 4);
            break;
        case ResourceType::COPPER_DEPOSITS:
            params.resource_color = Colors::COPPER_DEPOSIT_GLEAM;
            params.resource_density = 0.13f + dist(rng) * 0.1f;
            params.vein_count = 2 + static_cast<int>(dist(rng) * 3);
            break;
        default:
            params.resource_density = 0.0f;
            params.vein_count = 0;
            break;
    }
    
    // Boulder characteristics
    params.moss_coverage = 0.2f + dist(rng) * 0.4f;
    params.weathering_factor = 0.3f + dist(rng) * 0.3f;
}

void ResourceBoulder::generatePattern() {
    // Clear pattern with proper terrain background
    sf::Color terrain_bg = getTerrainBackground();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            setTile(x, y, ' ', sf::Color::Black, terrain_bg, false, false);
        }
    }
    
    // Generate in layers
    generateStoneBase();
    generateResourceVeins();
    generateMossPatches();
    generateCracksAndTexture();
}

void ResourceBoulder::generateStoneBase() {
    int center_x = width / 2;
    int center_y = height / 2;
    int radius = std::min(width, height) / 2;
    
    // Create roughly circular boulder shape
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float distance = std::sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
            
            // Add some irregularity to the shape
            float noise = getProceduralNoise(x, y, 0.3f);
            float effective_radius = radius * (0.8f + noise * 0.4f);
            
            if (distance <= effective_radius) {
                char stone_char = selectStoneCharacter(x, y);
                sf::Color stone_color = getStoneColor(x, y);
                // FIXED: Use terrain background blended with stone color instead of pure stone
                sf::Color bg_color = Tile::interpolateColor(getTerrainBackground(), params.base_stone_color, 0.8f);
                
                setTile(x, y, stone_char, stone_color, bg_color, true, false); // blocks movement
            }
        }
    }
}

void ResourceBoulder::generateResourceVeins() {
    if (resource_type == ResourceType::NONE || params.vein_count == 0) {
        return;
    }
    
    std::mt19937 rng(random_seed + 1000);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (int vein = 0; vein < params.vein_count; ++vein) {
        // Start from a random edge
        int start_x, start_y;
        if (dist(rng) < 0.5f) {
            start_x = (dist(rng) < 0.5f) ? 0 : width - 1;
            start_y = static_cast<int>(dist(rng) * height);
        } else {
            start_x = static_cast<int>(dist(rng) * width);
            start_y = (dist(rng) < 0.5f) ? 0 : height - 1;
        }
        
        // Generate vein towards center with some randomness
        int length = 3 + static_cast<int>(dist(rng) * (std::min(width, height) / 2));
        float thickness = 0.8f + dist(rng) * 0.4f;
        
        generateVein(start_x, start_y, length, thickness);
    }
}

void ResourceBoulder::generateVein(int start_x, int start_y, int length, float thickness) {
    std::mt19937 rng(random_seed + start_x * 1000 + start_y * 100);
    std::uniform_real_distribution<float> dist(-0.3f, 0.3f);
    
    int center_x = width / 2;
    int center_y = height / 2;
    
    for (int step = 0; step < length; ++step) {
        float progress = static_cast<float>(step) / length;
        
        // Move towards center with some randomness
        int vein_x = static_cast<int>(start_x + (center_x - start_x) * progress + dist(rng) * 2);
        int vein_y = static_cast<int>(start_y + (center_y - start_y) * progress + dist(rng) * 2);
        
        if (vein_x >= 0 && vein_x < width && vein_y >= 0 && vein_y < height) {
            addVeinSegment(vein_x, vein_y, thickness);
        }
    }
}

void ResourceBoulder::addVeinSegment(int x, int y, float thickness) {
    // Only add vein to existing stone tiles
    ObjectTile& tile = getTileRef(x, y);
    if (tile.character == ' ') return; // No stone here
    
    float noise = getProceduralNoise(x, y, 0.5f);
    if (noise < params.resource_density * thickness) {
        char resource_char = selectResourceCharacter(resource_type);
        sf::Color resource_color = getResourceColor(resource_type);
        
        setTile(x, y, resource_char, resource_color, tile.background, true, false);
    }
}

void ResourceBoulder::generateMossPatches() {
    std::mt19937 rng(random_seed + 2000);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    int moss_patch_count = static_cast<int>(params.moss_coverage * width * height / 10);
    
    for (int patch = 0; patch < moss_patch_count; ++patch) {
        int moss_x = static_cast<int>(dist(rng) * width);
        int moss_y = static_cast<int>(dist(rng) * height);
        int moss_radius = 1 + static_cast<int>(dist(rng) * 2);
        
        addMossPatch(moss_x, moss_y, moss_radius);
    }
}

void ResourceBoulder::addMossPatch(int center_x, int center_y, int radius) {
    for (int y = center_y - radius; y <= center_y + radius; ++y) {
        for (int x = center_x - radius; x <= center_x + radius; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                if (shouldHaveMoss(x, y, center_x, center_y, radius)) {
                    ObjectTile& tile = getTileRef(x, y);
                    if (tile.character != ' ') { // Only moss existing stone
                        char moss_char = selectMossCharacter(x, y);
                        sf::Color moss_fg = getMossColor(x, y);
                        
                        setTile(x, y, moss_char, moss_fg, tile.background, tile.blocks_movement, false);
                    }
                }
            }
        }
    }
}

bool ResourceBoulder::shouldHaveMoss(int x, int y, int center_x, int center_y, int radius) const {
    float distance = std::sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
    float noise = getProceduralNoise(x, y, 0.4f);
    return distance <= radius && noise > 0.3f;
}

void ResourceBoulder::generateCracksAndTexture() {
    // Add weathering details to existing tiles
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            ObjectTile& tile = getTileRef(x, y);
            if (tile.character != ' ') {
                float weathering_noise = getProceduralNoise(x, y, 0.6f);
                if (weathering_noise > 0.8f) {
                    // Add cracks or weathering
                    if (tile.character == 'O') {
                        tile.character = (weathering_noise > 0.9f) ? '8' : 'o';
                    }
                }
            }
        }
    }
}

char ResourceBoulder::selectStoneCharacter(int x, int y) const {
    float noise = getProceduralNoise(x, y, 0.5f);
    
    if (noise < 0.6f) return 'O';      // Solid stone
    else if (noise < 0.8f) return 'o'; // Smaller stones
    else return '8';                   // Weathered stone
}

char ResourceBoulder::selectResourceCharacter(ResourceType type) const {
    switch (type) {
        case ResourceType::GOLD_VEINS: return '$';
        case ResourceType::SILVER_VEINS: return '=';
        case ResourceType::IRON_DEPOSITS: return '#';
        case ResourceType::COPPER_DEPOSITS: return '+';
        default: return '.';
    }
}

char ResourceBoulder::selectMossCharacter(int /* x */, int /* y */) const {
    return '.'; // Simple moss character
}

sf::Color ResourceBoulder::getStoneColor(int /* x */, int /* y */) const {
    return varyColor(params.base_stone_color, 0.05f);
}

sf::Color ResourceBoulder::getResourceColor(ResourceType type, bool sparkling) const {
    sf::Color base_color;
    
    switch (type) {
        case ResourceType::GOLD_VEINS:
            base_color = sparkling ? Colors::GOLD_SPARKLE : Colors::GOLD_VEIN_GLEAM;
            break;
        case ResourceType::SILVER_VEINS:
            base_color = sparkling ? Colors::SILVER_SPARKLE : Colors::SILVER_LODE_GLEAM;
            break;
        case ResourceType::IRON_DEPOSITS:
            base_color = Colors::IRON_ORE_METAL;
            break;
        case ResourceType::COPPER_DEPOSITS:
            base_color = Colors::COPPER_DEPOSIT_GLEAM;
            break;
        default:
            base_color = Colors::ROCK_OUTCROP_GRAY;
            break;
    }
    
    return base_color;
}

sf::Color ResourceBoulder::getMossColor(int /* x */, int /* y */) const {
    return varyColor(params.moss_color, 0.05f);
}

// FIXED: Add method to get terrain background for boulders
sf::Color ResourceBoulder::getTerrainBackground() const {
    // Boulders can appear in various terrains, use neutral background
    return Core::LandColors::GRASS_MID_SLOPE;
}

void ResourceBoulder::updateAnimation(float time_delta) {
    if (resource_type == ResourceType::NONE) return;
    
    current_time += time_delta;
    updateResourceSparkle(time_delta);
    updateMossGrowth(time_delta);
}

void ResourceBoulder::updateResourceSparkle(float time_delta) {
    sparkle_phase += time_delta;
    
    // Update sparkle effects on resource tiles
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            ObjectTile& tile = getTileRef(x, y);
            if (tile.character == selectResourceCharacter(resource_type)) {
                // Occasional sparkle effect
                float sparkle_intensity = std::sin(sparkle_phase * 2.0f + x + y) * 0.5f + 0.5f;
                if (sparkle_intensity > 0.8f) {
                    tile.foreground = getResourceColor(resource_type, true);
                } else {
                    tile.foreground = getResourceColor(resource_type, false);
                }
            }
        }
    }
}

void ResourceBoulder::updateMossGrowth(float /* time_delta */) {
    // Moss slowly grows over time - very subtle animation
    moss_growth_phase = current_time * 0.1f;
}

bool ResourceBoulder::canPlaceAt(int world_x, int world_y, 
                                const std::vector<float>& heightmap,
                                const std::vector<float>& slope_map,
                                int map_width, int map_height) const {
    // Check bounds
    if (world_x < 0 || world_y < 0 || 
        world_x + width >= map_width || world_y + height >= map_height) {
        return false;
    }
    
    // Check center terrain
    int center_x = world_x + width / 2;
    int center_y = world_y + height / 2;
    
    if (center_x >= 0 && center_x < map_width && center_y >= 0 && center_y < map_height) {
        size_t index = static_cast<size_t>(center_y) * map_width + center_x;
        
        if (index < heightmap.size() && index < slope_map.size()) {
            return isValidTerrain(heightmap[index], slope_map[index]);
        }
    }
    
    return false;
}

bool ResourceBoulder::isValidTerrain(float height, float /* slope */) const {
    // Boulders can be placed almost anywhere above water
    return height >= 0.01f;
}

int ResourceBoulder::getSizeInTiles(BoulderSize size) const {
    switch (size) {
        case BoulderSize::SMALL: return 5;
        case BoulderSize::MEDIUM: return 8;
        case BoulderSize::LARGE: return 15;
        case BoulderSize::MASSIVE: return 25;
        default: return 8;
    }
}

int ResourceBoulder::getResourceYield() const {
    return getBaseResourceYield(resource_type, boulder_size);
}

int ResourceBoulder::getBaseResourceYield(ResourceType type, BoulderSize size) {
    int base_yield = 0;
    
    switch (type) {
        case ResourceType::GOLD_VEINS: base_yield = 50; break;
        case ResourceType::SILVER_VEINS: base_yield = 75; break;
        case ResourceType::IRON_DEPOSITS: base_yield = 150; break;
        case ResourceType::COPPER_DEPOSITS: base_yield = 125; break;
        default: base_yield = 0; break;
    }
    
    // Scale by boulder size
    float size_multiplier = 1.0f;
    switch (size) {
        case BoulderSize::SMALL: size_multiplier = 0.5f; break;
        case BoulderSize::MEDIUM: size_multiplier = 1.0f; break;
        case BoulderSize::LARGE: size_multiplier = 2.5f; break;
        case BoulderSize::MASSIVE: size_multiplier = 5.0f; break;
    }
    
    return static_cast<int>(base_yield * size_multiplier);
}

} // namespace Boulders
} // namespace MultiTileObjects
} // namespace Vegetation
} // namespace Systems
} // namespace World