// File: EmergentKingdoms/src/Entities/Fartling.h
#pragma once
#include "Entity.h"
#include "../Core/BaseConfig.h" // For Core::Colors (which are now sf::Color)
#include "../Core/Renderer.h" // For Core::ScreenCell
#include <cstdlib>          // For rand()

namespace Entities {

class Fartling : public Entity {
public:
    Fartling(int start_x, int start_y)
        : Entity(start_x, start_y, Core::ScreenCell('F', Core::Colors::RED)) { // Explicitly construct ScreenCell, use RED
    }

    void update() override {
        // Simple random movement
        int move_x = (rand() % 3) - 1; // -1, 0, or 1
        int move_y = (rand() % 3) - 1; // -1, 0, or 1
        
        // x and y are map coordinates, Game loop will handle wrapping/clamping to map boundaries
        x += move_x;
        y += move_y;
    }
};

} // namespace Entities