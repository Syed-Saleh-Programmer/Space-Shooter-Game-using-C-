#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"
#include "AddOn.h"

using json = nlohmann::json;
using namespace std;

class Player
{
private:
    string name;
    sf::Vector2f position;
    float moveSpeed;
    int score;
    int highestScore;
    int levelProgress;
    int maxLives;
    int currentLives;
    bool alive;
    bool exists = false;
    sf::Texture playerTexture;
    sf::Sprite playerSprite;
    std::vector<AddOn> addons; // Add-ons collected by the player

public:
    Player(std::string, sf::Vector2f);
    void update(sf::Sprite);
    void move(std::string, float);
    void draw(sf::RenderWindow&);
    sf::Vector2f getPosition();
    void setPosition(sf::Vector2f);
    sf::FloatRect getLocalBounds();
    sf::FloatRect getGlobalBounds();
    bool checkCollision(sf::Sprite);
    bool checkCollision(sf::CircleShape);
    void destroyAnimation();
    void updateScore(int);
    void updateLevelProgress(int);
    void addLife(int);
    int getScore();
    int getHighestScore();
    int getLevelProgress();
    int getLives();
    void takeDamage();
    bool isAlive();
    void setAlive(bool aliveVal) {
        alive = aliveVal;
    }
    std::string getName();
    void setName(std::string);
    void setScale(sf::Vector2f scale) {
        playerSprite.setScale(scale);
    }
    sf::Vector2f getScale() {
        return playerSprite.getScale();
    }

    bool getExists() {
        return exists;
    }


    // New methods for saving and loading player data
    void saveToFile();
    void loadFromFile();

    sf::Sprite getSprite() {
        return playerSprite;
    }

    // add on 

    /*void collectAddOn(AddOn addOn);
    void handleAddOns();*/
};

