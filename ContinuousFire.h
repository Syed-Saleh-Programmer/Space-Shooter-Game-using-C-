#pragma once
#ifndef CONTINUOUSFIRE_H
#define CONTINUOUSFIRE_H

#include <SFML/Graphics.hpp>
#include <vector>

class ContinuousFire {
private:
    sf::Clock fireClock; // Clock to track time for firing beams
    sf::Clock pauseClock; // Clock to track the pause duration
    float fireInterval;
    float pauseInterval; // Interval for pause duration
    std::vector<sf::RectangleShape> beams; // Shapes to represent the beams
    bool isFiring; // To track if the beam is currently firing

    void fireMonsterBeam(sf::Vector2f position); // Method to fire Monster beams
    void fireDragonBeams(sf::Vector2f position); // Method to fire Dragon beams

public:
    ContinuousFire(float fireInterval, float pauseInterval, int directions = 1);
    void updateForMonster(sf::Vector2f position); // Method to update the state of continuous fire for Monster
    void updateForDragon(sf::Vector2f position); // Method to update the state of continuous fire for Dragon
    void render(sf::RenderWindow& window); // Method to render the beams
};

#endif 












