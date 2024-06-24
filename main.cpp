
#include <SFML/Graphics.hpp>
#include<iostream>
#include "Game.h"

using namespace std;

int main() {
	sf::RenderWindow window(sf::VideoMode(780, 780), "Space Invaders | Semester Project | Syed M Saleh | M Hayat", sf::Style::Close);

	Game newGame(window);

	return 0;
}














