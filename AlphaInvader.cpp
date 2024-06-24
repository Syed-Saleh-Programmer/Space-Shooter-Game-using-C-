#include "AlphaInvader.h"
#include <iostream>

AlphaInvader::AlphaInvader(int level) : Invader(level) {
    score = 10 * level;
    loadTexture();
    setPosition(300, 100); // Set an initial position for the AlphaInvader
}

void AlphaInvader::attack() const {
    std::cout << "Alpha Invader attacking!" << std::endl;
}

void AlphaInvader::loadTexture() {
    if (!texture.loadFromFile("alpha.png")) {
        std::cerr << "Error loading alpha.png" << std::endl;
    }
    else {
        std::cout << "Alpha texture loaded successfully!" << std::endl;
        sprite.setTexture(texture);
        sprite.setScale(0.2f, 0.2f); 
    }
}



void AlphaInvader::update() {
    // Check if 5 seconds have passed to drop a bomb
    if (clock.getElapsedTime().asSeconds() >= 5.0f) {
        dropBomb();
        clock.restart();
    }

    // Update bombs 
    for (auto& bomb : bombs) {
        sf::Vector2f position = bomb.getSprite().getPosition();
        position.y += bomb.getSpeed() * 0.016f; // Move bomb down based on speed and frame time
        bomb.getSprite().setPosition(position);
    }
}

void AlphaInvader::dropBomb() {
    Bomb newBomb(10, this, 15.0f); // Create a bomb with 10 damage and a speed of 15 units per second
    bombs.push_back(newBomb);
    std::cout << "Alpha Invader dropped a bomb!" << std::endl;
}

void AlphaInvader::renderBombs(sf::RenderWindow& window) {
    for (auto& bomb : bombs) {
        bomb.render(window);
    }
}

sf::Sprite& AlphaInvader::getSprite() {
    return sprite;
}
















