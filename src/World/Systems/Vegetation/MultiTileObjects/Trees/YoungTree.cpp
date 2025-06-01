// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/Trees/YoungTree.cpp
#include "YoungTree.h"
#include "../../../../../Core/BaseConfig.h"
#include <cmath>
#include <random>

namespace World {
namespace Systems {
namespace Vegetation {
namespace MultiTileObjects {
namespace Trees {

YoungTree::YoungTree(int origin_x, int origin_y, unsigned int seed)
    : BaseVegetationObject(origin_x, origin_y, seed), wind_sway_phase(0.0f), growth_animation(0.0f) {
    
    // Set up as a smaller 6x6 tree (increased from 4x4 for better appearance)
    setDimensions(6, 6);
    has_animation = true;
    
    // Determine what type of young tree this is
    determineTreeType();
    
    generatePattern();
}

void YoungTree::determineTreeType() {
    std::mt19937 rng(random_seed);
    std::uniform_int_distribution<int> type_dist(0, 3);
    
    int tree_type_int = type_dist(rng);
    params.type = static_cast<TreeType>(tree_type_int);
    
    // Set tree-specific properties for TOP-DOWN VIEW
    switch (params.type) {
        case TreeType::BIRCH_SAPLING:
            params.trunk_color = Colors::SILVER_BIRCH_BARK;
            params.leaf_color = Colors::SILVER_BIRCH_LEAVES;
            params.trunk_char = 'i';        // Thin trunk from above
            params.canopy_chars = "*o.,";   // Light, scattered canopy
            params.wind_sensitivity = 0.8f;
            break;
            
        case TreeType::OAK_SAPLING:
            params.trunk_color = Colors::ANCIENT_OAK_TRUNK;
            params.leaf_color = Colors::ANCIENT_OAK_CANOPY;
            params.trunk_char = 'O';        // Rounder, thicker trunk
            params.canopy_chars = "%*o.";   // Dense foliage patches
            params.wind_sensitivity = 0.6f;
            break;
            
        case TreeType::PINE_SAPLING:
            params.trunk_color = Colors::NOBLE_PINE_TRUNK;
            params.leaf_color = Colors::NOBLE_PINE_NEEDLES;
            params.trunk_char = '|';        // Straight pine trunk
            params.canopy_chars = "^*|.";   // Pointed, needle-like
            params.wind_sensitivity = 0.4f;
            break;
            
        case TreeType::WILLOW_SAPLING:
        default:
            params.trunk_color = Colors::WEEPING_WILLOW_TRUNK;
            params.leaf_color = Colors::WEEPING_WILLOW_FRONDS;
            params.trunk_char = 'I';        // Upright willow trunk
            params.canopy_chars = ",~`.";   // Drooping, flowing patterns
            params.wind_sensitivity = 0.9f;
            break;
    }
    
    // Add some variation
    params.trunk_color = varyColor(params.trunk_color, 0.1f);
    params.leaf_color = varyColor(params.leaf_color, 0.08f);
}

void YoungTree::generatePattern() {
    // Start completely clear
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            setTile(x, y, ' ', sf::Color::Black, sf::Color::Transparent, false, false);
        }
    }
    
