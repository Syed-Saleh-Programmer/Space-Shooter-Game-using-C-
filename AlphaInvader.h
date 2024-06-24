
#pragma once
#ifndef ALPHA_INVADER_H
#define ALPHA_INVADER_H

#include "Invader.h"
#include "Bomb.h"
#include <SFML/System/Clock.hpp>
#include <vector>

class AlphaInvader : public Invader {
private:
    sf::Clock clock; // Clock to track time for bomb dropping
    std::vector<Bomb> bombs; // Container to hold dropped bombs

public:
    AlphaInvader(int level);
    virtual void attack() const override;
    void update(); // Method to update the AlphaInvader state
    void loadTexture();
    void dropBomb(); // Method to drop a bomb
    void renderBombs(sf::RenderWindow& window); // Method to render bombs
    sf::Sprite& getSprite(); // Method to get the sprite
};

#endif 










