#pragma once
#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include <iostream>
//#include "Game.h"
#include "GameMusic.h"

class Game;  // used for forward declaration to avoid circular dependency between Game and Menu classes

class Menu {
private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::RectangleShape overlay;
    sf::Image cursorImage;
    sf::Cursor cursor;
    int levelSelected;
    Game* gamePtr;
    GameMusic* gameMusic;
    std::string playerNameEntered;
public:

    Menu(sf::RenderWindow& window, Game* game) {

        // create objects from pointers

        this->gamePtr = game;

        gameMusic = new GameMusic();

        gameMusic->playBgMusic();  // starts playing background music when menu is created

        // set level selected to 1

        levelSelected = 1;

        if (!backgroundTexture.loadFromFile("images/menu-bg/menu-small-bg.png")) {
            std::cerr << "\nFalied To load Background Texture.";
            return;
        }

        // loading the cursor image

        if (!cursorImage.loadFromFile("images/cursor.png")) {
            std::cerr << "\nFalied To load cursor image.";
            return;
        }

        //cursor

        cursor.loadFromPixels(cursorImage.getPixelsPtr(), sf::Vector2u(cursorImage.getSize().x, cursorImage.getSize().y), sf::Vector2u(0, 0));

        window.setMouseCursor(cursor);

        backgroundSprite.setTexture(backgroundTexture);

        // menu overlay settings
        overlay.setSize(sf::Vector2f(450, 450));
        overlay.setPosition(sf::Vector2f((window.getSize().x / 2.f) - (overlay.getGlobalBounds().width / 2.f), 230));
        overlay.setFillColor(sf::Color(165, 155, 232, 167));
        overlay.setOutlineThickness(15);
        overlay.setOutlineColor(sf::Color(0, 0, 0, 50));

    }


    // destructor

    ~Menu();

    void displayMainMenu(sf::RenderWindow& window);
    void displayInstructionsMenu(sf::RenderWindow& window);
    void displayHighScoresMenu(sf::RenderWindow& window);
    void displayLevelsSelectionMenu(sf::RenderWindow& window);
    void displayCreditsMenu(sf::RenderWindow& window);
    void CreateNewPlayer(sf::RenderWindow& window);
    void displayPauseMenu(sf::RenderWindow& window);

    // functions to get and set level selections from the levels selection menu

    const int getSelectedLevel() {
        return levelSelected;
   }

    void selectLevel(int level){
        levelSelected = level;
        return;
    }

    // function to get the selected name

    std::string getEnteredName() {
        return playerNameEntered;
    }


    // menus above this line ---------------------------------------------------------------



};