#pragma once
#ifndef INVADER_H
#define INVADER_H

#include "Enemy.h"

class Invader : public Enemy {
public:
    Invader(int level);
    virtual void attack() const override = 0; // Pure virtual to make Invader an abstract class
};

#endif 










