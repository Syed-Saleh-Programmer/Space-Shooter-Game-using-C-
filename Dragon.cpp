#include "Dragon.h"
#include <iostream>

Dragon::Dragon()
    : continuousFire(2.0f, 2.0f, 3), visibilityDuration(5.0f), interval(10.0f), isVisible(false), score(50) {
    // Constructor implementation with 2 seconds firing interval and 2 seconds pause interval
}

void Dragon::loadTexture(const std::string& filename) {
    if (!texture.loadFromFile(filename)) {
        std::cerr << "Error loading " << filename << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setScale(0.9f, 0.9f); // Adjust the scale to make the dragon smaller
}

void Dragon::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void Dragon::update() {
    const float speed = 10.0f; // Adjust speed as needed

    if (isVisible) {
        if (visibilityClock.getElapsedTime().asSeconds() >= visibilityDuration) {
            isVisible = false;
        }
        else {
            continuousFire.updateForDragon(sprite.getPosition());
        }
    }
    else {
        // Gradually move the dragon downwards until it reaches the center
        sf::Vector2f position = sprite.getPosition();
        position.y += speed;
        sprite.setPosition(position);

        // If the dragon reaches the center, stop
        if (position.y >= 0) { 
            isVisible = true;
            visibilityClock.restart();
        }
    }
}



void Dragon::render(sf::RenderWindow& window) {
    if (isVisible) {
        window.draw(sprite);
        continuousFire.render(window);
    }
}

bool Dragon::isAlive() const {
    return isVisible;
}

void Dragon::setVisible(bool visible) {
    isVisible = visible;
    if (isVisible) {
        visibilityClock.restart();
    }
}

sf::Vector2f Dragon::getPosition() const {
    return sprite.getPosition();
}

int Dragon::getScore() const {
    return score;
}


bool Dragon::checkCollision(sf::Sprite object) {
    if (sprite.getGlobalBounds().intersects(object.getGlobalBounds())) {
        return true;
    }
    else return false;
}

