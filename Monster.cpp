#include "Monster.h"
#include <iostream>

Monster::Monster()
    : continuousFire(2.0f, 2.0f, 1), moveSpeed(250.0f), isMovingRight(true), score(40) {
    // Constructor implementation with 2 seconds firing interval and 2 seconds pause interval
}

void Monster::loadTexture(const std::string& filename) {
    if (!texture.loadFromFile(filename)) {
        std::cerr << "Error loading " << filename << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setScale(0.15f, 0.15f); // Adjust the scale to make the monster smaller
}

void Monster::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void Monster::update() {
    if (isMovingRight) {
        sprite.move(moveSpeed * 0.0066f, 0);
        if (sprite.getPosition().x >= 550) {
            isMovingRight = false;
        }
    }
    else {
        sprite.move(-moveSpeed * 0.0006f, 0);
        if (sprite.getPosition().x <= 100) {
            isMovingRight = true;
        }
    }
    continuousFire.updateForMonster(sprite.getPosition() + sf::Vector2f(35,0));
}

void Monster::render(sf::RenderWindow& window) {
    continuousFire.render(window);
    window.draw(sprite);
}

sf::Vector2f Monster::getPosition() const {
    return sprite.getPosition();
}

int Monster::getScore() const {
    return score;
}


bool Monster::checkCollision(sf::Sprite object) {
    if (sprite.getGlobalBounds().intersects(object.getGlobalBounds())) {
        return true;
    }
    else return false;
}








