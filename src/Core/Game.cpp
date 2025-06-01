// File: EmergentKingdoms/src/Core/Game.cpp
#include "Game.h"
#include "BaseConfig.h"
#include "../World/Map.h"
#include "../Entities/Fartling.h" 
#include "../World/Systems/Lakes/LakeConfig.h"
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath> 

namespace Core {

Game::Game() :
    window(sf::VideoMode(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX), "Emergent Kingdoms", sf::Style::Default),
    game_map(MAP_WIDTH, MAP_HEIGHT, static_cast<unsigned int>(std::time(nullptr))),
    minimap_texture_needs_update(true),
    show_minimap(true),
    current_zoom_factor(1.0f),
    m_current_lod_scale(1),
    camera_x(0.0f),
    camera_y(0.0f),
    render_pixel_offset_x(0.0f),
    render_pixel_offset_y(0.0f),
    water_animation_timer(sf::Time::Zero),
    master_wave_time(0.0f),
    tide_phase(0.0f),
    wave_flow_offset(0.0f),
    secondary_wave_offset(0.0f),
    tertiary_wave_offset(0.0f),
    prev_wave_time(0.0f)
{
    srand(static_cast<unsigned int>(std::time(nullptr)));
    window.setFramerateLimit(0); 
    window.setVerticalSyncEnabled(true);
    
    // Initialize masterpiece wave timing
    master_wave_clock.restart();
}

Game::~Game() {
    // No explicit cleanup needed here for now
}

bool Game::initialize() { 
    updateZoom(1.0f); 
    if (!game_renderer.initialize(window, current_sfml_font_size)) {
        std::cerr << "Game::initialize(): Renderer failed to initialize!" << std::endl;
        return false;
    }
    game_renderer.updateLayout(current_sfml_font_size,
                               current_char_cell_width_px, current_char_cell_height_px,
                               current_screen_width_chars, current_screen_height_chars);
    if (!initializeMinimapOverlay()) {
         std::cerr << "Game::initialize(): Minimap Overlay failed to initialize!" << std::endl;
    }
    
    // ===== FIXED: ADD MAP GENERATION CALL =====
    std::cout << "Starting world generation..." << std::endl;
    game_map.generate();
    std::cout << "World generation completed." << std::endl;
    
    float initial_effective_screen_width_map_tiles = static_cast<float>(current_screen_width_chars * m_current_lod_scale);
    float initial_effective_screen_height_map_tiles = static_cast<float>(current_screen_height_chars * m_current_lod_scale);
    camera_x = (MAP_WIDTH > initial_effective_screen_width_map_tiles) ? static_cast<float>(MAP_WIDTH - initial_effective_screen_width_map_tiles) / 2.0f : 0.0f;
    camera_y = (MAP_HEIGHT > initial_effective_screen_height_map_tiles) ? static_cast<float>(MAP_HEIGHT - initial_effective_screen_height_map_tiles) / 2.0f : 0.0f;
    camera_x = std::fmod(std::fmod(camera_x, static_cast<float>(MAP_WIDTH)) + static_cast<float>(MAP_WIDTH), static_cast<float>(MAP_WIDTH));
    float max_camera_y_tiles = static_cast<float>(MAP_HEIGHT) - initial_effective_screen_height_map_tiles;
    if (max_camera_y_tiles < 0) max_camera_y_tiles = 0; 
    camera_y = std::max(0.0f, std::min(camera_y, max_camera_y_tiles));
    entities.push_back(std::make_unique<Entities::Fartling>(MAP_WIDTH / 2, MAP_HEIGHT / 2));
    entities.push_back(std::make_unique<Entities::Fartling>(MAP_WIDTH / 3, MAP_HEIGHT / 3));
    delta_clock.restart(); 
    return true;
}

bool Game::initializeMinimapOverlay() { 
    if (!minimap_texture.create(MAP_WIDTH, MAP_HEIGHT)) {
        std::cerr << "Error creating minimap texture!" << std::endl; 
        return false;
    }
    minimap_sprite.setTexture(minimap_texture);
    float minimap_w = static_cast<float>(WINDOW_WIDTH_PX) * MINIMAP_OVERLAY_WIDTH_FACTOR;
    float minimap_h = static_cast<float>(WINDOW_HEIGHT_PX) * MINIMAP_OVERLAY_HEIGHT_FACTOR;
    minimap_sprite.setScale( minimap_w / MAP_WIDTH, minimap_h / MAP_HEIGHT );
    minimap_sprite.setPosition(WINDOW_WIDTH_PX - minimap_w - MINIMAP_MARGIN_PX, MINIMAP_MARGIN_PX);
    minimap_background_rect.setSize(sf::Vector2f(minimap_w, minimap_h));
    minimap_background_rect.setPosition(minimap_sprite.getPosition());
    minimap_background_rect.setFillColor(sf::Color(50, 50, 50, 150));
    minimap_background_rect.setOutlineColor(sf::Color(150,150,150, 200));
    minimap_background_rect.setOutlineThickness(1.0f);
    minimap_viewport_rect.setFillColor(sf::Color(255, 255, 255, 70));
    minimap_viewport_rect.setOutlineColor(sf::Color::White);
    minimap_viewport_rect.setOutlineThickness(1.0f); 
    minimap_texture_needs_update = true;
    return true;
}

void Game::updateZoom(float new_zoom_factor) { 
    current_zoom_factor = std::max(MIN_ZOOM_FACTOR, std::min(MAX_ZOOM_FACTOR, new_zoom_factor));
    m_current_lod_scale = 1;
    if (current_zoom_factor < LOD_ACTIVATION_ZOOM_FACTOR) {
        m_current_lod_scale = LOD_AGGREGATION_SCALE;
    }
    float visual_tile_width_px = INITIAL_CHAR_CELL_WIDTH_PX * current_zoom_factor;
    float visual_tile_height_px = INITIAL_CHAR_CELL_HEIGHT_PX * current_zoom_factor;
    current_char_cell_width_px = static_cast<int>(std::max(1.0f, visual_tile_width_px * m_current_lod_scale));
    current_char_cell_height_px = static_cast<int>(std::max(1.0f, visual_tile_height_px * m_current_lod_scale));
    current_sfml_font_size = static_cast<unsigned int>(std::max(1.0f, INITIAL_SFML_FONT_SIZE * current_zoom_factor));
    current_sfml_font_size = std::max(1u, current_sfml_font_size); 
    if (current_char_cell_width_px > 0) {
        current_screen_width_chars = WINDOW_WIDTH_PX / current_char_cell_width_px;
    } else {
        current_screen_width_chars = WINDOW_WIDTH_PX;
    } 
    current_screen_width_chars = std::max(1, current_screen_width_chars);
    if (current_char_cell_height_px > 0) {
        current_screen_height_chars = WINDOW_HEIGHT_PX / current_char_cell_height_px;
    } else {
        current_screen_height_chars = WINDOW_HEIGHT_PX;
    } 
    current_screen_height_chars = std::max(1, current_screen_height_chars);
    if (game_renderer.isInitialized()) {
         game_renderer.updateLayout(current_sfml_font_size,
                                   current_char_cell_width_px, current_char_cell_height_px,
                                   current_screen_width_chars, current_screen_height_chars);
    }
    float effective_screen_height_map_tiles = static_cast<float>(current_screen_height_chars * m_current_lod_scale);
    float max_camera_y_tiles = static_cast<float>(MAP_HEIGHT) - effective_screen_height_map_tiles;
    if (max_camera_y_tiles < 0) max_camera_y_tiles = 0;
    camera_y = std::max(0.0f, std::min(camera_y, max_camera_y_tiles)); 
}

void Game::run() { 
    if (!initialize()) {
        std::cerr << "Game::run(): Game failed to initialize!" << std::endl;
        return;
    }
    sf::Time time_since_last_update = sf::Time::Zero;
    sf::Time frame_delta_time_for_input; 
    while (window.isOpen()) {
        sf::Time frame_delta_time = delta_clock.restart();
        time_since_last_update += frame_delta_time;
        frame_delta_time_for_input = frame_delta_time;
        sf::Event event;
        while (window.pollEvent(event)) {
            handleEvent(event);
        }
        processContinuousInput(frame_delta_time_for_input); 
        while (time_since_last_update.asMicroseconds() >= MS_PER_TICK * 1000) {
             time_since_last_update -= sf::microseconds(MS_PER_TICK * 1000);
             update(sf::microseconds(MS_PER_TICK * 1000)); 
        }
        if (!window.isOpen()) break; 
        render();
    }
    game_renderer.shutdown(); 
}

void Game::handleEvent(sf::Event& event) { 
    if (event.type == sf::Event::Closed) {
        window.close();
        return;
    }
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Escape) {
            window.close();
            return;
        }
        if (event.key.code == sf::Keyboard::Add || event.key.code == sf::Keyboard::Equal) {
            updateZoom(current_zoom_factor * ZOOM_STEP_MULTIPLIER);
        }
        if (event.key.code == sf::Keyboard::Subtract || event.key.code == sf::Keyboard::Dash) {
            updateZoom(current_zoom_factor / ZOOM_STEP_MULTIPLIER);
        }
        if (event.key.code == sf::Keyboard::M) {
            show_minimap = !show_minimap;
        }
    }
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            if (event.mouseWheelScroll.delta > 0) updateZoom(current_zoom_factor * ZOOM_STEP_MULTIPLIER);
            else if (event.mouseWheelScroll.delta < 0) updateZoom(current_zoom_factor / ZOOM_STEP_MULTIPLIER);
        }
    }
     if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (show_minimap) {
                sf::Vector2i mouse_pos_window = sf::Mouse::getPosition(window);
                if (minimap_background_rect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mouse_pos_window))) {
                    sf::Vector2f minimap_sprite_pos = minimap_sprite.getPosition();
                    sf::Vector2f minimap_sprite_scale = minimap_sprite.getScale();
                    if (minimap_sprite_scale.x == 0.0f || minimap_sprite_scale.y == 0.0f) return; 
                    float click_relative_x_px = static_cast<float>(mouse_pos_window.x) - minimap_sprite_pos.x;
                    float click_relative_y_px = static_cast<float>(mouse_pos_window.y) - minimap_sprite_pos.y;
                    float target_map_x_tiles = click_relative_x_px / minimap_sprite_scale.x;
                    float target_map_y_tiles = click_relative_y_px / minimap_sprite_scale.y;
                    float effective_screen_width_map_tiles = static_cast<float>(current_screen_width_chars * m_current_lod_scale);
                    float effective_screen_height_map_tiles = static_cast<float>(current_screen_height_chars * m_current_lod_scale);
                    camera_x = target_map_x_tiles - (effective_screen_width_map_tiles / 2.0f);
                    camera_y = target_map_y_tiles - (effective_screen_height_map_tiles / 2.0f);
                    camera_x = std::fmod(std::fmod(camera_x, static_cast<float>(MAP_WIDTH)) + static_cast<float>(MAP_WIDTH), static_cast<float>(MAP_WIDTH));
                    float max_camera_y_tiles = static_cast<float>(MAP_HEIGHT) - effective_screen_height_map_tiles;
                     if (max_camera_y_tiles < 0) max_camera_y_tiles = 0;
                    camera_y = std::max(0.0f, std::min(camera_y, max_camera_y_tiles));
                }
            }
        }
    }
}

