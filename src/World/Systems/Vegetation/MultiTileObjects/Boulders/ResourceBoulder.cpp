// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/Boulders/ResourceBoulder.cpp
#include "ResourceBoulder.h"
#include "../../../../../Core/BaseConfig.h"
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
    
    generatePattern();
}

void ResourceBoulder::determineBoulderSize() {
    std::mt19937 rng(random_seed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    params.size = boulder_size;
    params.resource_type = resource_type;
    
    // Rich, natural stone colors that look great from above
    params.base_stone_color = varyColor(Colors::MOSSY_BOULDER_BASE, 0.15f);
    params.moss_color = varyColor(Colors::MOSSY_BOULDER_MOSS, 0.12f);
    
    // Resource-specific colors
    switch (resource_type) {
        case ResourceType::GOLD_VEINS:
            params.resource_color = Colors::GOLD_VEIN_GLEAM;
            params.resource_density = 0.15f + dist(rng) * 0.1f;  // More visible
            params.vein_count = 3 + static_cast<int>(dist(rng) * 4);
            break;
        case ResourceType::SILVER_VEINS:
            params.resource_color = Colors::SILVER_LODE_GLEAM;
            params.resource_density = 0.18f + dist(rng) * 0.1f;
            params.vein_count = 3 + static_cast<int>(dist(rng) * 5);
            break;
        case ResourceType::IRON_DEPOSITS:
            params.resource_color = Colors::IRON_ORE_METAL;
            params.resource_density = 0.2f + dist(rng) * 0.1f;
            params.vein_count = 4 + static_cast<int>(dist(rng) * 5);
            break;
        case ResourceType::COPPER_DEPOSITS:
            params.resource_color = Colors::COPPER_DEPOSIT_GLEAM;
            params.resource_density = 0.17f + dist(rng) * 0.1f;
            params.vein_count = 3 + static_cast<int>(dist(rng) * 4);
            break;
        default:
            params.resource_density = 0.0f;
            params.vein_count = 0;
            break;
    }
    
    // Boulder characteristics
    params.moss_coverage = 0.3f + dist(rng) * 0.4f;   // More moss for natural look
    params.weathering_factor = 0.2f + dist(rng) * 0.3f;
}

void ResourceBoulder::generatePattern() {
    // Start completely clear - NO terrain background bleeding!
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            setTile(x, y, ' ', sf::Color::Black, sf::Color::Transparent, false, false);
        }
    }
    
    // Generate beautiful boulder from above
    generateNaturalBoulderShape();
    generateResourceVeins();
    generateMossPatches();
    addWeatheringDetails();
}

void ResourceBoulder::generateNaturalBoulderShape() {
    int center_x = width / 2;
    int center_y = height / 2;
    float radius = std::min(width, height) / 2.0f - 0.5f;
    
    // Create natural, irregular boulder shape that looks great from above
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float dx = x - center_x;
            float dy = y - center_y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            // Add organic irregularity for natural boulder shape
            float angle = std::atan2(dy, dx);
            float irregularity = std::sin(angle * 3.0f) * 0.3f + std::sin(angle * 5.0f) * 0.2f;
            float effective_radius = radius * (0.85f + irregularity + getProceduralNoise(x, y, 0.2f) * 0.3f);
            
            if (distance <= effective_radius) {
                // Determine boulder surface type based on distance from center
                float distance_factor = distance / effective_radius;
                
                char boulder_char = selectBoulderCharacter(x, y, distance_factor);
                sf::Color stone_fg = getBoulderForegroundColor(x, y, distance_factor);
                sf::Color stone_bg = getBoulderBackgroundColor(x, y, distance_factor);
                
                setTile(x, y, boulder_char, stone_fg, stone_bg, true, false); // blocks movement
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
    
    int center_x = width / 2;
    int center_y = height / 2;
    
    for (int vein = 0; vein < params.vein_count; ++vein) {
        // Create more natural vein patterns
        float angle = (vein / static_cast<float>(params.vein_count)) * 2.0f * 3.14159f + dist(rng) * 1.0f;
        int vein_length = 2 + static_cast<int>(dist(rng) * (std::min(width, height) / 3));
        
        generateNaturalVein(center_x, center_y, angle, vein_length, 1.0f + dist(rng) * 0.5f);
    }
}

void ResourceBoulder::generateNaturalVein(int start_x, int start_y, float angle, int length, float thickness) {
    std::mt19937 rng(random_seed + start_x * 1000 + start_y * 100);
    std::uniform_real_distribution<float> dist(-0.2f, 0.2f);
    
    for (int step = 0; step < length; ++step) {
        float progress = static_cast<float>(step) / length;
        
        // Follow the angle with some natural wandering
        float current_angle = angle + dist(rng) * progress;
        int vein_x = start_x + static_cast<int>(std::cos(current_angle) * step);
        int vein_y = start_y + static_cast<int>(std::sin(current_angle) * step);
        
        if (vein_x >= 0 && vein_x < width && vein_y >= 0 && vein_y < height) {
            addResourceDeposit(vein_x, vein_y, thickness * (1.0f - progress * 0.3f));
        }
    }
}

void ResourceBoulder::addResourceDeposit(int x, int y, float thickness) {
    // Only add resource to existing boulder tiles
    ObjectTile& tile = getTileRef(x, y);
    if (tile.character == ' ') return; // No boulder here
    
    float noise = getProceduralNoise(x, y, 0.6f);
    if (noise < params.resource_density * thickness) {
        char resource_char = selectResourceCharacter(resource_type);
        sf::Color resource_color = getResourceColor(resource_type);
        
        // Keep the boulder background but show resource character
        setTile(x, y, resource_char, resource_color, tile.background, true, false);
    }
}

void ResourceBoulder::generateMossPatches() {
    std::mt19937 rng(random_seed + 2000);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    // Create natural moss patches for organic appearance
    int moss_patch_count = static_cast<int>(params.moss_coverage * width * height / 8);
    
    for (int patch = 0; patch < moss_patch_count; ++patch) {
        int moss_x = static_cast<int>(dist(rng) * width);
        int moss_y = static_cast<int>(dist(rng) * height);
        int moss_radius = 1 + static_cast<int>(dist(rng) * 3);
        
        addMossPatch(moss_x, moss_y, moss_radius);
    }
}

void ResourceBoulder::addMossPatch(int center_x, int center_y, int radius) {
    for (int y = center_y - radius; y <= center_y + radius; ++y) {
        for (int x = center_x - radius; x <= center_x + radius; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                if (shouldHaveMoss(x, y, center_x, center_y, radius)) {
                    ObjectTile& tile = getTileRef(x, y);
                    if (tile.character != ' ') { // Only moss existing boulder
                        char moss_char = selectMossCharacter(x, y);
                        sf::Color moss_fg = getMossColor(x, y);
                        
                        // Apply moss while keeping boulder background
                        setTile(x, y, moss_char, moss_fg, tile.background, tile.blocks_movement, false);
                    }
                }
            }
        }
    }
}

