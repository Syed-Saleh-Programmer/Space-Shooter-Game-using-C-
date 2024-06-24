#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class AddOn {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    std::string type; // "powerup", "fire", "danger", "life"
    bool active;
    sf::Clock durationTimer;

public:
    AddOn(std::string type, sf::Vector2f position) : type(type), active(false) {
        if (type == "powerup") {
            texture.loadFromFile("images/power-ups/power.png");
        }
        else if (type == "fire") {
            texture.loadFromFile("images/power-ups/fire.png");
        }
        else if (type == "danger") {
            texture.loadFromFile("images/power-ups/danger.png");
        }
        else if (type == "life") {
            texture.loadFromFile("images/power-ups/life.png");
        }
        sprite.setTexture(texture);
        sprite.setPosition(position);
    }

    void activate() {
        active = true;
        durationTimer.restart();
    }

    void deactivate() {
        active = false;
    }

    bool isActive() {
        return active;
    }

    std::string getType() {
        return type;
    }

    sf::FloatRect getGlobalBounds() {
        return sprite.getGlobalBounds();
    }

    sf::Sprite& getSprite() { // Function to access sprite
        return sprite;
    }

    void update() {
        // Check if the duration has expired
        if (durationTimer.getElapsedTime().asSeconds() >= 5.0f) {
            active = false;
        }
    }

    void draw(sf::RenderWindow& window) {
        if (active) {
            window.draw(sprite);
        }
    }
};
