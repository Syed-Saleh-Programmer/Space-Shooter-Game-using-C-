#pragma once
#ifndef GAMMA_INVADER_H
#define GAMMA_INVADER_H

#include "Invader.h"
#include "Bomb.h"
#include <SFML/System/Clock.hpp>
#include <vector>

class GammaInvader : public Invader {
private:
    sf::Clock clock; // Clock to track time for bomb dropping
    std::vector<Bomb> bombs; // Container to hold dropped bombs

public:
    GammaInvader(int level);
    virtual void attack() const override;
    void update(); // Method to update the GammaInvader state
    void loadTexture();
    void dropBomb(); // Method to drop a bomb
    void renderBombs(sf::RenderWindow& window); // Method to render bombs
};

#endif 



