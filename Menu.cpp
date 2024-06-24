#include "Menu.h"
#include "Game.h"
#include "Player.h"
#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>

using json = nlohmann::json;


Menu::~Menu() {
    //delete gamePtr;
}


// Main Menu --------------------------------------------------------------------------------------

void Menu::displayMainMenu(sf::RenderWindow& window) {

    // Load button textures
    sf::Texture playButtonTexture, instructionsButtonTexture, highScoresButtonTexture, creditsButtonTexture, exitButtonTexture;
    if (!playButtonTexture.loadFromFile("images/buttons/play.png") ||
        !instructionsButtonTexture.loadFromFile("images/buttons/instructions.png") ||
        !highScoresButtonTexture.loadFromFile("images/buttons/high-scores.png") ||
        !creditsButtonTexture.loadFromFile("images/buttons/credits.png") || 
        !exitButtonTexture.loadFromFile("images/buttons/exit.png")) {
        std::cerr << "Failed to load button textures." << std::endl;
        return;
    }

    sf::Sprite playButtonSprite(playButtonTexture), instructionsButtonSprite(instructionsButtonTexture),
        highScoresButtonSprite(highScoresButtonTexture), creditsButtonSprite(creditsButtonTexture), exitButtonSprite(exitButtonTexture);

    playButtonSprite.setPosition((window.getSize().x / 2.0f) - (playButtonSprite.getGlobalBounds().width / 2.0f), 250.f);
    instructionsButtonSprite.setPosition((window.getSize().x / 2.0f) - (instructionsButtonSprite.getGlobalBounds().width / 2.0f), 330.f);
    highScoresButtonSprite.setPosition((window.getSize().x / 2.0f) - (highScoresButtonSprite.getGlobalBounds().width / 2.0f), 410.f);
    creditsButtonSprite.setPosition((window.getSize().x / 2.0f) - (creditsButtonSprite.getGlobalBounds().width / 2.0f), 490.f);
    exitButtonSprite.setPosition((window.getSize().x / 2.0f) - (exitButtonSprite.getGlobalBounds().width / 2.0f), 570.f);



    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                gameMusic->playClickSound();
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (playButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    CreateNewPlayer(window);
                }
                else if (instructionsButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    displayInstructionsMenu(window);
                }
                else if (highScoresButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    displayHighScoresMenu(window);
                }
                else if (creditsButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    displayCreditsMenu(window);
                }
                else if (exitButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.close();
                }
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(overlay);
        window.draw(playButtonSprite);
        window.draw(instructionsButtonSprite);
        window.draw(highScoresButtonSprite);
        window.draw(creditsButtonSprite);
        window.draw(exitButtonSprite);
        window.display();
    }
}




// Instructions Menu ---------------------------------------------------------------------------