void Game::processContinuousInput(sf::Time deltaTime) { 
    float current_camera_scroll_speed = CAMERA_SCROLL_SPEED_TILES_PER_SEC;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
        current_camera_scroll_speed *= 2.0f; 
    }
    float move_speed_map_tiles = current_camera_scroll_speed * deltaTime.asSeconds();
    float move_x_map_tiles = 0.0f;
    float move_y_map_tiles = 0.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    move_y_map_tiles -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  move_y_map_tiles += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  move_x_map_tiles -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) move_x_map_tiles += 1.0f;
    if (move_x_map_tiles != 0.0f || move_y_map_tiles != 0.0f) {
        if (move_x_map_tiles != 0.0f && move_y_map_tiles != 0.0f) {
            float length = std::sqrt(move_x_map_tiles * move_x_map_tiles + move_y_map_tiles * move_y_map_tiles);
            move_x_map_tiles = (move_x_map_tiles / length) * move_speed_map_tiles;
            move_y_map_tiles = (move_y_map_tiles / length) * move_speed_map_tiles;
        } else {
            move_x_map_tiles *= move_speed_map_tiles;
            move_y_map_tiles *= move_speed_map_tiles;
        }
        camera_x += move_x_map_tiles;
        camera_y += move_y_map_tiles;
        camera_x = std::fmod(std::fmod(camera_x, static_cast<float>(MAP_WIDTH)) + static_cast<float>(MAP_WIDTH), static_cast<float>(MAP_WIDTH));
        float effective_screen_height_map_tiles = static_cast<float>(current_screen_height_chars * m_current_lod_scale);
        float max_camera_y_tiles = static_cast<float>(MAP_HEIGHT) - effective_screen_height_map_tiles;
        if (max_camera_y_tiles < 0) max_camera_y_tiles = 0;
        camera_y = std::max(0.0f, std::min(camera_y, max_camera_y_tiles));
    }
}

