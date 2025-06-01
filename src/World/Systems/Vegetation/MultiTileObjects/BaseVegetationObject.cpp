// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/BaseVegetationObject.cpp
#include "BaseVegetationObject.h"
#include "../VegetationColors.h"
#include "../../../../Core/BaseConfig.h"
#include <algorithm>
#include <cmath>
#include <random>

namespace World {
namespace Systems {
namespace Vegetation {
namespace MultiTileObjects {

BaseVegetationObject::BaseVegetationObject(int origin_x, int origin_y, unsigned int seed)
    : origin_x(origin_x), origin_y(origin_y), width(1), height(1), 
      random_seed(seed), has_animation(false), current_time(0.0f) {
}

void BaseVegetationObject::setDimensions(int w, int h) {
    width = w;
    height = h;
    tiles.resize(h);
    for (auto& row : tiles) {
        row.resize(w);
        // FIXED: Initialize tiles with proper default values
        for (auto& tile : row) {
            tile.character = ' ';
            tile.foreground = sf::Color::White;
            tile.background = getDefaultTerrainBackground(); // FIXED: Use terrain background instead of transparent
            tile.blocks_movement = false;
            tile.is_canopy = false;
            tile.animation_phase = 0.0f;
        }
    }
}

void BaseVegetationObject::setTile(int x, int y, char ch, sf::Color fg, sf::Color bg, 
                                  bool blocks, bool canopy) {
    ensureTileSpace(x, y);
    
    ObjectTile& tile = tiles[y][x];
    tile.relative_x = x;
    tile.relative_y = y;
    tile.character = ch;
    tile.foreground = fg;
    // FIXED: Don't allow transparent backgrounds - use terrain background instead
    tile.background = (bg == sf::Color::Transparent) ? getDefaultTerrainBackground() : bg;
    tile.blocks_movement = blocks;
    tile.is_canopy = canopy;
    tile.animation_phase = 0.0f;
}

BaseVegetationObject::ObjectTile& BaseVegetationObject::getTileRef(int x, int y) {
    ensureTileSpace(x, y);
    return tiles[y][x];
}

BaseVegetationObject::ObjectTile BaseVegetationObject::getTileAt(int relative_x, int relative_y) const {
    EntityContext defaultContext;
    return getTileAt(relative_x, relative_y, defaultContext);
}

BaseVegetationObject::ObjectTile BaseVegetationObject::getTileAt(int relative_x, int relative_y, 
                                                                 const EntityContext& context) const {
    if (relative_x < 0 || relative_x >= width || relative_y < 0 || relative_y >= height) {
        // FIXED: Return properly initialized empty tile
        ObjectTile empty_tile;
        empty_tile.character = ' ';
        empty_tile.foreground = sf::Color::White;
        empty_tile.background = getDefaultTerrainBackground(); // FIXED: Use terrain background
        empty_tile.blocks_movement = false;
        empty_tile.is_canopy = false;
        empty_tile.animation_phase = 0.0f;
        return empty_tile;
    }
    
    ObjectTile tile = tiles[relative_y][relative_x];
    
    // If there's an entity underneath a canopy tile, show the "under canopy" version
    if (context.entity_underneath && tile.is_canopy) {
        // Transform canopy view to underneath view (darker, trunk-like)
        tile.character = (tile.character == 'T' || tile.character == 'A') ? '|' : '.';
        tile.foreground = sf::Color(
            static_cast<sf::Uint8>(tile.foreground.r * 0.4f),
            static_cast<sf::Uint8>(tile.foreground.g * 0.4f), 
            static_cast<sf::Uint8>(tile.foreground.b * 0.4f)
        );
        tile.background = sf::Color(
            static_cast<sf::Uint8>(tile.background.r * 0.6f),
            static_cast<sf::Uint8>(tile.background.g * 0.6f),
            static_cast<sf::Uint8>(tile.background.b * 0.6f)
        );
    }
    
    return tile;
}

std::vector<BaseVegetationObject::ObjectTile> BaseVegetationObject::getAllTiles() const {
    EntityContext defaultContext;
    return getAllTiles(defaultContext);
}

std::vector<BaseVegetationObject::ObjectTile> BaseVegetationObject::getAllTiles(const EntityContext& context) const {
    std::vector<ObjectTile> all_tiles;
    all_tiles.reserve(width * height);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            EntityContext tile_context = context;
            // Check if entity is specifically at this tile
            if (context.entity_underneath) {
                int world_x = origin_x + x;
                int world_y = origin_y + y;
                tile_context.entity_underneath = (context.entity_x == world_x && context.entity_y == world_y);
            }
            all_tiles.push_back(getTileAt(x, y, tile_context));
        }
    }
    
