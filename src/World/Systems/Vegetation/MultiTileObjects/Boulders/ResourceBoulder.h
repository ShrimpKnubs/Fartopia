// File: EmergentKingdoms/src/World/Systems/Vegetation/MultiTileObjects/Boulders/ResourceBoulder.h
#pragma once
#include "../BaseVegetationObject.h"
#include "../../VegetationColors.h"

namespace World {
namespace Systems {
namespace Vegetation {
namespace MultiTileObjects {
namespace Boulders {

/**
 * Resource Boulder - Large stone formations (5x5 to 30x30) with integrated resource veins
 * Features: Procedural rock patterns, moss growth, embedded gold/silver/iron/copper veins
 */
class ResourceBoulder : public BaseVegetationObject {
public:
    enum class ResourceType {
        NONE,           // Plain boulder with no resources
        GOLD_VEINS,     // Contains gold deposits
        SILVER_VEINS,   // Contains silver deposits  
        IRON_DEPOSITS,  // Contains iron ore
        COPPER_DEPOSITS // Contains copper ore
    };
    
    enum class BoulderSize {
        SMALL,      // 5x5
        MEDIUM,     // 8x8  
        LARGE,      // 15x15
        MASSIVE     // 25x25+
    };
    
    ResourceBoulder(int origin_x, int origin_y, unsigned int seed, 
                   BoulderSize size = BoulderSize::MEDIUM, 
                   ResourceType resource = ResourceType::NONE);
    
    void generatePattern() override;
    void updateAnimation(float time_delta) override;
    std::string getObjectType() const override { return "Resource Boulder"; }
    
    bool canPlaceAt(int world_x, int world_y, 
                   const std::vector<float>& heightmap,
                   const std::vector<float>& slope_map,
                   int map_width, int map_height) const override;
    
    // Resource-specific methods
    ResourceType getResourceType() const { return resource_type; }
    int getResourceYield() const;
    bool hasResources() const { return resource_type != ResourceType::NONE; }

private:
    struct BoulderParams {
        BoulderSize size;
        ResourceType resource_type;
        sf::Color base_stone_color;
        sf::Color moss_color;
        sf::Color resource_color;
        float moss_coverage;        // 0-1, how much moss covers the boulder
        float resource_density;     // 0-1, how much of the resource is visible
        int vein_count;            // Number of resource veins
        float weathering_factor;    // How weathered/cracked the stone is
    };
    BoulderParams params;
    
    // Animation state
    float sparkle_phase;           // For animated resource glints
    float moss_growth_phase;       // Moss slowly spreads over time
    
    // Boulder generation
    void determineBoulderSize();
    void generateStoneBase();
    void generateResourceVeins();
    void generateMossPatches();
    void generateCracksAndTexture();
    
    // Resource vein generation
    void generateVein(int start_x, int start_y, int length, float thickness);
    void addVeinSegment(int x, int y, float thickness);
    
    // Moss generation  
    void addMossPatch(int center_x, int center_y, int radius);
    bool shouldHaveMoss(int x, int y, int center_x, int center_y, int radius) const;
    
    // Animation methods
    void updateResourceSparkle(float time_delta);
    void updateMossGrowth(float time_delta);
    
    // Utility methods
    char selectStoneCharacter(int x, int y) const;
    char selectResourceCharacter(ResourceType type) const;
    char selectMossCharacter(int x, int y) const;
    sf::Color getStoneColor(int x, int y) const;
    sf::Color getResourceColor(ResourceType type, bool sparkling = false) const;
    sf::Color getMossColor(int x, int y) const;
    
    // FIXED: Add terrain background method
    sf::Color getTerrainBackground() const;
    
    // Placement validation
    bool isValidTerrain(float height, float slope) const;
    int getSizeInTiles(BoulderSize size) const;
    
    // Resource calculations
    static int getBaseResourceYield(ResourceType type, BoulderSize size);
    
    ResourceType resource_type;
    BoulderSize boulder_size;
};

} // namespace Boulders
} // namespace MultiTileObjects
} // namespace Vegetation
} // namespace Systems
} // namespace World