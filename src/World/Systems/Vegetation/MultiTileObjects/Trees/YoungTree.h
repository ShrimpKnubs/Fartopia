// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/Trees/YoungTree.h
#pragma once
#include "../BaseVegetationObject.h"
#include "../../VegetationColors.h"

namespace World {
namespace Systems {
namespace Vegetation {
namespace MultiTileObjects {
namespace Trees {

/**
 * Young Tree - Smaller (4x4) growing tree with simple but dynamic appearance
 * Features: Slender trunk, smaller canopy, more responsive to wind
 */
class YoungTree : public BaseVegetationObject {
public:
    YoungTree(int origin_x, int origin_y, unsigned int seed);
    
    void generatePattern() override;
    void updateAnimation(float time_delta) override;
    std::string getObjectType() const override { return "Young Tree"; }
    
    bool canPlaceAt(int world_x, int world_y, 
                   const std::vector<float>& heightmap,
                   const std::vector<float>& slope_map,
                   int map_width, int map_height) const override;

private:
    // Tree type (birch, oak sapling, pine sapling, etc.)
    enum class TreeType {
        BIRCH_SAPLING,
        OAK_SAPLING, 
        PINE_SAPLING,
        WILLOW_SAPLING
    };
    
    struct YoungTreeParams {
        TreeType type;
        sf::Color trunk_color;
        sf::Color leaf_color;
        char trunk_char;
        std::string canopy_chars;
        float wind_sensitivity;     // How much it sways in wind
    };
    YoungTreeParams params;
    
    // Animation state
    float wind_sway_phase;
    float growth_animation;
    
    // Generation methods
    void determineTreeType();
    void generateTrunk();
    void generateCanopy();
    
    // Animation methods
    void updateWindSway(float time_delta);
    void updateGrowthAnimation(float time_delta);
    
    // Utility methods
    char selectCanopyChar(int x, int y) const;
    sf::Color getAnimatedLeafColor(int x, int y) const;
    bool isValidTerrain(float height, float slope) const;
    
    // FIXED: Add terrain background method
    sf::Color getTerrainBackground() const;
};

} // namespace Trees
} // namespace MultiTileObjects
} // namespace Vegetation
} // namespace Systems
} // namespace World