    generateBushyCanopy();
    generateTrunk();
}

void YoungTree::generateBushyCanopy() {
    int center_x = width / 2;
    int center_y = height / 2;
    
    // Create dense, bushy canopy for young tree
    // Even small trees should look full and healthy
    
    // Main canopy layer - dense core
    addCanopyCircle(center_x, center_y - 1, 2, 0.9f);
    
    // Secondary layer for natural shape
    addCanopyCircle(center_x, center_y - 2, 1, 0.8f);
    
    // Add some asymmetry
    std::mt19937 rng(random_seed + 500);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    if (dist(rng) > 0.5f) {
        addCanopyCircle(center_x - 1, center_y, 1, 0.7f);
    }
    if (dist(rng) > 0.5f) {
        addCanopyCircle(center_x + 1, center_y, 1, 0.7f);
    }
}

void YoungTree::addCanopyCircle(int center_x, int center_y, int radius, float density) {
    for (int y = center_y - radius; y <= center_y + radius; ++y) {
        for (int x = center_x - radius; x <= center_x + radius; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                float distance = std::sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
                
                if (distance <= radius + 0.5f) { // Slightly larger for better coverage
                    float distance_factor = 1.0f - (distance / (radius + 0.5f));
                    float noise = getProceduralNoise(x, y, 0.3f);
                    
                    // High chance of foliage for bushy appearance
                    if (noise < density * distance_factor * 1.1f) {
                        char canopy_char = selectDenseCanopyChar(x, y, density);
                        sf::Color leaf_color = getAnimatedLeafColor(x, y, distance_factor);
                        sf::Color leaf_bg = getCanopyBackground(x, y, distance_factor);
                        
                        setTile(x, y, canopy_char, leaf_color, leaf_bg, false, true); // true = is_canopy
                    }
                }
            }
        }
    }
}

void YoungTree::generateTrunk() {
    // Simple but visible trunk in center bottom
    int center_x = width / 2;
    int trunk_y = height - 1; // Bottom of tree
    
    // Only show trunk if not covered by canopy
    ObjectTile current_tile = getTileAt(center_x, trunk_y);
    if (current_tile.character == ' ') {
        sf::Color trunk_bg = getTrunkBackground();
        
        setTile(center_x, trunk_y, params.trunk_char, 
               params.trunk_color, trunk_bg,
               true, false); // blocks movement
    }
}

char YoungTree::selectDenseCanopyChar(int x, int y, float density) const {
    if (params.canopy_chars.empty()) return '*';
    
    float noise = getProceduralNoise(x, y, 0.5f);
    
    // TOP-DOWN VIEW: Select character based on density for natural look from above
    int char_index;
    if (density > 0.8f) {
        // Dense areas - use first characters (densest from above)
        char_index = static_cast<int>(noise * 2) % std::min(2, static_cast<int>(params.canopy_chars.size()));
    } else if (density > 0.6f) {
        // Medium density - visible clusters from above
        char_index = static_cast<int>(noise * 3) % std::min(3, static_cast<int>(params.canopy_chars.size()));
    } else {
        // Lower density - scattered leaves from above
        char_index = static_cast<int>(noise * params.canopy_chars.size()) % params.canopy_chars.size();
    }
    
    return params.canopy_chars[char_index];
}

sf::Color YoungTree::getAnimatedLeafColor(int /* x */, int /* y */, float distance_factor) const {
    // Young trees have vibrant, healthy colors
    sf::Color base_color = params.leaf_color;
    
    // Growth animation affects brightness
    float growth_brightness = 1.0f + growth_animation * 0.15f;
    
    // Add depth - inner areas darker, edges lighter
    sf::Color deep_leaves = sf::Color(
        static_cast<sf::Uint8>(base_color.r * 0.8f),
        static_cast<sf::Uint8>(base_color.g * 0.9f),
        static_cast<sf::Uint8>(base_color.b * 0.7f)
    );
    sf::Color bright_leaves = sf::Color(
        static_cast<sf::Uint8>(std::min(255.0f, base_color.r * 1.2f * growth_brightness)),
        static_cast<sf::Uint8>(std::min(255.0f, base_color.g * 1.1f * growth_brightness)),
        static_cast<sf::Uint8>(std::min(255.0f, base_color.b * 1.0f * growth_brightness))
    );
    
    return Tile::interpolateColor(deep_leaves, bright_leaves, distance_factor);
}

sf::Color YoungTree::getCanopyBackground(int x, int y, float distance_factor) const {
    // Rich canopy background for young trees
    sf::Color deep_canopy = sf::Color(30, 40, 22);    // Deep interior
    sf::Color light_canopy = sf::Color(50, 65, 40);   // Lighter areas
    
    sf::Color base_bg = Tile::interpolateColor(deep_canopy, light_canopy, distance_factor);
    
    // Add variation
    float variation = getProceduralNoise(x, y, 0.4f) * 0.1f - 0.05f;
    return sf::Color(
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_bg.r * (1.0f + variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_bg.g * (1.0f + variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_bg.b * (1.0f + variation))))
    );
}

sf::Color YoungTree::getTrunkBackground() const {
    // Rich bark background for young trunk
    return sf::Color(50, 40, 30);
}

void YoungTree::updateAnimation(float time_delta) {
    current_time += time_delta;
    
    updateWindSway(time_delta);
    updateGrowthAnimation(time_delta);
    
    // Apply wind effects to canopy tiles
    float wind_strength = std::abs(wind_sway_phase) * params.wind_sensitivity;
    int wind_direction = (wind_sway_phase > 0) ? 1 : 0;
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            ObjectTile& tile = getTileRef(x, y);
            if (tile.is_canopy) {
                applyWindEffect(x, y, wind_strength, wind_direction);
            }
        }
    }
}

void YoungTree::updateWindSway(float /* time_delta */) {
    // Young trees sway more dramatically than ancient ones
    wind_sway_phase = std::sin(current_time * 1.5f) * params.wind_sensitivity;
}

void YoungTree::updateGrowthAnimation(float /* time_delta */) {
    // Gentle growth animation (pulsing)
    growth_animation = (std::sin(current_time * 0.3f) + 1.0f) * 0.5f;
}

bool YoungTree::canPlaceAt(int world_x, int world_y, 
                          const std::vector<float>& heightmap,
                          const std::vector<float>& slope_map,
                          int map_width, int map_height) const {
    // Check bounds
    if (world_x < 0 || world_y < 0 || 
        world_x + width >= map_width || world_y + height >= map_height) {
        return false;
    }
    
    // Check center tile terrain
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

bool YoungTree::isValidTerrain(float height, float slope) const {
    // FIXED: Young trees should NOT spawn on water or high mountains
    // They are more adaptable than ancient trees but still have limits
    return height >= 0.03f &&          // Above water level
           height <= 0.75f &&          // Below high mountain areas
           slope <= 0.04f;             // Gentle to moderate slopes only
}

} // namespace Trees
} // namespace MultiTileObjects
} // namespace Vegetation
} // namespace Systems
} // namespace World