#pragma once
#include<SFML/Graphics.hpp>

class Bullet
{
private:
	sf::CircleShape bullet;
public:
	Bullet(float radius);
	void fire(float speed);
	void draw(sf::RenderWindow&);
	void setPosition(sf::Vector2f);
	sf::Vector2f getPosition();
	sf::FloatRect getGlobalBounds();
};




