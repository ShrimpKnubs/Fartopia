// File: EmergentKingdoms/src/Core/Renderer.h
#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "BaseConfig.h"

namespace Core {

struct ScreenCell {
    char character = ' ';
    sf::Color fg_color = Colors::WHITE;
    sf::Color bg_color = Colors::BLACK; 

    ScreenCell() = default;
    ScreenCell(char ch, const sf::Color& fg = Colors::WHITE, const sf::Color& bg = Colors::BLACK)
        : character(ch), fg_color(fg), bg_color(bg) {}
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool initialize(sf::RenderWindow& window, unsigned int initial_sfml_font_size);
    void updateLayout(unsigned int new_sfml_font_size, 
                      int new_char_cell_width_px, int new_char_cell_height_px,
                      int new_screen_width_chars, int new_screen_height_chars);
    void prepareFrame();
    void setCell(int x, int y, char character, const sf::Color& fg_color = Colors::WHITE, const sf::Color& bg_color = Colors::BLACK);
    void setCell(int x, int y, const ScreenCell& cell);
    // MODIFIED: Reinstated pixel_shift parameters
    void render(sf::RenderWindow& window, float pixel_shift_x = 0.0f, float pixel_shift_y = 0.0f); 
    void shutdown();
    bool isInitialized() const { return initialized; }


private:
    sf::Font font;

    sf::VertexArray background_vertices;
    sf::VertexArray foreground_vertices;
    const sf::Texture* font_texture_atlas;


    std::vector<ScreenCell> next_buffer;

    unsigned int current_sfml_font_size_internal;
    int current_char_cell_width_px_internal;
    int current_char_cell_height_px_internal;
    int current_screen_width_chars_internal;
    int current_screen_height_chars_internal;

    bool initialized = false;
};

} // namespace Core