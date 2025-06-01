// File: EmergentKingdoms/src/Entities/Entity.h
#pragma once
#include "../Core/Renderer.h" // For ScreenCell, which now uses sf::Color

namespace Entities {

class Entity {
public:
    int x, y; // Position in map coordinates
    Core::ScreenCell display_cell;

    Entity(int start_x, int start_y, Core::ScreenCell cell)
        : x(start_x), y(start_y), display_cell(cell) {}

    virtual ~Entity() = default;

    virtual void update() {
        // Base update logic
    }

    // Drawing is handled by the Game's render loop by querying display_cell
    // virtual void draw(Core::Renderer& renderer) const {
    //     renderer.setCell(x, y, display_cell);
    // }
};

} // namespace Entities