void Game::update(sf::Time delta_time) { 
    if (!window.isOpen()) return;
    
    // Masterpiece flowing wave animation system
    sf::Time frame_delta = master_wave_clock.restart();
    float delta_seconds = frame_delta.asSeconds();
    
    // Smooth time interpolation to prevent stuttering
    float target_time = master_wave_time + delta_seconds;
    master_wave_time = master_wave_time * WAVE_INTERPOLATION_SMOOTH + target_time * (1.0f - WAVE_INTERPOLATION_SMOOTH);
    
    // Calculate flowing wave phases with natural physics
    tide_phase = std::sin(master_wave_time * 2.0f * 3.14159f / World::Systems::Lakes::TIDE_CYCLE_TIME) * 0.5f + 0.5f;
    
    // Primary wave set - main rolling waves toward shore
    float primary_cycle = master_wave_time / World::Systems::Lakes::WAVE_CYCLE_TIME;
    wave_flow_offset = std::fmod(primary_cycle, 1.0f);
    
    // Secondary wave set - offset for natural rhythm  
    float secondary_cycle = (master_wave_time * 0.7f) / World::Systems::Lakes::WAVE_CYCLE_TIME;
    secondary_wave_offset = std::fmod(secondary_cycle + 0.33f, 1.0f);
    
    // Tertiary wave set - creates complex, natural patterns
    float tertiary_cycle = (master_wave_time * 1.3f) / World::Systems::Lakes::WAVE_CYCLE_TIME;
    tertiary_wave_offset = std::fmod(tertiary_cycle + 0.66f, 1.0f);
    
    // Store for interpolation
    prev_wave_time = master_wave_time;
    
    // Maintain compatibility with existing water timer
    water_animation_timer += delta_time;
    
    // Update entities
    for (auto& entity : entities) { 
        entity->update();
        entity->x = (entity->x % MAP_WIDTH + MAP_WIDTH) % MAP_WIDTH;
        entity->y = (entity->y % MAP_HEIGHT + MAP_HEIGHT) % MAP_HEIGHT; 
    }
}

