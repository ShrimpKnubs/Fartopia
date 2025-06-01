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
 * Resource Boulder - Large stone formations (6x6 to 30x30) with integrated resource veins
 * Features: Natural rock patterns, moss growth, embedded gold/silver/iron/copper veins
 * Optimized for beautiful top-down view with no visual artifacts
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
        SMALL,      // 6x6
        MEDIUM,     // 10x10  
        LARGE,      // 18x18
        MASSIVE     // 30x30
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
    
    // Boulder generation - optimized for beautiful top-down view
    void determineBoulderSize();
    void generateNaturalBoulderShape();
    void generateResourceVeins();
    void generateMossPatches();
    void addWeatheringDetails();
    
    // Resource vein generation  
    void generateNaturalVein(int start_x, int start_y, float angle, int length, float thickness);
    void addResourceDeposit(int x, int y, float thickness);
    
    // Moss generation  
    void addMossPatch(int center_x, int center_y, int radius);
    bool shouldHaveMoss(int x, int y, int center_x, int center_y, int radius) const;
    
    // Animation methods
    void updateResourceSparkle(float time_delta);
    void updateMossGrowth(float time_delta);
    
    // Visual methods - create rich, natural appearance without artifacts
    char selectBoulderCharacter(int x, int y, float distance_factor) const;
    char selectResourceCharacter(ResourceType type) const;
    char selectMossCharacter(int x, int y) const;
    sf::Color getBoulderForegroundColor(int x, int y, float distance_factor) const;
    sf::Color getBoulderBackgroundColor(int x, int y, float distance_factor) const;
    sf::Color getResourceColor(ResourceType type, bool sparkling = false) const;
    sf::Color getMossColor(int x, int y) const;
    
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