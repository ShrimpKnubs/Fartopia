// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/Trees/AncientOakTree.cpp
#include "AncientOakTree.h"
#include "../../../../Core/BaseConfig.h"
#include <cmath>
#include <random>

namespace World {
namespace Systems {
namespace Vegetation {
namespace MultiTileObjects {
namespace Trees {

AncientOakTree::AncientOakTree(int origin_x, int origin_y, unsigned int seed)
    : BaseVegetationObject(origin_x, origin_y, seed), wind_sway_phase(0.0f), seasonal_factor(0.0f) {
    
    // Set up as a large 15x15 tree
    setDimensions(15, 15);
    has_animation = true;
    
    // Initialize tree parameters with procedural variation
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    params.trunk_radius = 2 + static_cast<int>(dist(rng) * 2);      // 2-3 tile radius
    params.canopy_radius = 6 + static_cast<int>(dist(rng) * 2);     // 6-7 tile radius
    params.canopy_density = 0.7f + dist(rng) * 0.2f;               // 70-90% density
    params.trunk_irregularity = 0.1f + dist(rng) * 0.2f;           // Some trunk variation
    
    // Vary the colors procedurally
    params.trunk_color = varyColor(Colors::ANCIENT_OAK_TRUNK, 0.1f);
    params.leaf_color_summer = varyColor(Colors::ANCIENT_OAK_CANOPY, 0.08f);
    params.leaf_color_autumn = sf::Color(
        static_cast<sf::Uint8>(std::min(255.0f, params.leaf_color_summer.r * 1.2f)),
        static_cast<sf::Uint8>(params.leaf_color_summer.g * 0.8f), 
        static_cast<sf::Uint8>(params.leaf_color_summer.b * 0.4f)
    );
    
    // FIXED: Generate the tree pattern immediately after construction
    generatePattern();
}

void AncientOakTree::generatePattern() {
    // Clear any existing pattern with proper terrain background
    sf::Color terrain_bg = getTerrainBackground();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            setTile(x, y, ' ', sf::Color::Black, terrain_bg, false, false);
        }
    }
    
