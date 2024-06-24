#pragma once
#ifndef BOMB_H
#define BOMB_H

#include <SFML/Graphics.hpp>
#include "Enemy.h"
#include<iostream>

using namespace std;

class Bomb {
private:
    int damage;
    float speed; // Speed attribute for the bomb
    Enemy* owner;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Bomb(int damage, Enemy* owner, float speed) : damage(damage), owner(owner), speed(speed) {
        if (!texture.loadFromFile("bomb.png")) {
            std::cerr << "Error loading bomb.png" << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setScale(0.4f, 0.4f); // Scale down the bomb sprite to 30% of its original size
        // Set the initial position of the bomb to be the same as the owner's position
        if (owner) {
            sprite.setPosition(owner->getPosition());
        }
    }
    void explode();
    void render(sf::RenderWindow& window);
    sf::Sprite& getSprite(); // Method to get the sprite for rendering and modification
    float getSpeed() const; // Getter method for speed

};

#endif 







