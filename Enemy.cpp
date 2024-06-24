#include "Enemy.h"
#include <iostream>

Enemy::Enemy(int level) : level(level), score(0), health(100) {} // Initialize health to 100

Enemy::~Enemy() {}

int Enemy::getScore() const {
    return score;
}

void Enemy::takeDamage(int damage) {
    health -= damage;
    std::cout << "Enemy takes " << damage << " damage, health is now " << health << std::endl;
    if (health <= 0) {
        std::cout << "Enemy destroyed!" << std::endl;
        
    }
}

void Enemy::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

bool Enemy::isAlive() const {
    return health > 0;
}

sf::Vector2f Enemy::getPosition() const {
    return sprite.getPosition();
}

void Enemy::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}


