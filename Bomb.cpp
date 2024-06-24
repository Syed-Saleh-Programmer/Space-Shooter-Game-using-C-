
#include <SFML/Graphics.hpp>
#include "Bomb.h"
#include <iostream>

//Bomb::Bomb(int damage, Enemy* owner, float speed) : damage(damage), owner(owner), speed(speed) {
//    
//}

void Bomb::explode() {
    std::cout << "Bomb explodes with damage: " << damage << std::endl;
    if (owner) {
        owner->takeDamage(damage);
    }
}

void Bomb::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::Sprite& Bomb::getSprite() {
    return sprite;
}

float Bomb::getSpeed() const {
    return speed;
}