bool ResourceBoulder::shouldHaveMoss(int x, int y, int center_x, int center_y, int radius) const {
    float distance = std::sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
    float noise = getProceduralNoise(x, y, 0.5f);
    return distance <= radius && noise > 0.2f; // More lenient for natural moss
}

void ResourceBoulder::addWeatheringDetails() {
    // Add final weathering touches for realism
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            ObjectTile& tile = getTileRef(x, y);
            if (tile.character != ' ') {
                float weathering_noise = getProceduralNoise(x, y, 0.8f);
                if (weathering_noise > 0.85f) {
                    // Add weathering details
                    if (tile.character == 'O') {
                        tile.character = (weathering_noise > 0.92f) ? '8' : 'o';
                    }
                    // Slightly darken weathered areas
                    tile.background = sf::Color(
                        static_cast<sf::Uint8>(tile.background.r * 0.9f),
                        static_cast<sf::Uint8>(tile.background.g * 0.9f),
                        static_cast<sf::Uint8>(tile.background.b * 0.9f)
                    );
                }
            }
        }
    }
}

char ResourceBoulder::selectBoulderCharacter(int x, int y, float distance_factor) const {
    float noise = getProceduralNoise(x, y, 0.4f);
    
    // Character selection based on position and randomness for natural look
    if (distance_factor < 0.3f) {
        // Center of boulder - solid
        return (noise < 0.7f) ? 'O' : '@';
    } else if (distance_factor < 0.7f) {
        // Mid section
        if (noise < 0.5f) return 'O';
        else if (noise < 0.8f) return 'o';
        else return '8';
    } else {
        // Edges - more varied
        if (noise < 0.4f) return 'o';
        else if (noise < 0.7f) return '.';
        else return '8';
    }
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

char ResourceBoulder::selectMossCharacter(int x, int y) const {
    float noise = getProceduralNoise(x, y, 0.6f);
    
    // Variety in moss characters for natural appearance
    if (noise < 0.4f) return '.';
    else if (noise < 0.7f) return ',';
    else return ';';
}

sf::Color ResourceBoulder::getBoulderForegroundColor(int x, int y, float distance_factor) const {
    // Create rich, varied stone colors
    sf::Color base_stone = params.base_stone_color;
    
    // Add depth and variation
    float depth_factor = 1.0f - distance_factor * 0.3f; // Center darker, edges lighter
    float variation = getProceduralNoise(x, y, 0.3f) * 0.15f - 0.075f;
    
    return sf::Color(
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_stone.r * depth_factor * (1.0f + variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_stone.g * depth_factor * (1.0f + variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_stone.b * depth_factor * (1.0f + variation))))
    );
}

sf::Color ResourceBoulder::getBoulderBackgroundColor(int x, int y, float distance_factor) const {
    // Rich boulder background colors - NO terrain bleeding!
    sf::Color deep_stone = sf::Color(60, 55, 50);    // Deep stone interior
    sf::Color light_stone = sf::Color(95, 88, 80);   // Lighter stone surface
    
    // Blend based on position for natural depth
    sf::Color base_bg = Tile::interpolateColor(deep_stone, light_stone, distance_factor);
    
    // Add subtle variation
    float variation = getProceduralNoise(x, y, 0.4f) * 0.1f - 0.05f;
    return sf::Color(
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_bg.r * (1.0f + variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_bg.g * (1.0f + variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_bg.b * (1.0f + variation))))
    );
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

sf::Color ResourceBoulder::getMossColor(int x, int y) const {
    return varyColor(params.moss_color, 0.08f);
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
        case BoulderSize::SMALL: return 6;    // Increased from 5
        case BoulderSize::MEDIUM: return 10;  // Increased from 8
        case BoulderSize::LARGE: return 18;   // Increased from 15
        case BoulderSize::MASSIVE: return 30; // Increased from 25
        default: return 10;
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