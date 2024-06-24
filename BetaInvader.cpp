#include "BetaInvader.h"
#include <iostream>

BetaInvader::BetaInvader(int level) : Invader(level) {
    score = 20 * level;
    loadTexture();
    setPosition(400, 100); // Set an initial position for the BetaInvader
}

void BetaInvader::attack() const {
    std::cout << "Beta Invader attacking!" << std::endl;
}

void BetaInvader::loadTexture() {
    if (!texture.loadFromFile("beta.png")) {
        std::cerr << "Error loading beta.png" << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setScale(0.1f, 0.1f); // Scale down the sprite
}

void BetaInvader::update() {
    // Check if 3 seconds have passed to drop a bomb
    if (clock.getElapsedTime().asSeconds() >= 3.0f) {
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

void BetaInvader::dropBomb() {
    Bomb newBomb(10, this, 15.0f); // Create a bomb with 10 damage and a speed of 15 units per second
    bombs.push_back(newBomb);
    std::cout << "Beta Invader dropped a bomb!" << std::endl;
}

void BetaInvader::renderBombs(sf::RenderWindow& window) {
    for (auto& bomb : bombs) {
        bomb.render(window);
    }
}

sf::Sprite& BetaInvader::getSprite() {
    return sprite;
}

