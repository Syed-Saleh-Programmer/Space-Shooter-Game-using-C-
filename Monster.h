#pragma once
#ifndef MONSTER_H
#define MONSTER_H

#include <SFML/Graphics.hpp>
#include "ContinuousFire.h"

class Monster {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    ContinuousFire continuousFire;
    float moveSpeed;
    bool isMovingRight;
    int score;

public:
    Monster();
    void loadTexture(const std::string& filename);
    void setPosition(float x, float y);
    void update(); // Method to update the state of the monster
    void render(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;
    int getScore() const;
    bool checkCollision(sf::Sprite);

};

#endif 


