void Menu::displayInstructionsMenu(sf::RenderWindow& window) {

    // Load button texture
    sf::Texture backButtonTexture;
    if (!backButtonTexture.loadFromFile("images/buttons/back.png")) {
        std::cerr << "Failed to load button texture." << std::endl;
        return;
    }
    sf::Sprite backButtonSprite(backButtonTexture);
    backButtonSprite.setPosition((window.getSize().x / 2.0f) - (backButtonSprite.getGlobalBounds().width / 2.0f), 250.f); // Adjust position as needed


    // paragraph font
    sf::Font instructionsFont;
    if (!instructionsFont.loadFromFile("fonts/small.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }

    sf::Text instructionsTxt;

    instructionsTxt.setFont(instructionsFont);
    instructionsTxt.setCharacterSize(18);

    instructionsTxt.setPosition(overlay.getPosition().x + 15, overlay.getPosition().y + 100);

    instructionsTxt.setString("1. Use Up, Down, Left, And Right arrow keys \n to move around.\n\n2. Click Space key to fire bullets.\n\n3. Collect AdOn's to get bonus life or powers.\n\n4. Dodge Monster Dragon and Danger Sign.\n\n5. Score For Dodging Enemies:\n\ta. Monster(40) b. Dragon(50) c. Danger(5)");
    instructionsTxt.setOutlineThickness(2);
    instructionsTxt.setOutlineColor(sf::Color::Black);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                gameMusic->playClickSound();
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (backButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    displayMainMenu(window);
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Backspace)
                    displayMainMenu(window);
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(overlay);
        window.draw(backButtonSprite);
        window.draw(instructionsTxt);
        window.display();
    }
}


// High Scores Menu with file handling-------------------------------------------------


void Menu::displayHighScoresMenu(sf::RenderWindow& window) {
    // Load button texture
    sf::Texture backButtonTexture;
    if (!backButtonTexture.loadFromFile("images/buttons/back.png")) {
        std::cerr << "Failed to load button texture." << std::endl;
        return;
    }
    sf::Sprite backButtonSprite(backButtonTexture);
    backButtonSprite.setPosition((window.getSize().x / 2.0f) - (backButtonSprite.getGlobalBounds().width / 2.0f), 550.f);

    // 1st badge texture
    sf::Texture firstBadgeTexture;
    if (!firstBadgeTexture.loadFromFile("images/badges/1st.png")) {
        std::cerr << "Failed to load button texture." << std::endl;
        return;
    }
    sf::Sprite firstBadgeSprite(firstBadgeTexture);
    firstBadgeSprite.setPosition(window.getSize().x / 2.0f + 80, 280.f);

    // 2nd button texture
    sf::Texture secondBadgeTexture;
    if (!secondBadgeTexture.loadFromFile("images/badges/2nd.png")) {
        std::cerr << "Failed to load button texture." << std::endl;
        return;
    }
    sf::Sprite secondBadgeSprite(secondBadgeTexture);
    secondBadgeSprite.setPosition(window.getSize().x / 2.0f + 85, 320.f);

    // 3rd button texture
    sf::Texture thirdBadgeTexture;
    if (!thirdBadgeTexture.loadFromFile("images/badges/3rd.png")) {
        std::cerr << "Failed to load button texture." << std::endl;
        return;
    }
    sf::Sprite thirdBadgeSprite(thirdBadgeTexture);
    thirdBadgeSprite.setPosition(window.getSize().x / 2.0f+ 80, 360.f);

    // settings for the badges
    firstBadgeSprite.setScale({ 0.065, 0.065 });
    secondBadgeSprite.setScale({ 0.065, 0.065 });
    thirdBadgeSprite.setScale({ 0.065, 0.065 });

    // Load high scores from JSON file
    std::vector<std::pair<std::string, int>> highScores;
    std::ifstream file("ssPlayers.json");
    if (file.is_open()) {
        json j;
        try {
            file >> j;
            if (j.is_array()) {
                for (const auto& entry : j) {
                    if (entry.contains("name") && entry.contains("score") && entry["score"].is_number()) {
                        highScores.push_back({ entry["name"].get<std::string>(), entry["score"].get<int>() });
                    }
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error reading high scores: " << e.what() << std::endl;
        }
        file.close();

        // Sort high scores in descending order
        std::sort(highScores.begin(), highScores.end(), [](const auto& a, const auto& b) {
            return b.second < a.second;
            });

        // Keep only the top 5 scores
        if (highScores.size() > 5) {
            highScores.resize(5);
        }
    }
    else {
        std::cerr << "Failed to load high scores file." << std::endl;
    }

    // Font and text setup for displaying high scores
    sf::Font font;
    if (!font.loadFromFile("fonts/small.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }

    std::vector<sf::Text> highScoreTexts;
    for (size_t i = 0; i < highScores.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(highScores[i].first + " \t\t " + std::to_string(highScores[i].second));
        text.setCharacterSize(24);
        text.setLineSpacing(8.f);
        text.setFillColor(sf::Color::White);
        text.setOutlineThickness(2);
        text.setOutlineColor(sf::Color::Black);
        text.setPosition((window.getSize().x / 2.0f) - 200, 280.f + i * 40); // Adjust position as needed
        highScoreTexts.push_back(text);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                gameMusic->playClickSound();
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (backButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    displayMainMenu(window);
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Backspace)
                    displayMainMenu(window);
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(overlay);
        window.draw(backButtonSprite);
        window.draw(firstBadgeSprite);
        window.draw(secondBadgeSprite);
        window.draw(thirdBadgeSprite);

        for (const auto& text : highScoreTexts) {
            window.draw(text);
        }

        window.display();
    }
}


// levels Selection Menu ------------------------------------------------------------------

void Menu::displayLevelsSelectionMenu(sf::RenderWindow& window) {

    // Load button textures
    sf::Texture backButtonTexture, level1ButtonTexture, level2ButtonTexture, level3ButtonTexture;
    if (!backButtonTexture.loadFromFile("images/buttons/back.png") ||
        !level1ButtonTexture.loadFromFile("images/buttons/level1.png") ||
        !level2ButtonTexture.loadFromFile("images/buttons/level2.png") ||
        !level3ButtonTexture.loadFromFile("images/buttons/level3.png")) {
        std::cerr << "Failed to load button textures." << std::endl;
        return;
    }

    sf::Sprite backButtonSprite(backButtonTexture), level1ButtonSprite(level1ButtonTexture),
        level2ButtonSprite(level2ButtonTexture), level3ButtonSprite(level3ButtonTexture);

    backButtonSprite.setPosition((window.getSize().x / 2.0f) - (backButtonSprite.getGlobalBounds().width / 2.0f), 250.f); // Adjust position as needed
    level1ButtonSprite.setPosition((window.getSize().x / 2.0f) - (level1ButtonSprite.getGlobalBounds().width / 2.0f), 350.f); // Adjust position as needed
    level2ButtonSprite.setPosition((window.getSize().x / 2.0f) - (level2ButtonSprite.getGlobalBounds().width / 2.0f), 450.f); // Adjust position as needed
    level3ButtonSprite.setPosition((window.getSize().x / 2.0f) - (level3ButtonSprite.getGlobalBounds().width / 2.0f), 550.f); // Adjust position as needed

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                gameMusic->playClickSound();
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (backButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    displayMainMenu(window);
                    
                }
                else if (level1ButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    selectLevel(1);
                    gameMusic->stopBgMusic();
                    gamePtr->startGame(window);
                }
                else if (level2ButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    selectLevel(2);
                }
                else if (level3ButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    selectLevel(3);
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Backspace) {
                    displayMainMenu(window);
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                    
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(overlay);
        window.draw(backButtonSprite);
        window.draw(level1ButtonSprite);
        window.draw(level2ButtonSprite);
        window.draw(level3ButtonSprite);
        window.display();
    }
}



// credits menu --------------------------------------------------------------------------------


void Menu::displayCreditsMenu(sf::RenderWindow& window) {

    // Load button texture
    sf::Texture backButtonTexture;
    if (!backButtonTexture.loadFromFile("images/buttons/back.png")) {
        std::cerr << "Failed to load button texture." << std::endl;
        return;
    }
    sf::Sprite backButtonSprite(backButtonTexture);
    backButtonSprite.setPosition((window.getSize().x / 2.0f) - (backButtonSprite.getGlobalBounds().width / 2.0f), 250.f); // Adjust position as needed


    // Text for developers

    // heading font
    sf::Font headingFont;
    if (!headingFont.loadFromFile("fonts/dark-italic.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }

    // paragraph font
    sf::Font paraFont;
    if (!paraFont.loadFromFile("fonts/small.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }


    sf::Text headingText, creditsText;
    headingText.setFont(headingFont);
    creditsText.setFont(paraFont);

    headingText.setString("Team");
    headingText.setStyle(sf::Text::Underlined);
    headingText.setOutlineThickness(2);
    headingText.setOutlineColor(sf::Color::Black);
    creditsText.setString("----- Project Manager -----\n\t\t\tMr. Imran Ihsan\n\n\n----- Core Developers -----\n\t\t\tMr. S M Saleh\n\t\t\tMr. M Hayat");
    creditsText.setOutlineThickness(2);
    creditsText.setOutlineColor(sf::Color::Black);

    headingText.setCharacterSize(35);
    creditsText.setCharacterSize(20);

    headingText.setPosition((window.getSize().x / 2.0f) - (headingText.getGlobalBounds().width / 2.0f), 320.f);
    creditsText.setPosition((window.getSize().x / 2.0f) - (creditsText.getGlobalBounds().width / 2.0f), 390.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::BackSpace) {
                    displayMainMenu(window);
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                gameMusic->playClickSound();
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (backButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    displayMainMenu(window);
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Backspace)
                    displayMainMenu(window);
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(overlay);
        window.draw(headingText);
        window.draw(creditsText);
        window.draw(backButtonSprite);
        window.display();
    }

}


// Create New Player Menu ---------------------------------------------------------------------------


void Menu::CreateNewPlayer(sf::RenderWindow& window) {
    // Load fonts
    sf::Font headingFont, textFont;
    if (!headingFont.loadFromFile("fonts/dark-italic.ttf") ||
        !textFont.loadFromFile("fonts/small.ttf")) {
        std::cerr << "Failed to load fonts." << std::endl;
        return;
    }

    // Heading text
    sf::Text headingText;
    headingText.setFont(headingFont);
    headingText.setString("Enter your Name");
    headingText.setCharacterSize(35);
    //headingText.setStyle(sf::Text::Underlined);
    headingText.setOutlineThickness(2);
    headingText.setOutlineColor(sf::Color::Black);
    headingText.setPosition(sf::Vector2f(overlay.getPosition().x + 40, overlay.getPosition().y + 100));

    // Input box setup
    sf::RectangleShape inputBox(sf::Vector2f(400, 50));
    inputBox.setFillColor(sf::Color::White);
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(sf::Color::Black);
    inputBox.setPosition(sf::Vector2f(overlay.getPosition().x + 40, overlay.getPosition().y + 200));

    // Text entered by the user
    sf::Text inputText;
    inputText.setFont(textFont);
    inputText.setCharacterSize(25);
    inputText.setFillColor(sf::Color::Black);
    inputText.setPosition(inputBox.getPosition().x + 10, inputBox.getPosition().y + 10);
    inputText.setString("Player1");

    //std::string playerNameEntered;

    // Load button texture
    sf::Texture startButtonTexture;
    if (!startButtonTexture.loadFromFile("images/buttons/play.png")) {
        std::cerr << "Failed to load button texture." << std::endl;
        return;
    }
    sf::Sprite startButtonSprite(startButtonTexture);
    startButtonSprite.setPosition(sf::Vector2f(overlay.getPosition().x + (overlay.getSize().x /2.f) - (startButtonSprite.getGlobalBounds().width/2.f), overlay.getPosition().y + 300));

    bool showStartButton = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') { // Handle backspace
                    if (!playerNameEntered.empty()) {
                        playerNameEntered.pop_back();
                    }
                }
                else if (event.text.unicode < 128 && playerNameEntered.size() < 7) { // Handle character input
                    playerNameEntered += static_cast<char>(event.text.unicode);
                }
                inputText.setString(playerNameEntered);
                showStartButton = !playerNameEntered.empty();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (startButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    gameMusic->playClickSound();
                    std::cout << "\nPlayer Name: " << playerNameEntered;
                    gameMusic->stopBgMusic();
                    gamePtr->startGame(window);
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    displayMainMenu(window);
                }
            }
        }

        window.clear(sf::Color(0,0,0,0));
        window.draw(backgroundSprite);
        window.draw(overlay);
        window.draw(headingText);
        window.draw(inputBox);
        window.draw(inputText);
        if (showStartButton) {
            window.draw(startButtonSprite);
        }
        window.display();
    }
}