void Game::updateMinimapTexture() { 
    if (!minimap_texture_needs_update) return;
    sf::Image image;
    image.create(MAP_WIDTH, MAP_HEIGHT); 
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            const World::Tile& tile = game_map.getTile(x, y);
            Core::ScreenCell minimap_cell = World::Tile::determineDisplay(
                tile.base_type, tile.height_val, tile.slope_val, tile.aspect_val, 
                tile.distance_to_land, 
                0.0f, // global_water_animation_progress (static for minimap)
                tile.animation_offset,
                tile.wave_strand_intensity,
                tile.is_marsh_water_patch,
                tile.distance_to_water // Pass distance_to_water
            );
            image.setPixel(x, y, minimap_cell.bg_color);
        }
    }
    minimap_texture.update(image);
    minimap_texture_needs_update = false; 
}

void Game::render() {
    window.clear(Colors::DEFAULT_BG); 
    game_renderer.prepareFrame(); 

    float camera_lod_cell_x = camera_x / static_cast<float>(m_current_lod_scale);
    float camera_lod_cell_y = camera_y / static_cast<float>(m_current_lod_scale);
    render_pixel_offset_x = -(std::fmod(camera_lod_cell_x, 1.0f) * current_char_cell_width_px);
    render_pixel_offset_y = -(std::fmod(camera_lod_cell_y, 1.0f) * current_char_cell_height_px);
    
    int chars_to_draw_x = current_screen_width_chars + 2; 
    int chars_to_draw_y = current_screen_height_chars + 2;

    // Use the masterpiece flowing wave animation progress
    float current_water_animation_progress = wave_flow_offset;

    for (int screen_char_y = 0; screen_char_y < chars_to_draw_y; ++screen_char_y) {
        for (int screen_char_x = 0; screen_char_x < chars_to_draw_x; ++screen_char_x) {
            int map_tile_to_fetch_x = static_cast<int>(std::floor(camera_x / m_current_lod_scale) + screen_char_x) * m_current_lod_scale;
            int map_tile_to_fetch_y = static_cast<int>(std::floor(camera_y / m_current_lod_scale) + screen_char_y) * m_current_lod_scale;
            
            if (map_tile_to_fetch_y < 0 || map_tile_to_fetch_y >= MAP_HEIGHT) continue;

            Core::ScreenCell cell_for_renderer;
            
            // ===== NEW: CHECK FOR MULTI-TILE VEGETATION OBJECTS FIRST =====
            if (game_map.hasVegetationObjectAt(map_tile_to_fetch_x, map_tile_to_fetch_y)) {
                // Find entity position for under-canopy rendering
                int entity_x = -1, entity_y = -1;
                for (const auto& entity : entities) {
                    if (static_cast<int>(entity->x) == map_tile_to_fetch_x && 
                        static_cast<int>(entity->y) == map_tile_to_fetch_y) {
                        entity_x = static_cast<int>(entity->x);
                        entity_y = static_cast<int>(entity->y);
                        break;
                    }
                }
                
                cell_for_renderer = game_map.getVegetationObjectDisplay(
                    map_tile_to_fetch_x, map_tile_to_fetch_y, entity_x, entity_y);
            } else {
                // ===== ORIGINAL TILE RENDERING =====
                const World::Tile& tile_to_draw = game_map.getTile(map_tile_to_fetch_x, map_tile_to_fetch_y);
                
                cell_for_renderer = World::Tile::determineDisplay(
                    tile_to_draw.base_type, 
                    tile_to_draw.height_val, 
                    tile_to_draw.slope_val, 
                    tile_to_draw.aspect_val, 
                    tile_to_draw.distance_to_land, 
                    current_water_animation_progress,
                    tile_to_draw.animation_offset,
                    tile_to_draw.wave_strand_intensity,
                    tile_to_draw.is_marsh_water_patch,
                    tile_to_draw.distance_to_water
                );
            }

            if (m_current_lod_scale > 1) { 
                cell_for_renderer.character = ' '; 
            }
            game_renderer.setCell(screen_char_x, screen_char_y, cell_for_renderer);
        }
    }

    // Entity rendering - same as before
    for (const auto& entity : entities) { 
        float entity_rel_map_x = static_cast<float>(entity->x) - camera_x;
        float entity_rel_map_y = static_cast<float>(entity->y) - camera_y;
        if (std::abs(entity_rel_map_x) > static_cast<float>(MAP_WIDTH) / 2.0f) {
            if (entity_rel_map_x > 0) entity_rel_map_x -= static_cast<float>(MAP_WIDTH);
            else entity_rel_map_x += static_cast<float>(MAP_WIDTH);
        }
        int screen_char_x = static_cast<int>(std::floor(entity_rel_map_x / m_current_lod_scale));
        int screen_char_y = static_cast<int>(std::floor(entity_rel_map_y / m_current_lod_scale));
        if (screen_char_x >= 0 && screen_char_x < chars_to_draw_x && screen_char_y >= 0 && screen_char_y < chars_to_draw_y) {
            Core::ScreenCell entity_cell = entity->display_cell;
            game_renderer.setCell(screen_char_x, screen_char_y, entity_cell);
        }
    }
    
    game_renderer.render(window, render_pixel_offset_x, render_pixel_offset_y);

    if (show_minimap) {
        renderMinimapOverlay();
    }

    window.display();
}

