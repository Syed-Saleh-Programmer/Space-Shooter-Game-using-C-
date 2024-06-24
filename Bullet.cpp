#include<SFML/Graphics.hpp>
#include "Bullet.h"
#include<iostream>




Bullet::Bullet(float radius) {
	bullet.setRadius(radius);
	bullet.setFillColor(sf::Color(255,150,0));
}



void Bullet::draw(sf::RenderWindow& window) {
	window.draw(bullet);
}



void Bullet::fire(float speed) {
	bullet.move(sf::Vector2f(0.0f, -speed));
}

void Bullet::setPosition(sf::Vector2f position) {
	bullet.setPosition(position);
}

sf::Vector2f Bullet::getPosition() {
	return bullet.getPosition();
}

sf::FloatRect Bullet::getGlobalBounds() {
	return bullet.getGlobalBounds();
}