    // Generate from bottom up: roots -> trunk -> canopy
    generateRoots();
    generateTrunk();
    generateCanopy();
}

void AncientOakTree::generateTrunk() {
    int center_x = width / 2;
    
    // Generate trunk from bottom to about 1/3 height
    int trunk_height = height / 3;
    
    for (int y = height - 1; y >= height - trunk_height; --y) {
        // Trunk gets slightly thinner as it goes up
        float height_factor = static_cast<float>(height - 1 - y) / trunk_height;
        int current_radius = static_cast<int>(params.trunk_radius * (1.0f - height_factor * 0.3f));
        
        addTrunkSection(center_x, y, current_radius);
    }
}

void AncientOakTree::generateCanopy() {
    int center_x = width / 2;
    int center_y = height / 2;
    
    // Generate canopy in layers for natural look
    // Bottom layer (densest)
    addCanopyLayer(center_x, center_y - 2, params.canopy_radius, params.canopy_density);
    
    // Middle layer
    addCanopyLayer(center_x, center_y - 4, params.canopy_radius - 1, params.canopy_density * 0.8f);
    
    // Top layer (thinner)
    addCanopyLayer(center_x, center_y - 6, params.canopy_radius - 2, params.canopy_density * 0.6f);
    
    // Add some asymmetry and branch extensions
    std::mt19937 rng(random_seed + 1000);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (int i = 0; i < 3; ++i) {
        int branch_x = center_x + static_cast<int>((dist(rng) - 0.5f) * params.canopy_radius);
        int branch_y = center_y - static_cast<int>(dist(rng) * 4);
        addCanopyLayer(branch_x, branch_y, 2, params.canopy_density * 0.4f);
    }
}

void AncientOakTree::generateRoots() {
    int center_x = width / 2;
    int center_y = height / 2;
    
    // Add some root extensions at the base
    std::mt19937 rng(random_seed + 2000);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (int i = 0; i < 6; ++i) {
        float angle = (i / 6.0f) * 2.0f * 3.14159f;
        int root_length = 2 + static_cast<int>(dist(rng) * 2);
        
        for (int j = 1; j <= root_length; ++j) {
            int root_x = center_x + static_cast<int>(std::cos(angle) * j);
            int root_y = center_y + static_cast<int>(std::sin(angle) * j * 0.3f); // Roots spread outward and slightly down
            
            if (root_x >= 0 && root_x < width && root_y >= 0 && root_y < height) {
                char root_char = (j == 1) ? getTrunkColor(root_x, root_y).r > 100 ? '\\' : '/' : '.';
                sf::Color root_bg = Tile::interpolateColor(getTerrainBackground(), Colors::FOREST_UNDERGROWTH, 0.3f);
                setTile(root_x, root_y, root_char, 
                       varyColor(params.trunk_color, 0.15f),
                       root_bg, 
                       false, false);
            }
        }
    }
}

void AncientOakTree::addCanopyLayer(int center_x, int center_y, int radius, float density) {
    for (int y = center_y - radius; y <= center_y + radius; ++y) {
        for (int x = center_x - radius; x <= center_x + radius; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                if (isInCanopyRadius(x, y, center_x, center_y, radius)) {
                    // Use density and noise to determine if this tile has leaves
                    float distance_factor = 1.0f - (std::sqrt((x - center_x) * (x - center_x) + 
                                                            (y - center_y) * (y - center_y)) / radius);
                    float noise = getProceduralNoise(x, y, 0.3f);
                    
                    if (noise < density * distance_factor) {
                        char canopy_char = selectCanopyCharacter(x, y, density);
                        sf::Color leaf_fg = getLeafColor(x, y);
                        // FIXED: Use forest undergrowth background blended with terrain
                        sf::Color leaf_bg = Tile::interpolateColor(getTerrainBackground(), Colors::FOREST_UNDERGROWTH, 0.6f);
                        
                        setTile(x, y, canopy_char, leaf_fg, leaf_bg, false, true); // true = is_canopy
                    }
                }
            }
        }
    }
}

void AncientOakTree::addTrunkSection(int center_x, int center_y, int radius) {
    for (int y = center_y - radius; y <= center_y + radius; ++y) {
        for (int x = center_x - radius; x <= center_x + radius; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                float distance = std::sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
                
                if (distance <= radius) {
                    char trunk_char = selectTrunkCharacter(x, y);
                    sf::Color trunk_fg = getTrunkColor(x, y);
                    // FIXED: Use terrain background blended with trunk color
                    sf::Color trunk_bg = Tile::interpolateColor(getTerrainBackground(), params.trunk_color, 0.7f);
                    
                    setTile(x, y, trunk_char, trunk_fg, trunk_bg, true, false); // blocks movement
                }
            }
        }
    }
}

bool AncientOakTree::isInCanopyRadius(int x, int y, int center_x, int center_y, int radius) const {
    float distance = std::sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
    return distance <= radius;
}

char AncientOakTree::selectTrunkCharacter(int x, int y) const {
    std::string trunk_chars = "||\\/#";
    float noise = getProceduralNoise(x, y, 0.5f);
    
    if (noise < 0.5f) return '|';       // Vertical bark lines
    else if (noise < 0.7f) return '\\'; // Diagonal bark texture  
    else if (noise < 0.85f) return '/'; // Other diagonal
    else return '#';                    // Rough bark sections
}

char AncientOakTree::selectCanopyCharacter(int x, int y, float density) const {
    float noise = getProceduralNoise(x, y, 0.4f);
    
    if (density > 0.8f) {
        // Dense canopy
        if (noise < 0.4f) return '#';      // Dense leaves
        else if (noise < 0.7f) return '%'; // Medium density
        else return '*';                   // Light leaf clusters
    } else {
        // Lighter canopy
        if (noise < 0.3f) return '%';      // Medium leaves
        else if (noise < 0.6f) return '*'; // Light leaves
        else return '.';                   // Very light/sparse
    }
}

sf::Color AncientOakTree::getLeafColor(int /* x */, int /* y */) const {
    // Interpolate between summer and autumn colors based on seasonal factor
    sf::Color base_color = interpolateSeasonalColor(params.leaf_color_summer, 
                                                   params.leaf_color_autumn, seasonal_factor);
    
    // Add subtle variation based on position
    return varyColor(base_color, 0.05f);
}

sf::Color AncientOakTree::getTrunkColor(int /* x */, int /* y */) const {
    return varyColor(params.trunk_color, 0.08f);
}

// FIXED: Add method to get terrain background for this tree
sf::Color AncientOakTree::getTerrainBackground() const {
    // Ancient oaks grow in rich soil areas
    return Core::LandColors::GRASS_MID_SLOPE;
}

void AncientOakTree::updateAnimation(float time_delta) {
    current_time += time_delta;
    
    // Wind sway effect (gentle for large trees)
    wind_sway_phase = std::sin(current_time * 0.5f) * 0.3f; // Slow, gentle sway
    
    // Seasonal color changes (very slow)
    seasonal_factor = (std::sin(current_time * 0.01f) + 1.0f) * 0.5f; // Cycles over ~10 minutes
    
    // Apply wind effects to canopy tiles
    float wind_strength = std::abs(wind_sway_phase);
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

bool AncientOakTree::canPlaceAt(int world_x, int world_y, 
                               const std::vector<float>& heightmap,
                               const std::vector<float>& slope_map,
                               int map_width, int map_height) const {
    // Check if the tree fits in the map bounds
    if (world_x < 0 || world_y < 0 || 
        world_x + width >= map_width || world_y + height >= map_height) {
        return false;
    }
    
    // Check terrain suitability in a 3x3 area around the center
    int center_x = world_x + width / 2;
    int center_y = world_y + height / 2;
    
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int check_x = center_x + dx;
            int check_y = center_y + dy;
            
            if (check_x >= 0 && check_x < map_width && check_y >= 0 && check_y < map_height) {
                size_t index = static_cast<size_t>(check_y) * map_width + check_x;
                
                if (index < heightmap.size() && index < slope_map.size()) {
                    if (!isValidTerrain(heightmap[index], slope_map[index])) {
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

bool AncientOakTree::isValidTerrain(float height, float slope) const {
    // Ancient oaks prefer mid-elevation, gentle slopes
    return height >= 0.05f && height <= 0.7f && slope <= 0.03f;
}

} // namespace Trees
} // namespace MultiTileObjects
} // namespace Vegetation
} // namespace Systems
} // namespace World