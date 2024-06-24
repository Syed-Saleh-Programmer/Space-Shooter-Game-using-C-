#include "GammaInvader.h"
#include <iostream>

GammaInvader::GammaInvader(int level) : Invader(level) {
    score = 30 * level;
    loadTexture();
    setPosition(200, 100); // Set an initial position for the GammaInvader
}

void GammaInvader::attack() const {
    std::cout << "Gamma Invader attacking!" << std::endl;
}

void GammaInvader::loadTexture() {
    if (!texture.loadFromFile("gamma.png")) {
        std::cerr << "Error loading gamma_invader.png" << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setScale(0.2f, 0.2f); // Scale down the sprite to half its size
}

void GammaInvader::update() {
    // Check if 2 seconds have passed to drop a bomb
    if (clock.getElapsedTime().asSeconds() >= 2.0f) {
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

void GammaInvader::dropBomb() {
    Bomb newBomb(10, this, 15.0f); // Create a bomb with 10 damage and a speed of 50 units per second
    bombs.push_back(newBomb);
    std::cout << "Gamma Invader dropped a bomb!" << std::endl;
}

void GammaInvader::renderBombs(sf::RenderWindow& window) {
    for (auto& bomb : bombs) {
        bomb.render(window);
    }
}


