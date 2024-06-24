#pragma once
#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>

class Enemy {
protected:
    int level;
    int score;
    int health;
    sf::Sprite sprite;
    sf::Texture texture;

public:
    Enemy(int level);
    virtual ~Enemy();
    virtual int getScore() const;
    virtual void attack() const = 0;
    virtual void takeDamage(int damage);
    virtual void render(sf::RenderWindow& window);
    bool isAlive() const;
    sf::Vector2f getPosition() const; // Method to get the position of the enemy
    void setPosition(float x, float y); // Method to set the position of the enemy
};
#endif







