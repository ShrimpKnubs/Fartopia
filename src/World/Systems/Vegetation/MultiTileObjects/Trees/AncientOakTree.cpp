// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/Trees/AncientOakTree.cpp
#include "AncientOakTree.h"
#include "../../../../../Core/BaseConfig.h"
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
    params.canopy_density = 0.85f + dist(rng) * 0.1f;              // MUCH DENSER: 85-95% density
    params.trunk_irregularity = 0.1f + dist(rng) * 0.2f;           // Some trunk variation
    
    // Rich, lush colors for beautiful canopy
    params.trunk_color = varyColor(Colors::ANCIENT_OAK_TRUNK, 0.1f);
    params.leaf_color_summer = varyColor(Colors::ANCIENT_OAK_CANOPY, 0.08f);
    params.leaf_color_autumn = sf::Color(
        static_cast<sf::Uint8>(std::min(255.0f, params.leaf_color_summer.r * 1.2f)),
        static_cast<sf::Uint8>(params.leaf_color_summer.g * 0.8f), 
        static_cast<sf::Uint8>(params.leaf_color_summer.b * 0.4f)
    );
    
    generatePattern();
}

void AncientOakTree::generatePattern() {
    // Start with completely clear pattern - no terrain bleeding
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            setTile(x, y, ' ', sf::Color::Black, sf::Color::Transparent, false, false);
        }
    }
    
    // Generate a magnificent bushy canopy from top-down view
    generateDenseBushyCanopy();
    generateVisibleTrunk();
    generateRootFlare();
}

void AncientOakTree::generateDenseBushyCanopy() {
    int center_x = width / 2;
    int center_y = height / 2;
    
    // Create multiple overlapping layers for maximum density and bushiness
    
    // Layer 1: Core dense canopy (largest)
    addDenseCanopyLayer(center_x, center_y - 1, params.canopy_radius, 0.95f);
    
    // Layer 2: Secondary canopy for natural variation
    addDenseCanopyLayer(center_x - 1, center_y - 2, params.canopy_radius - 1, 0.88f);
    addDenseCanopyLayer(center_x + 1, center_y - 2, params.canopy_radius - 1, 0.88f);
    
    // Layer 3: Tertiary layers for organic shape
    addDenseCanopyLayer(center_x, center_y - 3, params.canopy_radius - 2, 0.80f);
    addDenseCanopyLayer(center_x - 2, center_y, params.canopy_radius - 2, 0.75f);
    addDenseCanopyLayer(center_x + 2, center_y, params.canopy_radius - 2, 0.75f);
    
    // Layer 4: Branch extensions for natural asymmetry
    std::mt19937 rng(random_seed + 1000);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (int i = 0; i < 6; ++i) {
        float angle = (i / 6.0f) * 2.0f * 3.14159f + dist(rng) * 0.5f;
        int branch_x = center_x + static_cast<int>(std::cos(angle) * (params.canopy_radius - 1));
        int branch_y = center_y + static_cast<int>(std::sin(angle) * (params.canopy_radius - 1));
        addDenseCanopyLayer(branch_x, branch_y, 2, 0.70f);
    }
}

void AncientOakTree::addDenseCanopyLayer(int center_x, int center_y, int radius, float density) {
    for (int y = center_y - radius; y <= center_y + radius; ++y) {
        for (int x = center_x - radius; x <= center_x + radius; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                float distance = std::sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
                
                if (distance <= radius) {
                    // Much more aggressive density for bushy appearance
                    float distance_factor = 1.0f - (distance / radius);
                    distance_factor = std::pow(distance_factor, 0.5f); // Softer falloff for denser coverage
                    
                    float noise = getProceduralNoise(x, y, 0.2f); // Less noise variation
                    
                    // MUCH higher chance of dense foliage
                    if (noise < density * distance_factor * 1.2f) { // Boosted density
                        char canopy_char = selectBushyCanopyCharacter(x, y, density, distance_factor);
                        sf::Color leaf_fg = getLushLeafColor(x, y, distance_factor);
                        // Use rich, solid canopy background - no terrain bleeding!
                        sf::Color leaf_bg = getBushyCanopyBackground(x, y, distance_factor);
                        
                        setTile(x, y, canopy_char, leaf_fg, leaf_bg, false, true); // true = is_canopy
                    }
                }
            }
        }
    }
}

void AncientOakTree::generateVisibleTrunk() {
    int center_x = width / 2;
    
    // Only show trunk where it's not covered by canopy
    // Generate trunk from bottom to about 1/3 height
    int trunk_height = height / 3;
    
    for (int y = height - 1; y >= height - trunk_height; --y) {
        // Trunk gets slightly thinner as it goes up
        float height_factor = static_cast<float>(height - 1 - y) / trunk_height;
        int current_radius = static_cast<int>(params.trunk_radius * (1.0f - height_factor * 0.3f));
        
        addVisibleTrunkSection(center_x, y, current_radius);
    }
}

void AncientOakTree::addVisibleTrunkSection(int center_x, int center_y, int radius) {
    for (int y = center_y - radius; y <= center_y + radius; ++y) {
        for (int x = center_x - radius; x <= center_x + radius; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                float distance = std::sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
                
                if (distance <= radius) {
                    // Only place trunk if there's no canopy here
                    ObjectTile current_tile = getTileAt(x, y);
                    if (current_tile.character == ' ') {
                        char trunk_char = selectTrunkCharacter(x, y);
                        sf::Color trunk_fg = getTrunkColor(x, y);
                        sf::Color trunk_bg = getBarkBackground(x, y);
                        
                        setTile(x, y, trunk_char, trunk_fg, trunk_bg, true, false); // blocks movement
                    }
                }
            }
        }
    }
}