    return all_tiles;
}

bool BaseVegetationObject::containsPoint(int world_x, int world_y) const {
    return (world_x >= origin_x && world_x < origin_x + width &&
            world_y >= origin_y && world_y < origin_y + height);
}

bool BaseVegetationObject::overlaps(const BaseVegetationObject& other) const {
    return !(origin_x + width <= other.origin_x || 
             other.origin_x + other.width <= origin_x ||
             origin_y + height <= other.origin_y ||
             other.origin_y + other.height <= origin_y);
}

sf::Color BaseVegetationObject::varyColor(const sf::Color& base, float variation) const {
    std::mt19937 rng(random_seed);
    std::uniform_real_distribution<float> dist(-variation, variation);
    
    auto clamp = [](float val) -> sf::Uint8 {
        return static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, val)));
    };
    
    return sf::Color(
        clamp(base.r + dist(rng) * 255.0f),
        clamp(base.g + dist(rng) * 255.0f),
        clamp(base.b + dist(rng) * 255.0f),
        base.a // Preserve alpha
    );
}

char BaseVegetationObject::selectCharFromSet(const std::string& charset, int x, int y) const {
    if (charset.empty()) return ' ';
    
    std::mt19937 rng(random_seed + x * 1000 + y);
    std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);
    return charset[dist(rng)];
}

float BaseVegetationObject::getProceduralNoise(int x, int y, float frequency) const {
    // Simple noise function for procedural variation
    unsigned int seed = random_seed + x * 73856093 + y * 19349663;
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    float noise = dist(rng);
    return std::sin(noise * frequency) * 0.5f + 0.5f; // Normalize to 0-1
}

void BaseVegetationObject::applyWindEffect(int x, int y, float wind_strength, int wind_direction) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    
    ObjectTile& tile = tiles[y][x];
    
    // Wind affects character choice and color
    if (wind_strength > 0.3f) {
        // Strong wind - bend characters
        if (tile.character == '|') tile.character = (wind_direction % 2 == 0) ? '/' : '\\';
        if (tile.character == '^') tile.character = (wind_direction % 2 == 0) ? '>' : '<';
    }
    
    // Wind affects color intensity (lighter when swaying)
    float color_intensity = 1.0f + wind_strength * 0.2f;
    tile.foreground = sf::Color(
        static_cast<sf::Uint8>(std::min(255.0f, tile.foreground.r * color_intensity)),
        static_cast<sf::Uint8>(std::min(255.0f, tile.foreground.g * color_intensity)),
        static_cast<sf::Uint8>(std::min(255.0f, tile.foreground.b * color_intensity)),
        tile.foreground.a
    );
}

sf::Color BaseVegetationObject::interpolateSeasonalColor(const sf::Color& summer, const sf::Color& autumn, 
                                                        float seasonal_factor) const {
    seasonal_factor = std::max(0.0f, std::min(1.0f, seasonal_factor));
    
    return sf::Color(
        static_cast<sf::Uint8>(summer.r + (autumn.r - summer.r) * seasonal_factor),
        static_cast<sf::Uint8>(summer.g + (autumn.g - summer.g) * seasonal_factor),
        static_cast<sf::Uint8>(summer.b + (autumn.b - summer.b) * seasonal_factor),
        summer.a
    );
}

// FIXED: Add method to get default terrain background
sf::Color BaseVegetationObject::getDefaultTerrainBackground() const {
    // Use a generic grass background - this will be overridden by more specific backgrounds
    return Core::LandColors::PLAINS_GRASS_BASE;
}

void BaseVegetationObject::ensureTileSpace(int x, int y) {
    if (y >= static_cast<int>(tiles.size())) {
        tiles.resize(y + 1);
    }
    if (x >= static_cast<int>(tiles[y].size())) {
        tiles[y].resize(x + 1);
        // FIXED: Initialize new tiles properly
        for (size_t i = tiles[y].size() - (x + 1 - tiles[y].size()); i < tiles[y].size(); ++i) {
            tiles[y][i].character = ' ';
            tiles[y][i].foreground = sf::Color::White;
            tiles[y][i].background = getDefaultTerrainBackground(); // FIXED: Use terrain background
            tiles[y][i].blocks_movement = false;
            tiles[y][i].is_canopy = false;
            tiles[y][i].animation_phase = 0.0f;
        }
    }
    
    // Update dimensions if needed
    width = std::max(width, x + 1);
    height = std::max(height, y + 1);
}

} // namespace MultiTileObjects
} // namespace Vegetation
} // namespace Systems
} // namespace World