void Game::renderMinimapOverlay() { 
    updateMinimapTexture(); 
    sf::View main_view = window.getView();
    sf::View minimap_view_fixed(sf::FloatRect(0.f, 0.f, static_cast<float>(WINDOW_WIDTH_PX), static_cast<float>(WINDOW_HEIGHT_PX)));
    window.setView(minimap_view_fixed);
    window.draw(minimap_background_rect);
    window.draw(minimap_sprite);
    float vp_map_x_tiles = camera_x;
    float vp_map_y_tiles = camera_y;
    float vp_map_w_tiles = static_cast<float>(current_screen_width_chars * m_current_lod_scale);
    float vp_map_h_tiles = static_cast<float>(current_screen_height_chars * m_current_lod_scale);
    sf::Vector2f minimap_pos = minimap_sprite.getPosition();
    sf::Vector2f minimap_scale = minimap_sprite.getScale();
    float first_part_w_tiles = vp_map_w_tiles;
    float second_part_w_tiles = 0.0f;
    if (vp_map_x_tiles + vp_map_w_tiles > MAP_WIDTH) {
        first_part_w_tiles = static_cast<float>(MAP_WIDTH) - vp_map_x_tiles;
        second_part_w_tiles = vp_map_w_tiles - first_part_w_tiles;
    }
    minimap_viewport_rect.setOutlineThickness(1.0f); 
    minimap_viewport_rect.setPosition(std::round(minimap_pos.x + vp_map_x_tiles * minimap_scale.x), std::round(minimap_pos.y + vp_map_y_tiles * minimap_scale.y));
    minimap_viewport_rect.setSize(sf::Vector2f(std::round(first_part_w_tiles * minimap_scale.x), std::round(vp_map_h_tiles * minimap_scale.y)));
    window.draw(minimap_viewport_rect);
    if (second_part_w_tiles > 0) {
        minimap_viewport_rect.setPosition(std::round(minimap_pos.x), std::round(minimap_pos.y + vp_map_y_tiles * minimap_scale.y));
        minimap_viewport_rect.setSize(sf::Vector2f(std::round(second_part_w_tiles * minimap_scale.x), std::round(vp_map_h_tiles * minimap_scale.y)));
        window.draw(minimap_viewport_rect);
    }
    window.setView(main_view); 
}

} // namespace Core