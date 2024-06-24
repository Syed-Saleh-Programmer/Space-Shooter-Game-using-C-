#include "ContinuousFire.h"
#include <iostream>

ContinuousFire::ContinuousFire(float fireInterval, float pauseInterval, int directions)
    : fireInterval(fireInterval), pauseInterval(pauseInterval), isFiring(false) {
    // Create beams based on the number of directions
    for (int i = 0; i < directions; ++i) {
        sf::RectangleShape beam;
        beam.setSize(sf::Vector2f(10, 600)); 
        beam.setFillColor(sf::Color::Yellow); /
        beams.push_back(beam);
    }
}

void ContinuousFire::fireMonsterBeam(sf::Vector2f position) {
    std::cout << "Firing continuous beam of lightning from Monster at position: " << position.x << ", " << position.y << std::endl;
    beams[0].setPosition(position.x + 60, position.y + beams[0].getSize().y / 10); // Downwards
    isFiring = true;
}

void ContinuousFire::fireDragonBeams(sf::Vector2f position) {
    std::cout << "Firing continuous beams from Dragon at position: " << position.x << ", " << position.y << std::endl;
    beams[0].setPosition(position.x + 98, position.y + beams[0].getSize().y / 2.8); // Downwards
    beams[1].setPosition(position.x + 35, position.y + beams[1].getSize().y / 3.5); // Left-downwards
    beams[2].setPosition(position.x + 178, position.y + beams[2].getSize().y / 3.5); // Right-downwards



    isFiring = true;
}


void ContinuousFire::updateForMonster(sf::Vector2f position) {
    if (isFiring) {
        // Check if it's time to stop firing
        if (fireClock.getElapsedTime().asSeconds() >= fireInterval) {
            isFiring = false;
            pauseClock.restart(); // Start the pause timer
        }
        else {
            // Update the position of the beams
            beams[0].setPosition(position.x + 60, position.y + beams[0].getSize().y / 10); // Downwards
        }
    }
    else {
        // Check if it's time to start firing again
        if (pauseClock.getElapsedTime().asSeconds() >= pauseInterval) {
            fireMonsterBeam(position);
            fireClock.restart(); // Restart the fire timer
        }
    }
}

void ContinuousFire::updateForDragon(sf::Vector2f position) {
    if (isFiring) {
        // Check if it's time to stop firing
        if (fireClock.getElapsedTime().asSeconds() >= fireInterval) {
            isFiring = false;
            pauseClock.restart(); // Start the pause timer
        }
        else {
            // Update the position of the beams
            beams[0].setPosition(position.x + 98, position.y + beams[0].getSize().y / 2.8); // Downwards
            beams[1].setPosition(position.x + 35, position.y + beams[1].getSize().y / 3.5); // Left-downwards
            beams[1].setRotation(40.f);
            beams[2].setPosition(position.x + 178, position.y + beams[2].getSize().y / 3.5); // Right-downwards
            beams[2].setRotation(-40.f);
        }

    }
    else {
        // Check if it's time to start firing again
        if (pauseClock.getElapsedTime().asSeconds() >= pauseInterval) {
            fireDragonBeams(position);
            fireClock.restart(); // Restart the fire timer
        }
    }
}

void ContinuousFire::render(sf::RenderWindow& window) {
    if (isFiring) {
        for (auto& beam : beams) {
            window.draw(beam);
        }
    }
}
