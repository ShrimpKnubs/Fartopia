// File: EmergentKingdoms/src/main.cpp
#include "Core/Game.h"
#include <iostream> // For std::ios_base::sync_with_stdio

// No need for windows.h or SetConsoleOutputCP for SFML graphical output

int main() {
    // std::ios_base::sync_with_stdio(false); // Less relevant for SFML output performance
    // std::cin.tie(NULL); // Less relevant

    Core::Game emergentKingdomsGame;
    emergentKingdomsGame.run(); // This now contains the SFML game loop

    return 0;
}