void AncientOakTree::generateRootFlare() {
    int center_x = width / 2;
    int center_y = height / 2;
    
    // Add some visible root flare at the base
    std::mt19937 rng(random_seed + 2000);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (int i = 0; i < 8; ++i) {
        float angle = (i / 8.0f) * 2.0f * 3.14159f;
        int root_length = 2 + static_cast<int>(dist(rng) * 2);
        
        for (int j = 1; j <= root_length; ++j) {
            int root_x = center_x + static_cast<int>(std::cos(angle) * j);
            int root_y = center_y + static_cast<int>(std::sin(angle) * j * 0.3f); 
            
            if (root_x >= 0 && root_x < width && root_y >= 0 && root_y < height) {
                // Only place roots where there's no canopy or trunk
                ObjectTile current_tile = getTileAt(root_x, root_y);
                if (current_tile.character == ' ') {
                    char root_char = (j == 1) ? '\\' : '.';
                    sf::Color root_fg = varyColor(params.trunk_color, 0.15f);
                    sf::Color root_bg = getGroundBackground();
                    
                    setTile(root_x, root_y, root_char, root_fg, root_bg, false, false);
                }
            }
        }
    }
}

char AncientOakTree::selectBushyCanopyCharacter(int x, int y, float density, float distance_factor) const {
    // Characters that create a dense, bushy appearance from above
    float noise = getProceduralNoise(x, y, 0.4f);
    float effective_density = density * distance_factor;
    
    if (effective_density > 0.9f) {
        // Very dense core - solid foliage
        if (noise < 0.6f) return '@';      // Very dense leaves
        else if (noise < 0.85f) return '#'; // Dense foliage
        else return '%';                   // Thick canopy
    } else if (effective_density > 0.7f) {
        // Dense areas
        if (noise < 0.5f) return '#';      // Dense foliage
        else if (noise < 0.8f) return '%'; // Medium-dense
        else return '*';                   // Leaf clusters
    } else {
        // Lighter canopy edges
        if (noise < 0.4f) return '%';      // Medium foliage
        else if (noise < 0.7f) return '*'; // Light clusters
        else return '.';                   // Sparse edges
    }
}

char AncientOakTree::selectTrunkCharacter(int x, int y) const {
    std::string trunk_chars = "||\\/#";
    float noise = getProceduralNoise(x, y, 0.5f);
    
    if (noise < 0.5f) return '|';       // Vertical bark lines
    else if (noise < 0.7f) return '\\'; // Diagonal bark texture  
    else if (noise < 0.85f) return '/'; // Other diagonal
    else return '#';                    // Rough bark sections
}

sf::Color AncientOakTree::getLushLeafColor(int x, int y, float distance_factor) const {
    // Create rich, lush leaf colors with depth
    sf::Color base_color = interpolateSeasonalColor(params.leaf_color_summer, 
                                                   params.leaf_color_autumn, seasonal_factor);
    
    // Add position-based variation for natural appearance
    float position_variation = getProceduralNoise(x, y, 0.3f) * 0.1f - 0.05f;
    
    // Add depth based on position - inner leaves darker, outer leaves lighter
    float depth_factor = distance_factor;
    sf::Color deep_forest = sf::Color(
        static_cast<sf::Uint8>(base_color.r * 0.7f),
        static_cast<sf::Uint8>(base_color.g * 0.8f),
        static_cast<sf::Uint8>(base_color.b * 0.6f)
    );
    sf::Color bright_leaves = sf::Color(
        static_cast<sf::Uint8>(std::min(255.0f, base_color.r * 1.2f)),
        static_cast<sf::Uint8>(std::min(255.0f, base_color.g * 1.1f)),
        static_cast<sf::Uint8>(base_color.b)
    );
    
    sf::Color final_color = Tile::interpolateColor(deep_forest, bright_leaves, depth_factor);
    
    // Apply position-based variation
    final_color = sf::Color(
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, final_color.r * (1.0f + position_variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, final_color.g * (1.0f + position_variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, final_color.b * (1.0f + position_variation))))
    );
    
    return final_color;
}

sf::Color AncientOakTree::getBushyCanopyBackground(int x, int y, float distance_factor) const {
    // Rich canopy background that creates depth without terrain bleeding
    sf::Color deep_canopy = sf::Color(25, 35, 18);    // Deep forest interior
    sf::Color light_canopy = sf::Color(45, 62, 35);   // Lighter canopy areas
    
    // Vary based on position for natural depth
    sf::Color base_bg = Tile::interpolateColor(deep_canopy, light_canopy, distance_factor);
    
    // Add some procedural variation
    float variation = getProceduralNoise(x, y, 0.3f) * 0.1f - 0.05f;
    return sf::Color(
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_bg.r * (1.0f + variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_bg.g * (1.0f + variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, base_bg.b * (1.0f + variation))))
    );
}

sf::Color AncientOakTree::getTrunkColor(int x, int y) const {
    // Add position-based variation for realistic bark
    float variation = getProceduralNoise(x, y, 0.4f) * 0.08f - 0.04f;
    sf::Color varied_trunk = sf::Color(
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, params.trunk_color.r * (1.0f + variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, params.trunk_color.g * (1.0f + variation)))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, params.trunk_color.b * (1.0f + variation))))
    );
    return varied_trunk;
}

sf::Color AncientOakTree::getBarkBackground(int x, int y) const {
    // Rich bark background
    sf::Color dark_bark = sf::Color(45, 35, 25);
    sf::Color light_bark = sf::Color(75, 65, 50);
    
    float noise = getProceduralNoise(x, y, 0.4f);
    return Tile::interpolateColor(dark_bark, light_bark, noise);
}

sf::Color AncientOakTree::getGroundBackground() const {
    // Rich forest floor
    return sf::Color(40, 50, 30);
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