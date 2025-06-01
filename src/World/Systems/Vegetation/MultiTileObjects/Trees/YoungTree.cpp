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
    
    // Set up as a smaller 4x4 tree
    setDimensions(4, 4);
    has_animation = true;
    
    // Determine what type of young tree this is
    determineTreeType();
    
    // FIXED: Generate the tree pattern immediately after construction
    generatePattern();
}

void YoungTree::determineTreeType() {
    std::mt19937 rng(random_seed);
    std::uniform_int_distribution<int> type_dist(0, 3);
    
    int tree_type_int = type_dist(rng);
    params.type = static_cast<TreeType>(tree_type_int);
    
    // Set tree-specific properties
    switch (params.type) {
        case TreeType::BIRCH_SAPLING:
            params.trunk_color = Colors::SILVER_BIRCH_BARK;
            params.leaf_color = Colors::SILVER_BIRCH_LEAVES;
            params.trunk_char = '!';
            params.canopy_chars = "*.";
            params.wind_sensitivity = 0.8f;
            break;
            
        case TreeType::OAK_SAPLING:
            params.trunk_color = Colors::ANCIENT_OAK_TRUNK;
            params.leaf_color = Colors::ANCIENT_OAK_CANOPY;
            params.trunk_char = 'Y';
            params.canopy_chars = "%*";
            params.wind_sensitivity = 0.6f;
            break;
            
        case TreeType::PINE_SAPLING:
            params.trunk_color = Colors::NOBLE_PINE_TRUNK;
            params.leaf_color = Colors::NOBLE_PINE_NEEDLES;
            params.trunk_char = 'A';
            params.canopy_chars = "^*";
            params.wind_sensitivity = 0.4f;
            break;
            
        case TreeType::WILLOW_SAPLING:
        default:
            params.trunk_color = Colors::WEEPING_WILLOW_TRUNK;
            params.leaf_color = Colors::WEEPING_WILLOW_FRONDS;
            params.trunk_char = 'W';
            params.canopy_chars = "~.";
            params.wind_sensitivity = 0.9f;
            break;
    }
    
    // Add some variation
    params.trunk_color = varyColor(params.trunk_color, 0.1f);
    params.leaf_color = varyColor(params.leaf_color, 0.08f);
}

void YoungTree::generatePattern() {
    // Clear pattern with proper terrain background
    sf::Color terrain_bg = getTerrainBackground();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            setTile(x, y, ' ', sf::Color::Black, terrain_bg, false, false);
        }
    }
    
    generateTrunk();
    generateCanopy();
}

void YoungTree::generateTrunk() {
    // Simple trunk in center
    int center_x = width / 2;
    int trunk_y = height - 1; // Bottom of tree
    
    // FIXED: Use terrain background blended with trunk color instead of solid trunk
    sf::Color trunk_bg = Tile::interpolateColor(getTerrainBackground(), params.trunk_color, 0.6f);
    
    setTile(center_x, trunk_y, params.trunk_char, 
           params.trunk_color, trunk_bg,
           true, false); // blocks movement
}

void YoungTree::generateCanopy() {
    int center_x = width / 2;
    int center_y = height / 2;
    
    // Simple 3x3 canopy around center
    for (int y = center_y - 1; y <= center_y + 1; ++y) {
        for (int x = center_x - 1; x <= center_x + 1; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height && y < height - 1) {
                char canopy_char = selectCanopyChar(x, y);
                sf::Color leaf_color = getAnimatedLeafColor(x, y);
                // FIXED: Use terrain background instead of transparent
                sf::Color bg_color = getTerrainBackground();
                
                setTile(x, y, canopy_char, leaf_color, bg_color, false, true); // true = is_canopy
            }
        }
    }
}

char YoungTree::selectCanopyChar(int x, int y) const {
    if (params.canopy_chars.empty()) return '*';
    
    float noise = getProceduralNoise(x, y, 0.5f);
    size_t char_index = static_cast<size_t>(noise * params.canopy_chars.size());
    char_index = std::min(char_index, params.canopy_chars.size() - 1);
    
    return params.canopy_chars[char_index];
}

sf::Color YoungTree::getAnimatedLeafColor(int /* x */, int /* y */) const {
    // Young trees have more vibrant, changing colors
    sf::Color base_color = params.leaf_color;
    
    // Growth animation affects brightness
    float growth_brightness = 1.0f + growth_animation * 0.2f;
    
    return sf::Color(
        static_cast<sf::Uint8>(std::min(255.0f, base_color.r * growth_brightness)),
        static_cast<sf::Uint8>(std::min(255.0f, base_color.g * growth_brightness)),
        static_cast<sf::Uint8>(std::min(255.0f, base_color.b * growth_brightness))
    );
}

// FIXED: Add method to get terrain background for young trees
sf::Color YoungTree::getTerrainBackground() const {
    // Young trees grow in various terrain, use general grass background
    return Core::LandColors::PLAINS_GRASS_BASE;
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
    // Young trees are more adaptable than ancient trees
    return height >= 0.02f && height <= 0.8f && slope <= 0.05f;
}

} // namespace Trees
} // namespace MultiTileObjects
} // namespace Vegetation
} // namespace Systems
} // namespace World