// File: EmergentKingdoms/src/Core/Game.h
#pragma once

#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "../World/Map.h"
#include "../Entities/Entity.h"
#include <vector>
#include <memory>

namespace Core {

class Game {
public:
    Game();
    ~Game();

    bool initialize();
    void run();

private:
    void handleEvent(sf::Event& event);
    void processContinuousInput(sf::Time deltaTime);
    void update(sf::Time delta_time);
    void render();
    void updateZoom(float new_zoom_factor);

    bool initializeMinimapOverlay();
    void updateMinimapTexture();
    void renderMinimapOverlay();

    sf::RenderWindow window;

    Renderer game_renderer;
    World::Map game_map;
    std::vector<std::unique_ptr<Entities::Entity>> entities;

    sf::Texture minimap_texture;
    sf::Sprite minimap_sprite;
    sf::RectangleShape minimap_viewport_rect;
    sf::RectangleShape minimap_background_rect;
    bool minimap_texture_needs_update;
    bool show_minimap;

    float current_zoom_factor;
    unsigned int current_sfml_font_size;
    int current_char_cell_width_px;  
    int current_char_cell_height_px; 
    int current_screen_width_chars;
    int current_screen_height_chars;

    int m_current_lod_scale;

    float camera_x;
    float camera_y;

    float render_pixel_offset_x;
    float render_pixel_offset_y;

    sf::Clock delta_clock;

    // --- Masterpiece Flowing Wave Animation System ---
    sf::Time water_animation_timer;
    sf::Clock master_wave_clock;               // Master timing for wave flow
    float master_wave_time;                    // Continuous wave time in seconds
    float tide_phase;                          // Slow tide cycle
    
    // Wave flow state
    float wave_flow_offset;                    // Primary wave flow position
    float secondary_wave_offset;               // Secondary wave pattern
    float tertiary_wave_offset;                // Third wave set
    
    // Smooth animation interpolation
    float prev_wave_time;                      // Previous frame time for interpolation
    static constexpr float WAVE_INTERPOLATION_SMOOTH = 0.92f;
};

} // namespace Core