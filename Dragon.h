#pragma once
#ifndef DRAGON_H
#define DRAGON_H

#include <SFML/Graphics.hpp>
#include "ContinuousFire.h"

class Dragon {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    ContinuousFire continuousFire;
    sf::Clock visibilityClock;
    float visibilityDuration;
    float interval;
    bool isVisible;
    int score;
    bool hasAnimated;

public:
    Dragon();
    void loadTexture(const std::string& filename);
    void setPosition(float x, float y);
    void update(); // Method to update the state of the dragon
    void render(sf::RenderWindow& window);
    bool isAlive() const;
    void setVisible(bool visible);
    sf::Vector2f getPosition() const;
    int getScore() const;
    float getHeight() const {
        return sprite.getGlobalBounds().height;
    }
    void move(float offsetX, float offsetY) {
        sprite.move(offsetX, offsetY);
    }
    bool checkCollision(sf::Sprite);
};

#endif











