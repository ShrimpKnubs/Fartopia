// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/Trees/AncientOakTree.h
#pragma once
#include "../BaseVegetationObject.h"
#include "../../VegetationColors.h"

namespace World {
namespace Systems {
namespace Vegetation {
namespace MultiTileObjects {
namespace Trees {

/**
 * Majestic Ancient Oak Tree - Large (15x15) procedurally generated tree
 * Features: Thick trunk, sprawling canopy, seasonal color changes, wind swaying
 */
class AncientOakTree : public BaseVegetationObject {
public:
    AncientOakTree(int origin_x, int origin_y, unsigned int seed);
    
    void generatePattern() override;
    void updateAnimation(float time_delta) override;
    std::string getObjectType() const override { return "Ancient Oak"; }
    
    bool canPlaceAt(int world_x, int world_y, 
                   const std::vector<float>& heightmap,
                   const std::vector<float>& slope_map,
                   int map_width, int map_height) const override;

private:
    // Tree-specific properties
    struct TreeParams {
        int trunk_radius;           // Radius of the trunk base
        int canopy_radius;          // Radius of the full canopy
        float canopy_density;       // How dense the leaves are (0-1)
        float trunk_irregularity;   // How irregular the trunk shape is
        sf::Color trunk_color;      // Base trunk color
        sf::Color leaf_color_summer; // Summer leaf color
        sf::Color leaf_color_autumn; // Autumn leaf color
    };
    TreeParams params;
    
    // Animation state
    float wind_sway_phase;
    float seasonal_factor;      // 0=summer, 1=autumn
    
    // Generation methods
    void generateTrunk();
    void generateCanopy();
    void generateRoots();
    void addCanopyLayer(int center_x, int center_y, int radius, float density);
    void addTrunkSection(int center_x, int center_y, int radius);
    
    // Animation methods
    void updateWindSway(float wind_strength, int wind_direction);
    void updateSeasonalColors();
    
    // Utility methods
    bool isInCanopyRadius(int x, int y, int center_x, int center_y, int radius) const;
    char selectTrunkCharacter(int x, int y) const;
    char selectCanopyCharacter(int x, int y, float density) const;
    sf::Color getLeafColor(int x, int y) const;
    sf::Color getTrunkColor(int x, int y) const;
    
    // FIXED: Add terrain background method
    sf::Color getTerrainBackground() const;
    
    // Placement validation
    bool isValidTerrain(float height, float slope) const;
};

} // namespace Trees
} // namespace MultiTileObjects
} // namespace Vegetation
} // namespace Systems
} // namespace World