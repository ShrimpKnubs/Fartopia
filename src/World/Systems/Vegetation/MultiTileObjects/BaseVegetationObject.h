// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/BaseVegetationObject.h
#pragma once
#include "../../../Tile.h"
#include "../../../../Core/Renderer.h"
#include <vector>
#include <memory>

namespace World {
namespace Systems {
namespace Vegetation {
namespace MultiTileObjects {

/**
 * Abstract base class for all multi-tile vegetation objects
 * Provides framework for trees, boulders, grass fields, etc.
 * Optimized for beautiful top-down view with no visual artifacts
 */
class BaseVegetationObject {
public:
    struct ObjectTile {
        int relative_x = 0, relative_y = 0;                    // Position relative to object origin
        char character = ' ';                                  // Character to display
        sf::Color foreground = sf::Color::White;               // Foreground color
        sf::Color background = sf::Color::Transparent;         // Background color (transparent by default)
        bool blocks_movement = false;                          // Can entities pass through?
        bool is_canopy = false;                               // Is this a tree canopy (entity can walk under)?
        float animation_phase = 0.0f;                         // For animated effects
    };

    struct EntityContext {
        bool entity_underneath = false;        // Is there an entity under this tile?
        int entity_x = -1, entity_y = -1;     // Entity position if present
    };

    BaseVegetationObject(int origin_x, int origin_y, unsigned int seed);
    virtual ~BaseVegetationObject() = default;

    // Pure virtual methods - must be implemented by derived classes
    virtual void generatePattern() = 0;                          // Create the object's tile pattern
    virtual void updateAnimation(float time_delta) = 0;          // Update animations
    virtual std::string getObjectType() const = 0;              // Get type name for debugging
    virtual bool canPlaceAt(int world_x, int world_y, 
                           const std::vector<float>& heightmap,
                           const std::vector<float>& slope_map,
                           int map_width, int map_height) const = 0; // Check if placement is valid

    // Common functionality
    ObjectTile getTileAt(int relative_x, int relative_y) const;
    ObjectTile getTileAt(int relative_x, int relative_y, const EntityContext& context) const;
    std::vector<ObjectTile> getAllTiles() const;
    std::vector<ObjectTile> getAllTiles(const EntityContext& context) const;
    
    // Getters
    int getOriginX() const { return origin_x; }
    int getOriginY() const { return origin_y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isAnimated() const { return has_animation; }
    
    // Bounds checking
    bool containsPoint(int world_x, int world_y) const;
    bool overlaps(const BaseVegetationObject& other) const;

protected:
    // Object properties
    int origin_x, origin_y;             // World coordinates of object origin
    int width, height;                  // Object dimensions
    unsigned int random_seed;           // Seed for procedural generation
    bool has_animation;                 // Does this object animate?
    float current_time;                 // Current animation time
    
    // Tile data
    std::vector<std::vector<ObjectTile>> tiles; // [y][x] grid of tiles
    
    // Helper methods for derived classes
    void setDimensions(int w, int h);
    void setTile(int x, int y, char ch, sf::Color fg, sf::Color bg, // Background can be transparent
                bool blocks = false, bool canopy = false);
    ObjectTile& getTileRef(int x, int y);
    
    // Procedural generation helpers
    sf::Color varyColor(const sf::Color& base, float variation) const;
    char selectCharFromSet(const std::string& charset, int x, int y) const;
    float getProceduralNoise(int x, int y, float frequency = 1.0f) const;
    
    // Animation helpers
    void applyWindEffect(int x, int y, float wind_strength, int wind_direction);
    sf::Color interpolateSeasonalColor(const sf::Color& summer, const sf::Color& autumn, 
                                      float seasonal_factor) const;

private:
    // Ensure tiles vector is properly sized
    void ensureTileSpace(int x, int y);
};

} // namespace MultiTileObjects
} // namespace Vegetation  
} // namespace Systems
} // namespace World