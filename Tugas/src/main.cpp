#include "gameball.hpp"
#include <iostream>

int main() {
    try {
        Game game;  // Create game instance
        game.run(); // Start the game
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl; // Print any errors
    }
    return 0;
}
