// File: EmergentKingdoms/src/Core/Renderer.cpp
#include "Renderer.h"
#include <iostream>
#include <cmath> 

namespace Core {

Renderer::Renderer() : 
    font_texture_atlas(nullptr),
    current_sfml_font_size_internal(0),
    current_char_cell_width_px_internal(0),
    current_char_cell_height_px_internal(0),
    current_screen_width_chars_internal(0),
    current_screen_height_chars_internal(0)
{
    background_vertices.setPrimitiveType(sf::Quads);
    foreground_vertices.setPrimitiveType(sf::Quads);
}

Renderer::~Renderer() {
}

bool Renderer::initialize(sf::RenderWindow& window, unsigned int initial_sfml_font_size) {
    (void)window;
    if (!font.loadFromFile(FONT_PATH)) {
        std::cerr << "Renderer::initialize() - ERROR: Could not load font from " << FONT_PATH << std::endl;
        return false;
    }
    current_sfml_font_size_internal = initial_sfml_font_size; 
    font_texture_atlas = &font.getTexture(current_sfml_font_size_internal);
    
    initialized = true;
    return true;
}

void Renderer::updateLayout(unsigned int new_sfml_font_size, 
                            int new_char_cell_width_px, int new_char_cell_height_px,
                            int new_screen_width_chars, int new_screen_height_chars) {
    if (!initialized) return;

    bool font_size_changed = (current_sfml_font_size_internal != new_sfml_font_size);
    current_sfml_font_size_internal = new_sfml_font_size;
    current_char_cell_width_px_internal = new_char_cell_width_px;
    current_char_cell_height_px_internal = new_char_cell_height_px;
    
    current_screen_width_chars_internal = new_screen_width_chars + 2; 
    current_screen_height_chars_internal = new_screen_height_chars + 2;


    if (font_size_changed && font.getInfo().family != "") { 
         font_texture_atlas = &font.getTexture(current_sfml_font_size_internal);
    }
    
    size_t buffer_size = static_cast<size_t>(current_screen_width_chars_internal) * current_screen_height_chars_internal;
    if (buffer_size > 0) { 
        next_buffer.resize(buffer_size);
    } else {
        next_buffer.clear(); 
    }
    prepareFrame(); 
}


void Renderer::prepareFrame() {
    if (!initialized || next_buffer.empty()) return;
    ScreenCell empty_cell(' ', Colors::WHITE, Colors::DEFAULT_BG); 
    for (size_t i = 0; i < next_buffer.size(); ++i) {
        next_buffer[i] = empty_cell;
    }
}

void Renderer::setCell(int x, int y, char character, const sf::Color& fg_color, const sf::Color& bg_color) {
    if (!initialized || next_buffer.empty()) return;
    if (x >= 0 && x < current_screen_width_chars_internal && y >= 0 && y < current_screen_height_chars_internal) {
        size_t index = static_cast<size_t>(y) * current_screen_width_chars_internal + x;
        next_buffer[index].character = character;
        next_buffer[index].fg_color = fg_color;
        next_buffer[index].bg_color = bg_color; 
    }
}

void Renderer::setCell(int x, int y, const ScreenCell& cell) {
    if (!initialized || next_buffer.empty()) return;
     if (x >= 0 && x < current_screen_width_chars_internal && y >= 0 && y < current_screen_height_chars_internal) {
        size_t index = static_cast<size_t>(y) * current_screen_width_chars_internal + x;
        next_buffer[index] = cell;
    }
}

void Renderer::render(sf::RenderWindow& window, float pixel_shift_x, float pixel_shift_y) {
    if (!initialized || next_buffer.empty() || font_texture_atlas == nullptr ||
        current_char_cell_width_px_internal <= 0 || current_char_cell_height_px_internal <=0) {
        return; 
    }

    background_vertices.clear(); 
    foreground_vertices.clear(); 

    float cell_width_float = static_cast<float>(current_char_cell_width_px_internal);
    float cell_height_float = static_cast<float>(current_char_cell_height_px_internal);


    for (int y_char_grid = 0; y_char_grid < current_screen_height_chars_internal; ++y_char_grid) {
        for (int x_char_grid = 0; x_char_grid < current_screen_width_chars_internal; ++x_char_grid) {
            size_t buffer_idx = static_cast<size_t>(y_char_grid) * current_screen_width_chars_internal + x_char_grid;
            
            const ScreenCell& cell_to_draw = next_buffer[buffer_idx];

            float grid_pos_x = static_cast<float>(x_char_grid * current_char_cell_width_px_internal);
            float grid_pos_y = static_cast<float>(y_char_grid * current_char_cell_height_px_internal);
            
            float final_pos_x = grid_pos_x + pixel_shift_x;
            float final_pos_y = grid_pos_y + pixel_shift_y;

            background_vertices.append(sf::Vertex(sf::Vector2f(final_pos_x, final_pos_y), cell_to_draw.bg_color));
            background_vertices.append(sf::Vertex(sf::Vector2f(final_pos_x + cell_width_float, final_pos_y), cell_to_draw.bg_color));
            background_vertices.append(sf::Vertex(sf::Vector2f(final_pos_x + cell_width_float, final_pos_y + cell_height_float), cell_to_draw.bg_color));
            background_vertices.append(sf::Vertex(sf::Vector2f(final_pos_x, final_pos_y + cell_height_float), cell_to_draw.bg_color));

            if (cell_to_draw.character != ' ' && cell_to_draw.character != '\0') {
                sf::Glyph glyph = font.getGlyph(cell_to_draw.character, current_sfml_font_size_internal, false); 

                if (glyph.textureRect.width > 0 && glyph.textureRect.height > 0) {
                    float u1 = static_cast<float>(glyph.textureRect.left);
                    float v1 = static_cast<float>(glyph.textureRect.top);
                    float u2 = static_cast<float>(glyph.textureRect.left + glyph.textureRect.width);
                    float v2 = static_cast<float>(glyph.textureRect.top + glyph.textureRect.height);
                    
                    float char_offset_x = (cell_width_float - static_cast<float>(glyph.textureRect.width)) / 2.0f;
                    float char_render_pos_x = final_pos_x + char_offset_x;

                    float char_offset_y = (cell_height_float - static_cast<float>(glyph.textureRect.height)) / 2.0f;
                    float char_render_pos_y = final_pos_y + char_offset_y;

                    // Removed unused line_spacing variable and commented out alternatives
                    // float line_spacing = font.getLineSpacing(current_sfml_font_size_internal);

                    foreground_vertices.append(sf::Vertex(sf::Vector2f(char_render_pos_x, char_render_pos_y), cell_to_draw.fg_color, sf::Vector2f(u1, v1)));
                    foreground_vertices.append(sf::Vertex(sf::Vector2f(char_render_pos_x + glyph.textureRect.width, char_render_pos_y), cell_to_draw.fg_color, sf::Vector2f(u2, v1)));
                    foreground_vertices.append(sf::Vertex(sf::Vector2f(char_render_pos_x + glyph.textureRect.width, char_render_pos_y + glyph.textureRect.height), cell_to_draw.fg_color, sf::Vector2f(u2, v2)));
                    foreground_vertices.append(sf::Vertex(sf::Vector2f(char_render_pos_x, char_render_pos_y + glyph.textureRect.height), cell_to_draw.fg_color, sf::Vector2f(u1, v2)));
                }
            }
        }
    }
    
    if (background_vertices.getVertexCount() > 0) {
        window.draw(background_vertices);
    }
    if (font_texture_atlas && foreground_vertices.getVertexCount() > 0) { 
        sf::RenderStates states;
        states.texture = font_texture_atlas;
        window.draw(foreground_vertices, states);
    }
}

void Renderer::shutdown() {
    initialized = false;
}

} // namespace Core