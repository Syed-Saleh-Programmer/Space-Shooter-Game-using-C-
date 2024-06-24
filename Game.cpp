#include "Game.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Bullet.h"
#include "Player.h"
#include "Bomb.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include "Enemy.h"
#include "invader.h"
#include "Monster.h"
#include "Dragon.h"
#include "ContinuousFire.h"
#include <cstdlib>
#include <ctime> 


void Game::startGame(sf::RenderWindow& gameWindow) {

    // play the gameplay background music

    gameMusic->playGameplayMusic();

    // Get the name from the menu
    std::string playerName = gameMenu->getEnteredName();

    // Create the player with the name entered in the createNewPlayerMenu
    player = new Player(playerName, sf::Vector2f(0.4f, 0.4f));
    player->saveToFile();

    if (player->getExists() && player->getLives() == 0) {
        player->addLife(3); // adding 3 lives to the previously dead player
        player->setAlive(true); //setting the player status to alive
        player->setScale({ 0.4f, 0.4f });
    }

    //loading levels according to player's saved game
    if (player->getLevelProgress() == 0 || player->getLevelProgress() == 1) {
        loadL1phase1(alphaTexture, 4, 15, 15.f, gameWindow);
    }
    if (player->getLevelProgress() == 2) {
        loadL2phase1(alphaTexture, 20, 150, gameWindow, player, gameMusic);
    }
    if (player->getLevelProgress() == 3) {
        loadL3phase1(alphaTexture, 6, gameWindow, player, gameMusic);
    }
   
   
}


// load level function

void Game::loadL1phase1(sf::Texture& invaderTexture, int rows, int cols, float spacing, sf::RenderWindow& window) {
    
    player->updateLevelProgress(1);

    //hiding the mouse cursor
    window.setMouseCursorVisible(false);

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background_1.png")) {
        std::cerr << "\nFalied To load Background Texture.";
        return;
    }


    //beta texure
    sf::Texture betaTexture;
    sf::Sprite betaSprite;

    if (!betaTexture.loadFromFile("images/alpha_invader.png")) {
        std::cerr << "\nFailed to load beta texture.";
    }

    betaSprite.setTexture(betaTexture);


    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
       
    
    backgroundSprite.setScale({ 2.f, 1.5f });
    
    // Create a vector to hold the invaders and their target positions
    std::vector<sf::Sprite> invaders;
    std::vector<sf::Vector2f> targetPositions;

    // Setup for the purple strip
    sf::RectangleShape strip(sf::Vector2f(window.getSize().x, 50.f));
    strip.setFillColor(sf::Color(128, 0, 128, 80)); // Purple color
    strip.setPosition(0.f, 0.f);

    // live indicators in the top
    sf::Texture liveIconsTexture;
    if (!liveIconsTexture.loadFromFile("images/live-icon.png")) {
        std::cerr << "\nFailed to load icon for image lives.";
    }
    sf::Sprite liveIcon1(liveIconsTexture);
    liveIcon1.setScale({ 0.8f, 0.8f });
    liveIcon1.setPosition({ strip.getPosition().x + 120, 15.f});
    sf::Sprite liveIcon2(liveIconsTexture);
    liveIcon2.setScale({ 0.8f, 0.8f });
    liveIcon2.setPosition({ strip.getPosition().x + 150, 15.f});
    sf::Sprite liveIcon3(liveIconsTexture);
    liveIcon3.setScale({ 0.8f, 0.8f });
    liveIcon3.setPosition({ strip.getPosition().x + 180, 15.f});


    // Size and spacing for the invaders
    float invaderWidth = invaderTexture.getSize().x * 0.3f;
    float invaderHeight = invaderTexture.getSize().y * 0.3f;

    // Calculate target positions for invaders forming a hollow rectangle
    float startX = (window.getSize().x - (cols * (invaderWidth + spacing) - spacing)) / 2;
    float startY = (window.getSize().y - (rows * (invaderHeight + spacing) - spacing)) / 6;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Only place invaders on the perimeter
            if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1) {
                sf::Sprite invader(invaderTexture);
                invader.setScale(0.3f, 0.3f);

                // Start position for animation (off-screen)
                float x = -invaderWidth;
                float y = -invaderHeight;

                invader.setPosition(x, y);
                invaders.push_back(invader);

                float targetX = startX + j * (invaderWidth + spacing);
                float targetY = startY + i * (invaderHeight + spacing);
                targetPositions.push_back(sf::Vector2f(targetX, targetY));
            }
        }
    }

    // Animation variables
    float animationSpeed = 200.0f; // Speed at which invaders move
    sf::Clock clock;
    sf::Clock bulletClock;

    // Random number generation setup
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Bomb management
    std::vector<Bomb> bombs;
    std::vector<float> bombTimers(invaders.size());
    std::vector<float> nextBombTimes(invaders.size());
    std::vector<sf::Vector2f> bombVelocities;

    // Initialize timers for each invader
    for (size_t i = 0; i < invaders.size(); ++i) {
        bombTimers[i] = 0.0f;
        nextBombTimes[i] = static_cast<float>(std::rand() % 15 + 5); // Randomize initial bomb drop time between 5 and 20 seconds
    }

    // Bullet management
    std::vector<Bullet> playerBullets;
    bool isFiringBullets = false;
    float fireRate = 0.2f; // Rate of fire in seconds

    // Score management
    int score = 0;
    sf::Font ScoreFont;
    if (!ScoreFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text scoreText;
    scoreText.setFont(ScoreFont);
    scoreText.setString("Score: 0");
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineThickness(2);
    scoreText.setOutlineColor(sf::Color::Black);
    float windowWidth = window.getSize().x;
    scoreText.setPosition((windowWidth / 2.f) - 50.f, 10.f); // Centered

    

    // playe name font
    sf::Font playerNameFont;
    if (!playerNameFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }

    // player name display
    sf::Text playerNameText;
    playerNameText.setFont(playerNameFont);
    playerNameText.setString(player->getName());
    playerNameText.setCharacterSize(24);
    playerNameText.setFillColor(sf::Color::White);
    playerNameText.setOutlineThickness(2);
    playerNameText.setOutlineColor(sf::Color::Black);
    playerNameText.setPosition(10.f, 10.f);


    sf::Clock playerDestroyClock;
    playerDestroyClock.restart();
    

    // Game loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                player->saveToFile();
                window.close();
            }
                
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    paused = true;
                    displayPauseMenu(window);
                }
            }
        }

        // Calculate delta time for smooth animation
        float deltaTime = clock.restart().asSeconds();

        // Player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player->move("up", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player->move("down", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player->move("left", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player->move("right", deltaTime);
        }

        // Fire bullets
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            isFiringBullets = true;
        }



        if (isFiringBullets) {
            gameMusic->playFireSound();
        }
        else if (!isFiringBullets) {
            gameMusic->stopFireSound();
        }

        if(paused == false){

        // Update position of invaders
        for (size_t i = 0; i < invaders.size(); ++i) {
            sf::Vector2f position = invaders[i].getPosition();
            sf::Vector2f targetPosition = targetPositions[i];

            // Move invaders towards target positions
            if (position.x < targetPosition.x) {
                position.x += animationSpeed * deltaTime;
                if (position.x > targetPosition.x) position.x = targetPosition.x;
            }
            else if (position.x > targetPosition.x) {
                position.x -= animationSpeed * deltaTime;
                if (position.x < targetPosition.x) position.x = targetPosition.x;
            }

            if (position.y < targetPosition.y) {
                position.y += animationSpeed * deltaTime;
                if (position.y > targetPosition.y) position.y = targetPosition.y;
            }
            else if (position.y > targetPosition.y) {
                position.y -= animationSpeed * deltaTime;
                if (position.y < targetPosition.y) position.y = targetPosition.y;
            }

            invaders[i].setPosition(position);

            // Update bomb timer for each invader
            bombTimers[i] += deltaTime;
            if (bombTimers[i] >= nextBombTimes[i]) {
                // Drop a bomb
                Bomb bomb(50, nullptr, 100.0f); // Assuming a fixed damage and speed for bombs
                bomb.getSprite().setPosition(invaders[i].getPosition());
                bombs.push_back(bomb);
                bombVelocities.push_back(sf::Vector2f(0.0f, bomb.getSpeed())); // Bomb drops downwards

                // Reset timer and randomize next bomb drop time
                bombTimers[i] = 0.0f;
                nextBombTimes[i] = static_cast<float>(std::rand() % 15 + 5);
            }
        }

        // Update bomb positions
        for (size_t i = 0; i < bombs.size(); ++i) {
            sf::Vector2f bombPosition = bombs[i].getSprite().getPosition();
            bombPosition += bombVelocities[i] * deltaTime;
            bombs[i].getSprite().setPosition(bombPosition);
        }

        
        // Adding bullets to bullet container / vector
        if (isFiringBullets && bulletClock.getElapsedTime().asSeconds() > fireRate) {
            Bullet newBullet(5.f);
            newBullet.setPosition(sf::Vector2f((player->getPosition().x + player->getGlobalBounds().width / 2.0f) - 15.0f, player->getPosition().y));
            playerBullets.push_back(newBullet);
            isFiringBullets = false;

            // Restart the bullet clock
            bulletClock.restart();
        }

        // Firing the bullets
        for (auto& bullet : playerBullets) {
            bullet.fire(2.5f);
        }

        // Pop back the bullets which have gone offscreen to save memory and performance
        for (auto it = playerBullets.begin(); it != playerBullets.end();) {
            if (it->getPosition().y < 0) {
                it = playerBullets.erase(it);
            }
            else {
                ++it;
            }
        }

        // Check collision between player bullets and invaders
        for (auto it = playerBullets.begin(); it != playerBullets.end();) {
            bool hit = false;
            for (auto it_invader = invaders.begin(); it_invader != invaders.end();) {
                if (it_invader->getGlobalBounds().intersects(it->getGlobalBounds())) {
                    it_invader = invaders.erase(it_invader); // Remove invader
                    it = playerBullets.erase(it); // Remove bullet
                    score += 1 * 10; // Increment score on bullet hit
                    scoreText.setString("Score: " + std::to_string(score));
                    hit = true;
                    break;
                }
                else {
                    ++it_invader;
                }
            }
            if (!hit) {
                ++it;
            }
        }

        // check collision between bombs and player
        for (auto& bomb : bombs) {
            if (player->checkCollision(bomb.getSprite())) {
                player->takeDamage();
                bomb.getSprite().setScale({ 0,0 });
                //player->setScale({ 0,0 });
                //playerDestroyClock.restart();
                //std::cout << "\nCollided and destroyed."<<endl;
            }
        }

        for (auto& invader : invaders) {
            if (player->checkCollision(invader)) {
                player->takeDamage();
                //invader.setScale({ 0,0 });
                displayGameOverMenu(window);
                //player->setScale({ 0,0 });
                //playerDestroyClock.restart();
                //std::cout << "\nCollided and destroyed."<<endl;
            }
        }

        //std::cout << playerDestroyClock.getElapsedTime().asSeconds()<<endl;

        // update the player
        player->updateScore(score);

        // off screen player movement tracking and correction

        if ((player->getPosition().x + player->getGlobalBounds().width) < 0) {
            player->setPosition(sf::Vector2f(window.getSize().x, player->getPosition().y));
        }
        if (player->getPosition().x > window.getSize().x) {
            player->setPosition(sf::Vector2f(0, player->getPosition().y));
        }



        if (player->getLives() < 1) {
            gameMusic->stopGameplayMusic();
            gameMusic->playDestroyedSound();
            // set game over logic
        }

        player->destroyAnimation();


        if (player->getLives() < 3) {
            liveIcon3.setScale(0, 0);
        }
        if (player->getLives() < 2) {
            liveIcon2.setScale(0, 0);
        }
        if (player->getLives() < 1) {
            liveIcon1.setScale(0, 0);
        }

        // also implement redraw of the icons if implemented ad-ons (life)
        
        // gameover
        if (player->getLives() < 1) {
            displayGameOverMenu(window);
        }

        // Render
        window.clear();
        // check condition for next level

        if (invaders.empty()) {
            // All invaders are killed, load next level
            //loadLevel2(betaTexture, 10.f, 5.f, window, player);// Call function to load next level
            player->saveToFile();
            loadL1phase2(betaTexture, 5, 8, window, player, gameMusic);
            return; // Exit the current level loop
        }


        // draw the background

        window.draw(backgroundSprite);


        // Render the invaders
        for (const auto& invader : invaders) {
            window.draw(invader);
        }

        // Render the bullets
        for (auto& bullet : playerBullets) {
            bullet.draw(window);
        }

        // Render the bombs
        for (auto& bomb : bombs) {
            bomb.render(window);
        }

        }

        // Render player and UI elements
        
        
        player->draw(window);
        window.draw(strip);
        window.draw(liveIcon1);
        window.draw(liveIcon2);
        window.draw(liveIcon3);
        window.draw(scoreText);
        window.draw(playerNameText);
        window.display();
    }
}


void Game::loadL1phase2(sf::Texture& betaTexture, int height, float spacing, sf::RenderWindow& window, Player* player, GameMusic* gameMusic) {
    
    //hiding the mouse cursor
    window.setMouseCursorVisible(false);
    
    // Load the background texture and sprite
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background_1.png")) {
        std::cerr << "Failed to load Background Texture." << std::endl;
        return;
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    backgroundSprite.setScale({ 2.f, 1.5f });

    //beta texure
    sf::Texture gammaTexture;
    sf::Sprite gammaSprite;

    if (!gammaTexture.loadFromFile("images/alpha_invader.png")) {
        std::cerr << "\nFailed to load beta texture.";
    }

    gammaSprite.setTexture(gammaTexture);

    // Load the live icons texture
    sf::Texture liveIconsTexture;
    if (!liveIconsTexture.loadFromFile("images/live-icon.png")) {
        std::cerr << "Failed to load icon for image lives." << std::endl;
    }
    sf::Sprite liveIcon1(liveIconsTexture);
    liveIcon1.setScale({ 0.8f, 0.8f });
    liveIcon1.setPosition({ 120.f, 15.f });
    sf::Sprite liveIcon2(liveIconsTexture);
    liveIcon2.setScale({ 0.8f, 0.8f });
    liveIcon2.setPosition({ 150.f, 15.f });
    sf::Sprite liveIcon3(liveIconsTexture);
    liveIcon3.setScale({ 0.8f, 0.8f });
    liveIcon3.setPosition({ 180.f, 15.f });

    // Load score font and text
    sf::Font scoreFont;
    if (!scoreFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text scoreText;
    scoreText.setFont(scoreFont);
    scoreText.setString("Score: 0");
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineThickness(2);
    scoreText.setOutlineColor(sf::Color::Black);
    scoreText.setPosition((window.getSize().x / 2.f) - 50.f, 10.f);

    // Load player name font and text
    sf::Font playerNameFont;
    if (!playerNameFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text playerNameText;
    playerNameText.setFont(playerNameFont);
    playerNameText.setString(player->getName());
    playerNameText.setCharacterSize(24);
    playerNameText.setFillColor(sf::Color::White);
    playerNameText.setOutlineThickness(2);
    playerNameText.setOutlineColor(sf::Color::Black);
    playerNameText.setPosition(10.f, 10.f);

    // Create the invaders
    std::vector<sf::Sprite> invaders;
    std::vector<sf::Vector2f> targetPositions;

    float invaderWidth = betaTexture.getSize().x * 0.4f;
    float invaderHeight = betaTexture.getSize().y * 0.4f;

    float startX = (window.getSize().x - (spacing * (invaderWidth + spacing) - spacing)) / 2;
    float startY = (window.getSize().y - (height * (invaderHeight + height) - height)) / 8;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < spacing; ++j) {
            sf::Sprite invader(betaTexture);
            invader.setScale(0.4f, 0.4f);

            float x = startX + j * (invaderWidth + spacing);
            float y = startY + i * (invaderHeight + height);

            invader.setPosition(x, y);
            invaders.push_back(invader);
            targetPositions.push_back(sf::Vector2f(x, y));
        }
    }

    // Animation variables
    float animationSpeed = 200.0f;
    sf::Clock clock;
    sf::Clock bulletClock;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Bomb management
    std::vector<Bomb> bombs;
    std::vector<float> bombTimers(invaders.size());
    std::vector<float> nextBombTimes(invaders.size());
    std::vector<sf::Vector2f> bombVelocities;

    for (size_t i = 0; i < invaders.size(); ++i) {
        bombTimers[i] = 0.0f;
        nextBombTimes[i] = static_cast<float>(std::rand() % 15 + 5);
    }

    // Bullet management
    std::vector<Bullet> playerBullets;
    bool isFiringBullets = false;
    float fireRate = 0.2f;

    // Score management
    int score = 0;

    sf::Clock playerDestroyClock;
    playerDestroyClock.restart();

    // Game loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                player->saveToFile();
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    paused = true;
                    displayPauseMenu(window);
                }
            }
        }

        // Calculate delta time
        float deltaTime = clock.restart().asSeconds();

        // Player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player->move("up", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player->move("down", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player->move("left", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player->move("right", deltaTime);
        }

        // Fire bullets
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            isFiringBullets = true;
        }

        if (isFiringBullets) {
            gameMusic->playFireSound();
        }
        else {
            gameMusic->stopFireSound();
        }
         
        if (paused == false) {

            // Update invader positions
            for (size_t i = 0; i < invaders.size(); ++i) {
                sf::Vector2f position = invaders[i].getPosition();
                sf::Vector2f targetPosition = targetPositions[i];

                if (position.x < targetPosition.x) {
                    position.x += animationSpeed * deltaTime;
                    if (position.x > targetPosition.x) position.x = targetPosition.x;
                }
                else if (position.x > targetPosition.x) {
                    position.x -= animationSpeed * deltaTime;
                    if (position.x < targetPosition.x) position.x = targetPosition.x;
                }

                if (position.y < targetPosition.y) {
                    position.y += animationSpeed * deltaTime;
                    if (position.y > targetPosition.y) position.y = targetPosition.y;
                }
                else if (position.y > targetPosition.y) {
                    position.y -= animationSpeed * deltaTime;
                    if (position.y < targetPosition.y) position.y = targetPosition.y;
                }

                invaders[i].setPosition(position);

                bombTimers[i] += deltaTime;
                if (bombTimers[i] >= nextBombTimes[i]) {
                    Bomb bomb(50, nullptr, 100.0f);
                    bomb.getSprite().setPosition(invaders[i].getPosition());
                    bombs.push_back(bomb);
                    bombVelocities.push_back(sf::Vector2f(0.0f, bomb.getSpeed()));

                    bombTimers[i] = 0.0f;
                    nextBombTimes[i] = static_cast<float>(std::rand() % 15 + 5);
                }
            }

            // Update bomb positions
            for (size_t i = 0; i < bombs.size(); ++i) {
                sf::Vector2f bombPosition = bombs[i].getSprite().getPosition();
                bombPosition += bombVelocities[i] * deltaTime;
                bombs[i].getSprite().setPosition(bombPosition);
            }

            // Add bullets to bullet container
            if (isFiringBullets && bulletClock.getElapsedTime().asSeconds() > fireRate) {
                Bullet newBullet(5.f);
                newBullet.setPosition(sf::Vector2f((player->getPosition().x + player->getGlobalBounds().width / 2.0f) - 15.0f, player->getPosition().y));
                playerBullets.push_back(newBullet);
                isFiringBullets = false;

                bulletClock.restart();
            }

            // Fire the bullets
            for (auto& bullet : playerBullets) {
                bullet.fire(2.5f);
            }

            // Remove offscreen bullets
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                if (it->getPosition().y < 0) {
                    it = playerBullets.erase(it);
                }
                else {
                    ++it;
                }
            }

            // Check collision between player bullets and invaders
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                bool hit = false;
                for (auto it_invader = invaders.begin(); it_invader != invaders.end();) {
                    if (it_invader->getGlobalBounds().intersects(it->getGlobalBounds())) {
                        it_invader = invaders.erase(it_invader);
                        it = playerBullets.erase(it);
                        score += 1 * 10;
                        scoreText.setString("Score: " + std::to_string(score));
                        hit = true;
                        break;
                    }
                    else {
                        ++it_invader;
                    }
                }
                if (!hit) {
                    ++it;
                }
            }

            // Check collision between bombs and player
            for (auto& bomb : bombs) {
                if (player->checkCollision(bomb.getSprite())) {
                    player->takeDamage();
                    bomb.getSprite().setScale({ 0,0 });
                }
            }

            // Check collision between player and invaders
            for (auto& invader : invaders) {
                if (player->checkCollision(invader)) {
                    player->takeDamage();
                    //invader.setScale({ 0,0 });
                    displayGameOverMenu(window);
                }
            }

            // Update player
            player->updateScore(score);

            // off screen player movement tracking and correction

            if ((player->getPosition().x + player->getGlobalBounds().width) < 0) {
                player->setPosition(sf::Vector2f(window.getSize().x, player->getPosition().y));
            }
            if (player->getPosition().x > window.getSize().x) {
                player->setPosition(sf::Vector2f(0, player->getPosition().y));
            }

            // Update live icons based on player's lives
            if (player->getLives() < 3) {
                liveIcon3.setScale(0, 0);
            }
            if (player->getLives() < 2) {
                liveIcon2.setScale(0, 0);
            }
            if (player->getLives() < 1) {
                liveIcon1.setScale(0, 0);
            }

            // gameover
            if (player->getLives() < 1) {
                displayGameOverMenu(window);
            }

            // Render
            window.clear();

            // Check condition for next level
            if (invaders.empty()) {
                player->saveToFile();
                loadL1phase3(gammaTexture, 3, 10.0f, window, player, gameMusic);
                return;
            }

            window.draw(backgroundSprite);

            for (const auto& invader : invaders) {
                window.draw(invader);
            }

            for (auto& bullet : playerBullets) {
                bullet.draw(window);
            }

            for (auto& bomb : bombs) {
                bomb.render(window);
            }

        }

        player->draw(window);
        window.draw(liveIcon1);
        window.draw(liveIcon2);
        window.draw(liveIcon3);
        window.draw(scoreText);
        window.draw(playerNameText);
        window.display();
    }
}


void Game::loadL1phase3(sf::Texture& invaderTexture, int armLength, float spacing, sf::RenderWindow& window, Player* player, GameMusic* gameMusic) {
    
    //hiding the mouse cursor
    window.setMouseCursorVisible(false);

    // Load the background texture and sprite
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background_1.png")) {
        std::cerr << "Failed to load Background Texture." << std::endl;
        return;
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    backgroundSprite.setScale({ 2.f, 1.5f });

    //alpha texure
    sf::Texture alphaTexture;
    sf::Sprite alphaSprite;

    if (!alphaTexture.loadFromFile("images/beta_invader.png")) {
        std::cerr << "\nFailed to load beta texture.";
    }

    alphaSprite.setTexture(alphaTexture);
    
    // Create a vector to hold the invaders and their target positions
    std::vector<sf::Sprite> invaders;
    std::vector<sf::Vector2f> targetPositions;
   

    // Size and spacing for the invaders
    float invaderWidth = invaderTexture.getSize().x * 0.4f;
    float invaderHeight = invaderTexture.getSize().y * 0.4f;

    // Calculate target positions for invaders forming an 'X' cross sign
    float centerX = window.getSize().x / 2;
    float centerY = window.getSize().y / 4.5;

    // Diagonal lines
    for (int i = -armLength; i <= armLength; ++i) {
        sf::Sprite invader1(invaderTexture);
        invader1.setScale(0.4f, 0.4f);
        sf::Sprite invader2(invaderTexture);
        invader2.setScale(0.4f, 0.4f);

        // Start position for animation (off-screen)
        float x1 = -invaderWidth;
        float y1 = -invaderHeight;
        float x2 = window.getSize().x;
        float y2 = -invaderHeight;

        invader1.setPosition(x1, y1);
        invaders.push_back(invader1);

        invader2.setPosition(x2, y2);
        invaders.push_back(invader2);
    }

    // Target positions for the 'X' cross sign centered in the window
    for (int i = -armLength; i <= armLength; ++i) {
        // Diagonal line from top-left to bottom-right
        float x1 = centerX + i * (invaderWidth + spacing);
        float y1 = centerY + i * (invaderHeight + spacing);
        targetPositions.push_back(sf::Vector2f(x1, y1));

        // Diagonal line from top-right to bottom-left
        float x2 = centerX + i * (invaderWidth + spacing);
        float y2 = centerY - i * (invaderHeight + spacing);
        targetPositions.push_back(sf::Vector2f(x2, y2));
    }

    // Animation variables
    float animationSpeed = 200.0f; // Speed at which invaders move
    sf::Clock clock;
    sf::Clock bulletClock;

    // Random number generation setup
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Bomb management
    std::vector<Bomb> bombs;
    std::vector<float> bombTimers(invaders.size());
    std::vector<float> nextBombTimes(invaders.size());
    std::vector<sf::Vector2f> bombVelocities;

    // Initialize timers for each invader
    for (size_t i = 0; i < invaders.size(); ++i) {
        bombTimers[i] = 0.0f;
        nextBombTimes[i] = static_cast<float>(std::rand() % 15 + 2); // Randomize initial bomb drop time between 2 and 16 seconds
    }

    // Bullet management
    std::vector<Bullet> playerBullets;
    bool isFiringBullets = false;
    float fireRate = 0.2f; // Rate of fire in seconds

    // Score management
    int score = player->getScore();
    sf::Font ScoreFont;
    if (!ScoreFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text scoreText;
    scoreText.setFont(ScoreFont);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineThickness(2);
    scoreText.setOutlineColor(sf::Color::Black);
    float windowWidth = window.getSize().x;
    scoreText.setPosition((windowWidth / 2.f) - 50.f, 10.f); // Centered

    // Player name display
    sf::Font playerNameFont;
    if (!playerNameFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text playerNameText;
    playerNameText.setFont(playerNameFont);
    playerNameText.setString(player->getName());
    playerNameText.setCharacterSize(24);
    playerNameText.setFillColor(sf::Color::White);
    playerNameText.setOutlineThickness(2);
    playerNameText.setOutlineColor(sf::Color::Black);
    playerNameText.setPosition(10.f, 10.f);

    // Lives indicators
    sf::Texture liveIconsTexture;
    if (!liveIconsTexture.loadFromFile("images/live-icon.png")) {
        std::cerr << "\nFailed to load icon for image lives.";
    }
    sf::Sprite liveIcon1(liveIconsTexture);
    liveIcon1.setScale({ 0.8f, 0.8f });
    liveIcon1.setPosition({ 10.f, 45.f });
    sf::Sprite liveIcon2(liveIconsTexture);
    liveIcon2.setScale({ 0.8f, 0.8f });
    liveIcon2.setPosition({ 40.f, 45.f });
    sf::Sprite liveIcon3(liveIconsTexture);
    liveIcon3.setScale({ 0.8f, 0.8f });
    liveIcon3.setPosition({ 70.f, 45.f });

    // Game loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                player->saveToFile();
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    paused = true;
                    displayPauseMenu(window);
                }
            }
        }

        // Calculate delta time for smooth animation
        float deltaTime = clock.restart().asSeconds();

        // Player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player->move("up", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player->move("down", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player->move("left", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player->move("right", deltaTime);
        }

        // Fire bullets
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            isFiringBullets = true;
        }

        if (isFiringBullets) {
            gameMusic->playFireSound();
        }
        else if (!isFiringBullets) {
            gameMusic->stopFireSound();
        }


        if (paused == false) {

            // Update position of invaders
            for (size_t i = 0; i < invaders.size(); ++i) {
                sf::Vector2f position = invaders[i].getPosition();
                sf::Vector2f targetPosition = targetPositions[i];

                // Move invaders towards target positions
                if (position.x < targetPosition.x) {
                    position.x += animationSpeed * deltaTime;
                    if (position.x > targetPosition.x) position.x = targetPosition.x;
                }
                else if (position.x > targetPosition.x) {
                    position.x -= animationSpeed * deltaTime;
                    if (position.x < targetPosition.x) position.x = targetPosition.x;
                }

                if (position.y < targetPosition.y) {
                    position.y += animationSpeed * deltaTime;
                    if (position.y > targetPosition.y) position.y = targetPosition.y;
                }
                else if (position.y > targetPosition.y) {
                    position.y -= animationSpeed * deltaTime;
                    if (position.y < targetPosition.y) position.y = targetPosition.y;
                }

                invaders[i].setPosition(position);

                // Update bomb timer for each invader
                bombTimers[i] += deltaTime;
                if (bombTimers[i] >= nextBombTimes[i]) {
                    // Drop a bomb
                    Bomb bomb(50, nullptr, 100.0f); // Assuming a fixed damage and speed for bombs
                    bomb.getSprite().setPosition(invaders[i].getPosition());
                    bombs.push_back(bomb);
                    bombVelocities.push_back(sf::Vector2f(0.4f, bomb.getSpeed())); // Bomb drops downwards

                    // Reset timer and randomize next bomb drop time
                    bombTimers[i] = 0.0f;
                    nextBombTimes[i] = static_cast<float>(std::rand() % 15 + 2);
                }
            }

            // Update bomb positions
            for (size_t i = 0; i < bombs.size(); ++i) {
                sf::Vector2f bombPosition = bombs[i].getSprite().getPosition();
                bombPosition += bombVelocities[i] * deltaTime;
                bombs[i].getSprite().setPosition(bombPosition);
            }

            // Add bullets to bullet container
            if (isFiringBullets && bulletClock.getElapsedTime().asSeconds() > fireRate) {
                Bullet newBullet(5.f);
                newBullet.setPosition(sf::Vector2f((player->getPosition().x + player->getGlobalBounds().width / 2.0f) - 15.0f, player->getPosition().y));
                playerBullets.push_back(newBullet);
                isFiringBullets = false;

                bulletClock.restart();
            }

            // Fire the bullets
            for (auto& bullet : playerBullets) {
                bullet.fire(2.5f);
            }

            // Remove offscreen bullets
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                if (it->getPosition().y < 0) {
                    it = playerBullets.erase(it);
                }
                else {
                    ++it;
                }
            }

            // Check collision between player bullets and invaders
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                bool hit = false;
                for (auto it_invader = invaders.begin(); it_invader != invaders.end();) {
                    if (it_invader->getGlobalBounds().intersects(it->getGlobalBounds())) {
                        it_invader = invaders.erase(it_invader);
                        it = playerBullets.erase(it);
                        score += 1 * 10;
                        scoreText.setString("Score: " + std::to_string(score));
                        hit = true;
                        break;
                    }
                    else {
                        ++it_invader;
                    }
                }
                if (!hit) {
                    ++it;
                }
            }

            // Check collision between bombs and player
            for (auto& bomb : bombs) {
                if (player->checkCollision(bomb.getSprite())) {
                    player->takeDamage();
                    bomb.getSprite().setScale({ 0,0 });
                }
            }

            // Check collision between player and invaders
            // Check collision between player and invaders
            for (auto& invader : invaders) {
                if (player->checkCollision(invader)) {
                    player->takeDamage();
                    //invader.setScale({ 0,0 });
                    displayGameOverMenu(window);
                }
            }

            // Update player
            player->updateScore(score);

            // off screen player movement tracking and correction

            if ((player->getPosition().x + player->getGlobalBounds().width) < 0) {
                player->setPosition(sf::Vector2f(window.getSize().x, player->getPosition().y));
            }
            if (player->getPosition().x > window.getSize().x) {
                player->setPosition(sf::Vector2f(0, player->getPosition().y));
            }

            // Update live icons based on player's lives
            if (player->getLives() < 3) {
                liveIcon3.setScale(0, 0);
            }
            if (player->getLives() < 2) {
                liveIcon2.setScale(0, 0);
            }
            if (player->getLives() < 1) {
                liveIcon1.setScale(0, 0);
            }

            // gameover
            if (player->getLives() < 1) {
                displayGameOverMenu(window);
            }

            // Render
            window.clear();

            // Check condition for next level
            if (invaders.empty()) {
                player->saveToFile();
                loadL2phase1(alphaTexture, 20, 150.0f, window, player, gameMusic);
                return;
            }

            window.draw(backgroundSprite);

            for (const auto& invader : invaders) {
                window.draw(invader);
            }

            for (auto& bullet : playerBullets) {
                bullet.draw(window);
            }

            for (auto& bomb : bombs) {
                bomb.render(window);
            }

        }

        player->draw(window);
        window.draw(liveIcon1);
        window.draw(liveIcon2);
        window.draw(liveIcon3);
        window.draw(scoreText);
        window.draw(playerNameText);
        window.display();
    }
}


void Game::loadL2phase1(sf::Texture& invaderTexture, int numInvaders, float radius, sf::RenderWindow& window, Player* player, GameMusic* gameMusic) {
    
    player->updateLevelProgress(2);

    //hiding the mouse cursor
    window.setMouseCursorVisible(false);


    // Load the background texture and sprite
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background_2.jpg")) {
        std::cerr << "Failed to load Background Texture." << std::endl;
        return;
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    backgroundSprite.setScale({ 2.f, 1.5f });

    //alpha texure
    sf::Texture alphaTexture;
    sf::Sprite alphaSprite;

    if (!alphaTexture.loadFromFile("images/beta_invader.png")) {
        std::cerr << "\nFailed to load alphatexture.";
    }

    alphaSprite.setTexture(alphaTexture);
    // Size and spacing for the invaders
    float invaderWidth = invaderTexture.getSize().x * 0.4f;
    float invaderHeight = invaderTexture.getSize().y * 0.4f;

    // Create a vector to hold the invaders and their target positions
    std::vector<sf::Sprite> invaders;
    std::vector<sf::Vector2f> targetPositions;

    float angleIncrement = 2 * 3.14f / numInvaders; // Angle between each invader
    
    // Calculate target positions for invaders forming a hollow circle
    for (int i = 0; i < numInvaders; ++i) {
        sf::Sprite invader(invaderTexture);
        invader.setScale(0.3f, 0.3f);

        // Calculate position along the circle
        float angle = i * angleIncrement;
        float x = window.getSize().x / 2 + radius * std::cos(angle);
        float y = window.getSize().y / 3 + radius * std::sin(angle);
        targetPositions.push_back(sf::Vector2f(x - invaderWidth / 2, y - invaderHeight / 2));

        // Start position (off-screen to the left)
        invader.setPosition(-invaderWidth, y - invaderHeight / 5);
        invaders.push_back(invader);
    }

    sf::Clock clock;
    sf::Clock bulletClock;

    // Random number generation setup
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    

    // Bomb management
    std::vector<Bomb> bombs;
    std::vector<float> bombTimers(invaders.size());
    std::vector<float> nextBombTimes(invaders.size());
    std::vector<sf::Vector2f> bombVelocities;

    // Initialize timers for each invader
    for (size_t i = 0; i < invaders.size(); ++i) {
        bombTimers[i] = 0.0f;
        nextBombTimes[i] = static_cast<float>(std::rand() % 8 + 2); // Randomize initial bomb drop time between 2 and 16 seconds
    }
    // Bullet management
    std::vector<Bullet> playerBullets;
    bool isFiringBullets = false;
    float fireRate = 0.2f; // Rate of fire in seconds

    // Score management
    int score = player->getScore();
    sf::Font ScoreFont;
    if (!ScoreFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text scoreText;
    scoreText.setFont(ScoreFont);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineThickness(2);
    scoreText.setOutlineColor(sf::Color::Black);
    float windowWidth = window.getSize().x;
    scoreText.setPosition((windowWidth / 2.f) - 50.f, 10.f); // Centered

    // Player name display
    sf::Font playerNameFont;
    if (!playerNameFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text playerNameText;
    playerNameText.setFont(playerNameFont);
    playerNameText.setString(player->getName());
    playerNameText.setCharacterSize(24);
    playerNameText.setFillColor(sf::Color::White);
    playerNameText.setOutlineThickness(2);
    playerNameText.setOutlineColor(sf::Color::Black);
    playerNameText.setPosition(10.f, 10.f);

    // Lives indicators
    sf::Texture liveIconsTexture;
    if (!liveIconsTexture.loadFromFile("images/live-icon.png")) {
        std::cerr << "\nFailed to load icon for image lives.";
    }
    sf::Sprite liveIcon1(liveIconsTexture);
    liveIcon1.setScale({ 0.8f, 0.8f });
    liveIcon1.setPosition({ 10.f, 45.f });
    sf::Sprite liveIcon2(liveIconsTexture);
    liveIcon2.setScale({ 0.8f, 0.8f });
    liveIcon2.setPosition({ 40.f, 45.f });
    sf::Sprite liveIcon3(liveIconsTexture);
    liveIcon3.setScale({ 0.8f, 0.8f });
    liveIcon3.setPosition({ 70.f, 45.f });

    // Game loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                player->saveToFile();
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    paused = true;
                    displayPauseMenu(window);
                }
            }
        }

        // Calculate delta time for smooth animation
        float deltaTime = clock.restart().asSeconds();

        // Player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player->move("up", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player->move("down", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player->move("left", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player->move("right", deltaTime);
        }

        // Fire bullets
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            isFiringBullets = true;
        }

        if (isFiringBullets) {
            gameMusic->playFireSound();
        }
        else if (!isFiringBullets) {
            gameMusic->stopFireSound();
        }


        
        float animationSpeed = 300.0f;


        if (paused == false) {

            // Update position of invaders
            for (size_t i = 0; i < invaders.size(); ++i) {
                sf::Vector2f position = invaders[i].getPosition();
                sf::Vector2f targetPosition = targetPositions[i];

                // Move invaders towards target positions
                if (position.x < targetPosition.x) {
                    position.x += animationSpeed * deltaTime;
                    if (position.x > targetPosition.x) position.x = targetPosition.x;
                }

                if (position.y < targetPosition.y) {
                    position.y += animationSpeed * deltaTime;
                    if (position.y > targetPosition.y) position.y = targetPosition.y;
                }
                else if (position.y > targetPosition.y) {
                    position.y -= animationSpeed * deltaTime;
                    if (position.y < targetPosition.y) position.y = targetPosition.y;
                }

                invaders[i].setPosition(position);

                // Update bomb timer for each invader
                bombTimers[i] += deltaTime;
                if (bombTimers[i] >= nextBombTimes[i]) {
                    // Drop a bomb
                    Bomb bomb(50, nullptr, 200.0f); // Assuming a fixed damage and speed for bombs
                    bomb.getSprite().setPosition(invaders[i].getPosition());
                    bombs.push_back(bomb);
                    bombVelocities.push_back(sf::Vector2f(0.0f, bomb.getSpeed())); // Bomb drops downwards

                    // Reset timer and randomize next bomb drop time
                    bombTimers[i] = 0.0f;
                    nextBombTimes[i] = static_cast<float>(std::rand() % 8 + 2);
                }
            }

            // Update bomb positions
            for (size_t i = 0; i < bombs.size(); ++i) {
                sf::Vector2f bombPosition = bombs[i].getSprite().getPosition();
                bombPosition += bombVelocities[i] * deltaTime;
                bombs[i].getSprite().setPosition(bombPosition);
            }

            // Add bullets to bullet container
            if (isFiringBullets && bulletClock.getElapsedTime().asSeconds() > fireRate) {
                Bullet newBullet(5.f);
                newBullet.setPosition(sf::Vector2f((player->getPosition().x + player->getGlobalBounds().width / 2.0f) - 15.0f, player->getPosition().y));
                playerBullets.push_back(newBullet);
                isFiringBullets = false;

                bulletClock.restart();
            }

            // Fire the bullets
            for (auto& bullet : playerBullets) {
                bullet.fire(2.5f);
            }

            // Remove offscreen bullets
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                if (it->getPosition().y < 0) {
                    it = playerBullets.erase(it);
                }
                else {
                    ++it;
                }
            }

            // Check collision between player bullets and invaders
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                bool hit = false;
                for (auto it_invader = invaders.begin(); it_invader != invaders.end();) {
                    if (it_invader->getGlobalBounds().intersects(it->getGlobalBounds())) {
                        it_invader = invaders.erase(it_invader);
                        it = playerBullets.erase(it);
                        score += 2 * 20;
                        scoreText.setString("Score: " + std::to_string(score));
                        hit = true;
                        break;
                    }
                    else {
                        ++it_invader;
                    }
                }
                if (!hit) {
                    ++it;
                }
            }

            // Check collision between bombs and player
            for (auto& bomb : bombs) {
                if (player->checkCollision(bomb.getSprite())) {
                    player->takeDamage();
                    bomb.getSprite().setScale({ 0,0 });
                }
            }

            // Check collision between player and invaders
            for (auto& invader : invaders) {
                if (player->checkCollision(invader)) {
                    player->takeDamage();
                    //invader.setScale({ 0,0 });
                    displayGameOverMenu(window);
                }
            }

            // Update player
            player->updateScore(score);

            // off screen player movement tracking and correction

            if ((player->getPosition().x + player->getGlobalBounds().width) < 0) {
                player->setPosition(sf::Vector2f(window.getSize().x, player->getPosition().y));
            }
            if (player->getPosition().x > window.getSize().x) {
                player->setPosition(sf::Vector2f(0, player->getPosition().y));
            }

            // Update live icons based on player's lives
            if (player->getLives() < 3) {
                liveIcon3.setScale(0, 0);
            }
            if (player->getLives() < 2) {
                liveIcon2.setScale(0, 0);
            }
            if (player->getLives() < 1) {
                liveIcon1.setScale(0, 0);
            }

            // gameover
            if (player->getLives() < 1) {
                displayGameOverMenu(window);
            }

            // Render
            window.clear();

            // Check condition for next level
            if (invaders.empty()) {
                player->saveToFile();
                loadL2phase2(betaTexture, 4, window, player, gameMusic);
                return;
            }

            window.draw(backgroundSprite);

            for (const auto& invader : invaders) {
                window.draw(invader);
            }

            for (auto& bullet : playerBullets) {
                bullet.draw(window);
            }

            for (auto& bomb : bombs) {
                bomb.render(window);
            }
        }

        player->draw(window);
        window.draw(liveIcon1);
        window.draw(liveIcon2);
        window.draw(liveIcon3);
        window.draw(scoreText);
        window.draw(playerNameText);
        window.display();
    }
}


void Game::loadL2phase2(sf::Texture& invaderTexture, int armLength, sf::RenderWindow& window, Player* player, GameMusic* gameMusic)
{

    //hiding the mouse cursor
    window.setMouseCursorVisible(false);
    
    // Load the background texture and sprite
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background_2.jpg")) {
        std::cerr << "Failed to load Background Texture." << std::endl;
        return;
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    backgroundSprite.setScale({ 2.f, 1.5 });

    //alpha texure
    sf::Texture alphaTexture;
    sf::Sprite alphaSprite;

    if (!alphaTexture.loadFromFile("images/gamma_invader.png")) {
        std::cerr << "\nFailed to load alphatexture.";
    }
    

    // Size and spacing for the invaders
    float invaderWidth = invaderTexture.getSize().x * 0.3f;
    float invaderHeight = invaderTexture.getSize().y * 0.3f;
    float spacing = 2.0f;


    // Create a vector to hold the invaders and their target positions
    std::vector<sf::Sprite> invaders;
    std::vector<sf::Vector2f> targetPositions;


    // Initial positions for the invaders (off-screen to the left)
    float startXLeft = -invaderWidth;
    float startYTop = window.getSize().y / 2 - (armLength * (invaderHeight + spacing) - spacing) / 4;


    // Calculate target positions for the invaders forming a hollow diamond
    float centerX = window.getSize().x / 2;
    float centerY = window.getSize().y / 4;
    for (int i = -armLength; i <= armLength; ++i) {
        for (int j = -armLength; j <= armLength; ++j) {
            // Condition to create a hollow diamond
            if (std::abs(i) + std::abs(j) == armLength) {
                sf::Sprite invader(invaderTexture);
                invader.setScale(0.3f, 0.3f);

                // Calculate target position for the diamond shape
                float x = centerX + j * (invaderWidth + spacing);
                float y = centerY + i * (invaderHeight + spacing);
                targetPositions.push_back(sf::Vector2f(x - invaderWidth / 2, y - invaderHeight / 2));

                // Start position (off-screen to the left)
                invader.setPosition(startXLeft, y - invaderHeight / 2);
                invaders.push_back(invader);
            }
        }
    }

    sf::Clock clock;
    sf::Clock bulletClock;


    // Random number generation setup
    std::srand(static_cast<unsigned int>(std::time(nullptr)));


    // Bomb management
    std::vector<Bomb> bombs;
    std::vector<float> bombTimers(invaders.size());
    std::vector<float> nextBombTimes(invaders.size());
    std::vector<sf::Vector2f> bombVelocities;


    // Initialize timers for each invader
    for (size_t i = 0; i < invaders.size(); ++i) {
        bombTimers[i] = 0.0f;
        nextBombTimes[i] = static_cast<float>(std::rand() % 8 + 2); // Randomize initial bomb drop time between 2 and 16 seconds
    }

    // Bullet management
    std::vector<Bullet> playerBullets;
    bool isFiringBullets = false;
    float fireRate = 0.2f; // Rate of fire in seconds

    // Score management
    int score = player->getScore();
    sf::Font ScoreFont;
    if (!ScoreFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text scoreText;
    scoreText.setFont(ScoreFont);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineThickness(2);
    scoreText.setOutlineColor(sf::Color::Black);
    float windowWidth = window.getSize().x;
    scoreText.setPosition((windowWidth / 2.f) - 50.f, 10.f); // Centered

    // Player name display
    sf::Font playerNameFont;
    if (!playerNameFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text playerNameText;
    playerNameText.setFont(playerNameFont);
    playerNameText.setString(player->getName());
    playerNameText.setCharacterSize(24);
    playerNameText.setFillColor(sf::Color::White);
    playerNameText.setOutlineThickness(2);
    playerNameText.setOutlineColor(sf::Color::Black);
    playerNameText.setPosition(10.f, 10.f);

    // Lives indicators
    sf::Texture liveIconsTexture;
    if (!liveIconsTexture.loadFromFile("images/live-icon.png")) {
        std::cerr << "\nFailed to load icon for image lives.";
    }
    sf::Sprite liveIcon1(liveIconsTexture);
    liveIcon1.setScale({ 0.8f, 0.8f });
    liveIcon1.setPosition({ 10.f, 45.f });
    sf::Sprite liveIcon2(liveIconsTexture);
    liveIcon2.setScale({ 0.8f, 0.8f });
    liveIcon2.setPosition({ 40.f, 45.f });
    sf::Sprite liveIcon3(liveIconsTexture);
    liveIcon3.setScale({ 0.8f, 0.8f });
    liveIcon3.setPosition({ 70.f, 45.f });

    // Game loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                player->saveToFile();
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    paused = true;
                    displayPauseMenu(window);
                }
            }
        }

        // Calculate delta time for smooth animation
        float deltaTime = clock.restart().asSeconds();

        // Player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player->move("up", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player->move("down", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player->move("left", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player->move("right", deltaTime);
        }

        // Fire bullets
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            isFiringBullets = true;
        }

        if (isFiringBullets) {
            gameMusic->playFireSound();
        }
        else if (!isFiringBullets) {
            gameMusic->stopFireSound();
        }



        float animationSpeed = 300.0f;

        if (paused == false) {


            // Update position of invaders
            for (size_t i = 0; i < invaders.size(); ++i) {
                sf::Vector2f position = invaders[i].getPosition();
                sf::Vector2f targetPosition = targetPositions[i];

                // Move invaders towards target positions
                if (position.x < targetPosition.x) {
                    position.x += animationSpeed * deltaTime;
                    if (position.x > targetPosition.x) position.x = targetPosition.x;
                }

                if (position.y < targetPosition.y) {
                    position.y += animationSpeed * deltaTime;
                    if (position.y > targetPosition.y) position.y = targetPosition.y;
                }
                else if (position.y > targetPosition.y) {
                    position.y -= animationSpeed * deltaTime;
                    if (position.y < targetPosition.y) position.y = targetPosition.y;
                }

                invaders[i].setPosition(position);

                // Update bomb timer for each invader
                bombTimers[i] += deltaTime;
                if (bombTimers[i] >= nextBombTimes[i]) {
                    // Drop a bomb
                    Bomb bomb(50, nullptr, 150.0f); // Assuming a fixed damage and speed for bombs
                    bomb.getSprite().setPosition(invaders[i].getPosition());
                    bombs.push_back(bomb);
                    bombVelocities.push_back(sf::Vector2f(0.0f, bomb.getSpeed())); // Bomb drops downwards

                    // Reset timer and randomize next bomb drop time
                    bombTimers[i] = 0.0f;
                    nextBombTimes[i] = static_cast<float>(std::rand() % 15 + 2);
                }
            }

            // Update bomb positions
            for (size_t i = 0; i < bombs.size(); ++i) {
                sf::Vector2f bombPosition = bombs[i].getSprite().getPosition();
                bombPosition += bombVelocities[i] * deltaTime;
                bombs[i].getSprite().setPosition(bombPosition);
            }


            // Add bullets to bullet container
            if (isFiringBullets && bulletClock.getElapsedTime().asSeconds() > fireRate) {
                Bullet newBullet(5.f);
                newBullet.setPosition(sf::Vector2f((player->getPosition().x + player->getGlobalBounds().width / 2.0f) - 15.0f, player->getPosition().y));
                playerBullets.push_back(newBullet);
                isFiringBullets = false;

                bulletClock.restart();
            }

            // Fire the bullets
            for (auto& bullet : playerBullets) {
                bullet.fire(2.5f);
            }

            // Remove offscreen bullets
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                if (it->getPosition().y < 0) {
                    it = playerBullets.erase(it);
                }
                else {
                    ++it;
                }
            }

            // Check collision between player bullets and invaders
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                bool hit = false;
                for (auto it_invader = invaders.begin(); it_invader != invaders.end();) {
                    if (it_invader->getGlobalBounds().intersects(it->getGlobalBounds())) {
                        it_invader = invaders.erase(it_invader);
                        it = playerBullets.erase(it);
                        score += 2 * 20;
                        scoreText.setString("Score: " + std::to_string(score));
                        hit = true;
                        break;
                    }
                    else {
                        ++it_invader;
                    }
                }
                if (!hit) {
                    ++it;
                }
            }

            // Check collision between bombs and player
            for (auto& bomb : bombs) {
                if (player->checkCollision(bomb.getSprite())) {
                    player->takeDamage();
                    bomb.getSprite().setScale({ 0,0 });
                }
            }

            // Check collision between player and invaders
            for (auto& invader : invaders) {
                if (player->checkCollision(invader)) {
                    player->takeDamage();
                    //invader.setScale({ 0,0 });
                    displayGameOverMenu(window);
                }
            }

            // Update player
            player->updateScore(score);

            // off screen player movement tracking and correction

            if ((player->getPosition().x + player->getGlobalBounds().width) < 0) {
                player->setPosition(sf::Vector2f(window.getSize().x, player->getPosition().y));
            }
            if (player->getPosition().x > window.getSize().x) {
                player->setPosition(sf::Vector2f(0, player->getPosition().y));
            }

            // Update live icons based on player's lives
            if (player->getLives() < 3) {
                liveIcon3.setScale(0, 0);
            }
            if (player->getLives() < 2) {
                liveIcon2.setScale(0, 0);
            }
            if (player->getLives() < 1) {
                liveIcon1.setScale(0, 0);
            }

            // gameover
            if (player->getLives() < 1) {
                displayGameOverMenu(window);
            }

            // Render
            window.clear();

            // Check condition for next level
            if (invaders.empty()) {
                player->saveToFile();
                std::cout << "\nInavders all destroyed.Good to move on to level3.";
                //loadL3phase1(alphaTexture, 6, window, player, gameMusic);
                addMonster(window);
                //return;
            }

            window.draw(backgroundSprite);

            for (const auto& invader : invaders) {
                window.draw(invader);
            }

            for (auto& bullet : playerBullets) {
                bullet.draw(window);
            }

            for (auto& bomb : bombs) {
                bomb.render(window);
            }

        }

        player->draw(window);
        window.draw(liveIcon1);
        window.draw(liveIcon2);
        window.draw(liveIcon3);
        window.draw(scoreText);
        window.draw(playerNameText);
        window.display();
    }
}


void Game::loadL3phase1(sf::Texture& invaderTexture, int height, sf::RenderWindow& window, Player* player, GameMusic* gameMusic) {

    //addDragon(window);

    player->updateLevelProgress(3);

    //hiding the mouse cursor
    window.setMouseCursorVisible(false);

    // Load the background texture and sprite
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background_3.jpg")) {
        std::cerr << "Failed to load Background Texture." << std::endl;
        return;
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    backgroundSprite.setScale({ 2.f, 1.5f });

    //beta texure
    sf::Texture gammaTexture;
    sf::Sprite gammaSprite;
    sf::Sprite betaSprite;

    if (!gammaTexture.loadFromFile("images/gamma_invader.png")) {
        std::cerr << "\nFailed to load beta texture.";
    }

    betaSprite.setTexture(gammaTexture);


    // Calculate the heart shape based on the provided logic
    std::vector<std::string> heartShape;
    for (int i = height / 2; i <= height; i += 2) {
        std::string line = "";
        for (int j = 1; j < height - i; j += 2)
            line += " ";
        for (int j = 1; j <= i; j++)
            line += "*";
        for (int j = 1; j <= height - i; j++)
            line += " ";
        for (int j = 1; j <= i; j++)
            line += "*";
        heartShape.push_back(line);
    }
    for (int i = height; i >= 1; i--) {
        std::string line = "";
        for (int j = i; j < height; j++)
            line += " ";
        for (int j = 1; j <= (i * 2) - 1; j++)
            line += "*";
        heartShape.push_back(line);
    }

    // Create a vector to hold the invaders and their target positions
    std::vector<sf::Sprite> invaders;
    std::vector<sf::Vector2f> targetPositions;

    // Render Beta Invaders within the heart shape
    float invaderSize = 35.0f; // Adjust size as needed
    float invaderSpacing = 12.0f; // Adjust spacing between invaders
    float startY = -invaderSize; // Start position off-screen at the top

    for (size_t i = 0; i < heartShape.size(); ++i) {
        for (size_t j = 0; j < heartShape[i].size(); ++j) {
            if (heartShape[i][j] == '*') {
                sf::Sprite invader(invaderTexture);
                invader.setScale(invaderSize / invader.getLocalBounds().width, invaderSize / invader.getLocalBounds().height);

                // Calculate target position for the heart shape
                float x = j * (invaderSize + invaderSpacing) + window.getSize().x / 6;
                float y = i * (invaderSize + invaderSpacing) + window.getSize().y / 7;
                targetPositions.push_back(sf::Vector2f(x, y));

                // Start position (off-screen at the top)
                invader.setPosition(x, startY);
                invaders.push_back(invader);
            }
        }
    }

    // Animation variables
    float animationSpeed = 300.0f; // Speed at which invaders move
    sf::Clock clock;
    sf::Clock bulletClock;

    // Random number generation setup
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Bomb management
    std::vector<Bomb> bombs;
    std::vector<float> bombTimers(invaders.size());
    std::vector<float> nextBombTimes(invaders.size());
    std::vector<sf::Vector2f> bombVelocities;

    // Initialize timers for each invader
    for (size_t i = 0; i < invaders.size(); ++i) {
        bombTimers[i] = 0.0f;
        nextBombTimes[i] = static_cast<float>(std::rand() % 30 + 15); // Randomize initial bomb drop time between 1 and 5 seconds
        if (nextBombTimes[i] < 8.f) {
            continue;
        }
    }
    // Bullet management
    std::vector<Bullet> playerBullets;
    bool isFiringBullets = false;
    float fireRate = 0.2f; // Rate of fire in seconds

    // Score management
    int score = player->getScore();
    sf::Font ScoreFont;
    if (!ScoreFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text scoreText;
    scoreText.setFont(ScoreFont);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineThickness(2);
    scoreText.setOutlineColor(sf::Color::Black);
    float windowWidth = window.getSize().x;
    scoreText.setPosition((windowWidth / 2.f) - 50.f, 10.f); // Centered

    // Player name display
    sf::Font playerNameFont;
    if (!playerNameFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text playerNameText;
    playerNameText.setFont(playerNameFont);
    playerNameText.setString(player->getName());
    playerNameText.setCharacterSize(24);
    playerNameText.setFillColor(sf::Color::White);
    playerNameText.setOutlineThickness(2);
    playerNameText.setOutlineColor(sf::Color::Black);
    playerNameText.setPosition(10.f, 10.f);

    // Lives indicators
    sf::Texture liveIconsTexture;
    if (!liveIconsTexture.loadFromFile("images/live-icon.png")) {
        std::cerr << "\nFailed to load icon for image lives.";
    }
    sf::Sprite liveIcon1(liveIconsTexture);
    liveIcon1.setScale({ 0.8f, 0.8f });
    liveIcon1.setPosition({ 10.f, 45.f });
    sf::Sprite liveIcon2(liveIconsTexture);
    liveIcon2.setScale({ 0.8f, 0.8f });
    liveIcon2.setPosition({ 40.f, 45.f });
    sf::Sprite liveIcon3(liveIconsTexture);
    liveIcon3.setScale({ 0.8f, 0.8f });
    liveIcon3.setPosition({ 70.f, 45.f });

    // Game loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                player->saveToFile();
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    paused = true;
                    displayPauseMenu(window);
                }
            }
        }

        // Calculate delta time for smooth animation
        float deltaTime = clock.restart().asSeconds();

        // Player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player->move("up", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player->move("down", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player->move("left", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player->move("right", deltaTime);
        }

        // Fire bullets
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            isFiringBullets = true;
        }

        if (isFiringBullets) {
            gameMusic->playFireSound();
        }
        else if (!isFiringBullets) {
            gameMusic->stopFireSound();
        }



        // Update position of invaders
        //for (size_t i = 0; i < invaders.size(); ++i) {
        //    sf::Vector2f position = invaders[i].getPosition();
        //    sf::Vector2f targetPosition = targetPositions[i];

        //    // Move invaders towards target positions
        //    if (position.y < targetPosition.y) {
        //        position.y += animationSpeed * deltaTime;
        //        if (position.y > targetPosition.y) position.y = targetPosition.y;
        //    }

        //    invaders[i].setPosition(position);

        if (paused == false) {

            // Update position of invaders 2
            for (size_t i = 0; i < invaders.size(); ++i) {
                sf::Vector2f position = invaders[i].getPosition();
                sf::Vector2f targetPosition = targetPositions[i];

                // Move invaders towards target positions
                if (position.x < targetPosition.x) {
                    position.x += animationSpeed * deltaTime;
                    if (position.x > targetPosition.x) position.x = targetPosition.x;
                }

                if (position.y < targetPosition.y) {
                    position.y += animationSpeed * deltaTime;
                    if (position.y > targetPosition.y) position.y = targetPosition.y;
                }
                else if (position.y > targetPosition.y) {
                    position.y -= animationSpeed * deltaTime;
                    if (position.y < targetPosition.y) position.y = targetPosition.y;
                }

                invaders[i].setPosition(position);

                // Update bomb timer for each invader
                bombTimers[i] += deltaTime;
                if (bombTimers[i] >= nextBombTimes[i]) {
                    // Drop a bomb
                    Bomb bomb(50, nullptr, 50.0f); // Assuming a fixed damage and speed for bombs
                    bomb.getSprite().setPosition(invaders[i].getPosition());
                    bombs.push_back(bomb);
                    bombVelocities.push_back(sf::Vector2f(0.0f, bomb.getSpeed())); // Bomb drops downwards

                    // Reset timer and randomize next bomb drop time
                    bombTimers[i] = 0.0f;
                    nextBombTimes[i] = static_cast<float>(std::rand() % 30 + 10);
                }
            }

            // Update bomb positions
            for (size_t i = 0; i < bombs.size(); ++i) {
                sf::Vector2f bombPosition = bombs[i].getSprite().getPosition();
                bombPosition += bombVelocities[i] * deltaTime;
                bombs[i].getSprite().setPosition(bombPosition);
            }

            // Add bullets to bullet container
            if (isFiringBullets && bulletClock.getElapsedTime().asSeconds() > fireRate) {
                Bullet newBullet(5.f);
                newBullet.setPosition(sf::Vector2f((player->getPosition().x + player->getGlobalBounds().width / 2.0f) - 15.0f, player->getPosition().y));
                playerBullets.push_back(newBullet);
                isFiringBullets = false;

                bulletClock.restart();
            }

            // Fire the bullets
            for (auto& bullet : playerBullets) {
                bullet.fire(2.5f);
            }

            // Remove offscreen bullets
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                if (it->getPosition().y < 0) {
                    it = playerBullets.erase(it);
                }
                else {
                    ++it;
                }
            }

            // Check collision between player bullets and invaders
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                bool hit = false;
                for (auto it_invader = invaders.begin(); it_invader != invaders.end();) {
                    if (it_invader->getGlobalBounds().intersects(it->getGlobalBounds())) {
                        it_invader = invaders.erase(it_invader);
                        it = playerBullets.erase(it);
                        score += 3 * 30;
                        scoreText.setString("Score: " + std::to_string(score));
                        hit = true;
                        break;
                    }
                    else {
                        ++it_invader;
                    }
                }
                if (!hit) {
                    ++it;
                }
            }

            // Check collision between bombs and player
            for (auto& bomb : bombs) {
                if (player->checkCollision(bomb.getSprite())) {
                    player->takeDamage();
                    bomb.getSprite().setScale({ 0,0 });
                }
            }

            // Check collision between player and invaders
            for (auto& invader : invaders) {
                if (player->checkCollision(invader)) {
                    player->takeDamage();
                    //invader.setScale({ 0,0 });
                    displayGameOverMenu(window);
                }
            }

            // Update player
            player->updateScore(score);

            // off screen player movement tracking and correction

            if ((player->getPosition().x + player->getGlobalBounds().width) < 0) {
                player->setPosition(sf::Vector2f(window.getSize().x, player->getPosition().y));
            }
            if (player->getPosition().x > window.getSize().x) {
                player->setPosition(sf::Vector2f(0, player->getPosition().y));
            }

            // Update live icons based on player's lives
            if (player->getLives() < 3) {
                liveIcon3.setScale(0, 0);
            }
            if (player->getLives() < 2) {
                liveIcon2.setScale(0, 0);
            }
            if (player->getLives() < 1) {
                liveIcon1.setScale(0, 0);
            }

            // gameover
            if (player->getLives() < 1) {
                displayGameOverMenu(window);
            }

            // Render
            window.clear();

            // Check condition for next level
            if (invaders.empty()) {
                player->saveToFile();
                loadL3phase2(gammaTexture, 10, 20, window, player, gameMusic);
                return;
            }

            window.draw(backgroundSprite);

            for (const auto& invader : invaders) {
                window.draw(invader);
            }

            for (auto& bullet : playerBullets) {
                bullet.draw(window);
            }

            for (auto& bomb : bombs) {
                bomb.render(window);
            }

        }

        player->draw(window);
        window.draw(liveIcon1);
        window.draw(liveIcon2);
        window.draw(liveIcon3);
        window.draw(scoreText);
        window.draw(playerNameText);
        window.display();
    }
}


void Game::loadL3phase2(sf::Texture& invaderTexture, int rows, int cols, sf::RenderWindow& window, Player* player, GameMusic* gameMusic) {

    //hiding the mouse cursor
    window.setMouseCursorVisible(false);

    // Load the background texture and sprite
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background_3.jpg")) {
        std::cerr << "Failed to load Background Texture." << std::endl;
        return;
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    backgroundSprite.setScale({ 2.f, 1.5f });

    //gamma texure
    sf::Texture gammaTexture;
    sf::Sprite gammaSprite;

    if (!gammaTexture.loadFromFile("images/gamma_invader.png")) {
        std::cerr << "\nFailed to load gamma texture.";
    }

    gammaSprite.setTexture(gammaTexture);

    // Calculate the size of each cell in the circle shape grid
    float cellWidth = static_cast<float>(window.getSize().x) / (cols + 1); // Increase the divisor to decrease the gap
    float cellHeight = static_cast<float>(window.getSize().y) / (rows + 1); // Increase the divisor to decrease the gap

    // Create vectors to hold the invaders and their target positions
    std::vector<sf::Sprite> invaders;
    std::vector<sf::Vector2f> targetPositions;

    // Create invaders in the circle shape grid
    float centerX = window.getSize().x / 2;
    float centerY = window.getSize().y / 4;
    float radius = 200.0f;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float x = j * cellWidth;
            float y = i * cellHeight;

            // Check if the current cell is within the circle shape
            float dx = x - centerX;
            float dy = y - centerY;
            if (dx * dx + dy * dy <= radius * radius) {
                sf::Sprite invader(invaderTexture);
                invader.setOrigin(invader.getLocalBounds().width / 2, invader.getLocalBounds().height / 2);

                // Calculate target position
                sf::Vector2f targetPos(x + cellWidth / 2, y + cellHeight / 2);
                targetPositions.push_back(targetPos);

                // Initialize position off-screen
                if (x < centerX) {
                    invader.setPosition(-invader.getLocalBounds().width, y + cellHeight / 2);
                }
                else {
                    invader.setPosition(window.getSize().x + invader.getLocalBounds().width, y + cellHeight / 2);
                }

                float scaleFactor = std::min(cellWidth / invader.getLocalBounds().width, cellHeight / invader.getLocalBounds().height) * 1.f; // Adjust this value to make invaders smaller
                invader.setScale(scaleFactor, scaleFactor);
                invaders.push_back(invader);
            }
        }
    }

    // Animation variables
    float animationSpeed = 300.0f; // Speed at which invaders move
    sf::Clock clock;
    sf::Clock bulletClock;

    // Random number generation setup
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Bomb management
    std::vector<Bomb> bombs;
    std::vector<float> bombTimers(invaders.size());
    std::vector<float> nextBombTimes(invaders.size());
    std::vector<sf::Vector2f> bombVelocities;

    // Initialize timers for each invader
    for (size_t i = 0; i < invaders.size(); ++i) {
        bombTimers[i] = 0.0f;
        nextBombTimes[i] = static_cast<float>(std::rand() % 20 + 10); // Randomize initial bomb drop time between 1 and 5 seconds
    }
    // Bullet management
    std::vector<Bullet> playerBullets;
    bool isFiringBullets = false;
    float fireRate = 0.2f; // Rate of fire in seconds

    // Score management
    int score = player->getScore();
    sf::Font ScoreFont;
    if (!ScoreFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text scoreText;
    scoreText.setFont(ScoreFont);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineThickness(2);
    scoreText.setOutlineColor(sf::Color::Black);
    float windowWidth = window.getSize().x;
    scoreText.setPosition((windowWidth / 2.f) - 50.f, 10.f); // Centered

    // Player name display
    sf::Font playerNameFont;
    if (!playerNameFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text playerNameText;
    playerNameText.setFont(playerNameFont);
    playerNameText.setString(player->getName());
    playerNameText.setCharacterSize(24);
    playerNameText.setFillColor(sf::Color::White);
    playerNameText.setOutlineThickness(2);
    playerNameText.setOutlineColor(sf::Color::Black);
    playerNameText.setPosition(10.f, 10.f);

    // Lives indicators
    sf::Texture liveIconsTexture;
    if (!liveIconsTexture.loadFromFile("images/live-icon.png")) {
        std::cerr << "\nFailed to load icon for image lives.";
    }
    sf::Sprite liveIcon1(liveIconsTexture);
    liveIcon1.setScale({ 0.8f, 0.8f });
    liveIcon1.setPosition({ 10.f, 45.f });
    sf::Sprite liveIcon2(liveIconsTexture);
    liveIcon2.setScale({ 0.8f, 0.8f });
    liveIcon2.setPosition({ 40.f, 45.f });
    sf::Sprite liveIcon3(liveIconsTexture);
    liveIcon3.setScale({ 0.8f, 0.8f });
    liveIcon3.setPosition({ 70.f, 45.f });

    // Game loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                player->saveToFile();
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    paused = true;
                    displayPauseMenu(window);
                }
            }
        }

        // Calculate delta time for smooth animation
        float deltaTime = clock.restart().asSeconds();

        // Player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player->move("up", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player->move("down", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player->move("left", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player->move("right", deltaTime);
        }

        // Fire bullets
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            isFiringBullets = true;
        }

        if (isFiringBullets) {
            gameMusic->playFireSound();
        }
        else if (!isFiringBullets) {
            gameMusic->stopFireSound();
        }


        if (paused == false) {

            // Update position of invaders and drop bombs
            for (size_t i = 0; i < invaders.size(); ++i) {
                sf::Vector2f position = invaders[i].getPosition();
                sf::Vector2f targetPosition = targetPositions[i];

                // Move invaders towards target positions
                if (position.x < targetPosition.x) {
                    position.x += animationSpeed * deltaTime;
                    if (position.x > targetPosition.x) position.x = targetPosition.x;
                }
                else if (position.x > targetPosition.x) {
                    position.x -= animationSpeed * deltaTime;
                    if (position.x < targetPosition.x) position.x = targetPosition.x;
                }

                invaders[i].setPosition(position);

                // Update bomb timer for each invader
                bombTimers[i] += deltaTime;
                if (bombTimers[i] >= nextBombTimes[i]) {
                    // Drop a bomb
                    Bomb bomb(50, nullptr, 60.0f); // Assuming a fixed damage and speed for bombs
                    bomb.getSprite().setPosition(invaders[i].getPosition());
                    bombs.push_back(bomb);
                    bombVelocities.push_back(sf::Vector2f(0.0f, bomb.getSpeed())); // Bomb drops downwards

                    // Reset timer and randomize next bomb drop time
                    bombTimers[i] = 0.0f;
                    nextBombTimes[i] = static_cast<float>(std::rand() % 20 + 10);
                }
            }

            // Update bomb positions
            for (size_t i = 0; i < bombs.size(); ++i) {
                sf::Vector2f bombPosition = bombs[i].getSprite().getPosition();
                bombPosition += bombVelocities[i] * deltaTime;
                bombs[i].getSprite().setPosition(bombPosition);
            }

            // Add bullets to bullet container
            if (isFiringBullets && bulletClock.getElapsedTime().asSeconds() > fireRate) {
                Bullet newBullet(5.f);
                newBullet.setPosition(sf::Vector2f((player->getPosition().x + player->getGlobalBounds().width / 2.0f) - 15.0f, player->getPosition().y));
                playerBullets.push_back(newBullet);
                isFiringBullets = false;

                bulletClock.restart();
            }

            // Fire the bullets
            for (auto& bullet : playerBullets) {
                bullet.fire(2.5f);
            }

            // Remove offscreen bullets
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                if (it->getPosition().y < 0) {
                    it = playerBullets.erase(it);
                }
                else {
                    ++it;
                }
            }

            // Check collision between player bullets and invaders
            for (auto it = playerBullets.begin(); it != playerBullets.end();) {
                bool hit = false;
                for (auto it_invader = invaders.begin(); it_invader != invaders.end();) {
                    if (it_invader->getGlobalBounds().intersects(it->getGlobalBounds())) {
                        it_invader = invaders.erase(it_invader);
                        it = playerBullets.erase(it);
                        score += 3 * 30;
                        scoreText.setString("Score: " + std::to_string(score));
                        hit = true;
                        break;
                    }
                    else {
                        ++it_invader;
                    }
                }
                if (!hit) {
                    ++it;
                }
            }

            // Check collision between bombs and player
            for (auto& bomb : bombs) {
                if (player->checkCollision(bomb.getSprite())) {
                    player->takeDamage();
                    bomb.getSprite().setScale({ 0,0 });
                }
            }

            // Check collision between player and invaders
            for (auto& invader : invaders) {
                if (player->checkCollision(invader)) {
                    player->takeDamage();
                    //invader.setScale({ 0,0 });
                    
                }
            }

            // Update player
            player->updateScore(score);

            // off screen player movement tracking and correction

            if ((player->getPosition().x + player->getGlobalBounds().width) < 0) {
                player->setPosition(sf::Vector2f(window.getSize().x, player->getPosition().y));
            }
            if (player->getPosition().x > window.getSize().x) {
                player->setPosition(sf::Vector2f(0, player->getPosition().y));
            }

            // Update live icons based on player's lives
            if (player->getLives() < 3) {
                liveIcon3.setScale(0, 0);
            }
            if (player->getLives() < 2) {
                liveIcon2.setScale(0, 0);
            }
            if (player->getLives() < 1) {
                liveIcon1.setScale(0, 0);
            }

            // gameover
            if (player->getLives() < 1) {
                displayGameOverMenu(window);
            }

            // Render
            window.clear();

            // Check condition for next level
            if (invaders.empty()) {
                player->saveToFile();
                //displayGameEndMenu(window);
                addDragon(window);
                //return;
            }

            window.draw(backgroundSprite);

            for (const auto& invader : invaders) {
                window.draw(invader);
            }

            for (auto& bullet : playerBullets) {
                bullet.draw(window);
            }

            for (auto& bomb : bombs) {
                bomb.render(window);
            }

        }

        player->draw(window);
        window.draw(liveIcon1);
        window.draw(liveIcon2);
        window.draw(liveIcon3);
        window.draw(scoreText);
        window.draw(playerNameText);
        window.display();
    }
}

// monster
void Game::addMonster(sf::RenderWindow& window) {

    gameMusic->playDangerMusic();

    // Load the background texture and sprite
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background_3.jpg")) {
        std::cerr << "Failed to load Background Texture." << std::endl;
        return;
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    backgroundSprite.setScale({ 2.f, 1.5f });



    // monster
    Monster monster;
    monster.loadTexture("images/monster.png");
    monster.setPosition(-200, 50);

    // player, bullet and UI 
    sf::Clock bulletClock;
    sf::Clock clock;
    sf::Clock levelClock;
    // Bullet management
    std::vector<Bullet> playerBullets;
    bool isFiringBullets = false;
    float fireRate = 0.2f; // Rate of fire in seconds

    // Score management
    int score = player->getScore();
    sf::Font ScoreFont;
    if (!ScoreFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text scoreText;
    scoreText.setFont(ScoreFont);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineThickness(2);
    scoreText.setOutlineColor(sf::Color::Black);
    float windowWidth = window.getSize().x;
    scoreText.setPosition((windowWidth / 2.f) - 50.f, 10.f); // Centered

    // Player name display
    sf::Font playerNameFont;
    if (!playerNameFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text playerNameText;
    playerNameText.setFont(playerNameFont);
    playerNameText.setString(player->getName());
    playerNameText.setCharacterSize(24);
    playerNameText.setFillColor(sf::Color::White);
    playerNameText.setOutlineThickness(2);
    playerNameText.setOutlineColor(sf::Color::Black);
    playerNameText.setPosition(10.f, 10.f);

    // Lives indicators
    sf::Texture liveIconsTexture;
    if (!liveIconsTexture.loadFromFile("images/live-icon.png")) {
        std::cerr << "\nFailed to load icon for image lives.";
    }
    sf::Sprite liveIcon1(liveIconsTexture);
    liveIcon1.setScale({ 0.8f, 0.8f });
    liveIcon1.setPosition({ 10.f, 45.f });
    sf::Sprite liveIcon2(liveIconsTexture);
    liveIcon2.setScale({ 0.8f, 0.8f });
    liveIcon2.setPosition({ 40.f, 45.f });
    sf::Sprite liveIcon3(liveIconsTexture);
    liveIcon3.setScale({ 0.8f, 0.8f });
    liveIcon3.setPosition({ 70.f, 45.f });

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                player->saveToFile();
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    paused = true;
                    gameMusic->stopDangerMusic();
                    displayPauseMenu(window);
                }
            }
        }

        // Calculate delta time for smooth animation
        float deltaTime = clock.restart().asSeconds();

        // Player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player->move("up", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player->move("down", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player->move("left", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player->move("right", deltaTime);
        }

        // Fire bullets
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            isFiringBullets = true;
        }

        if (isFiringBullets) {
            gameMusic->playFireSound();
        }
        else if (!isFiringBullets) {
            gameMusic->stopFireSound();
        }


        // Add bullets to bullet container
        if (isFiringBullets && bulletClock.getElapsedTime().asSeconds() > fireRate) {
            Bullet newBullet(5.f);
            newBullet.setPosition(sf::Vector2f((player->getPosition().x + player->getGlobalBounds().width / 2.0f) - 15.0f, player->getPosition().y));
            playerBullets.push_back(newBullet);
            isFiringBullets = false;

            bulletClock.restart();
        }

        // Fire the bullets
        for (auto& bullet : playerBullets) {
            bullet.fire(2.5f);
        }

        // Remove offscreen bullets
        for (auto it = playerBullets.begin(); it != playerBullets.end();) {
            if (it->getPosition().y < 0) {
                it = playerBullets.erase(it);
            }
            else {
                ++it;
            }
        }

        // Update player
        player->updateScore(score);

        // off screen player movement tracking and correction

        if ((player->getPosition().x + player->getGlobalBounds().width) < 0) {
            player->setPosition(sf::Vector2f(window.getSize().x, player->getPosition().y));
        }
        if (player->getPosition().x > window.getSize().x) {
            player->setPosition(sf::Vector2f(0, player->getPosition().y));
        }

        // Update live icons based on player's lives
        if (player->getLives() < 3) {
            liveIcon3.setScale(0, 0);
        }
        if (player->getLives() < 2) {
            liveIcon2.setScale(0, 0);
        }
        if (player->getLives() < 1) {
            liveIcon1.setScale(0, 0);
        }

        // gameover
        if (player->getLives() < 1) {
            displayGameOverMenu(window);
        }




        monster.update();

        if (levelClock.getElapsedTime().asSeconds() >= 20) {
            gameMusic->stopDangerMusic();
            loadL3phase1(alphaTexture, 6, window, player, gameMusic);
            levelClock.restart();
        }

        if (monster.checkCollision(player->getSprite())) {
            displayGameOverMenu(window);
        }


        score += 0.3;
        player->updateScore(score);

        window.clear();
        window.draw(backgroundSprite);

        for (auto& bullet : playerBullets) {
            bullet.draw(window);
        }

        player->draw(window);
        window.draw(liveIcon1);
        window.draw(liveIcon2);
        window.draw(liveIcon3);
        window.draw(scoreText);
        window.draw(playerNameText);

        monster.render(window);
        window.display();
    }
}

// dragon
void Game::addDragon(sf::RenderWindow& window) {

    gameMusic->playDangerMusic();
    // Load the background texture and sprite
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background_3.jpg")) {
        std::cerr << "Failed to load Background Texture." << std::endl;
        return;
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    backgroundSprite.setScale({ 2.f, 1.5f });


    //dragon
    Dragon dragon;
    dragon.loadTexture("images/dragon.png");
    dragon.setPosition(300, -150); // Start above the window

    float speed = 0.03f; // Speed at which the dragon moves down

    // player, bullet and UI 
    sf::Clock bulletClock;
    sf::Clock clock;
    sf::Clock levelClock;
    // Bullet management
    std::vector<Bullet> playerBullets;
    bool isFiringBullets = false;
    float fireRate = 0.2f; // Rate of fire in seconds

    // Score management
    int score = player->getScore();
    sf::Font ScoreFont;
    if (!ScoreFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text scoreText;
    scoreText.setFont(ScoreFont);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineThickness(2);
    scoreText.setOutlineColor(sf::Color::Black);
    float windowWidth = window.getSize().x;
    scoreText.setPosition((windowWidth / 2.f) - 50.f, 10.f); // Centered

    // Player name display
    sf::Font playerNameFont;
    if (!playerNameFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }
    sf::Text playerNameText;
    playerNameText.setFont(playerNameFont);
    playerNameText.setString(player->getName());
    playerNameText.setCharacterSize(24);
    playerNameText.setFillColor(sf::Color::White);
    playerNameText.setOutlineThickness(2);
    playerNameText.setOutlineColor(sf::Color::Black);
    playerNameText.setPosition(10.f, 10.f);

    // Lives indicators
    sf::Texture liveIconsTexture;
    if (!liveIconsTexture.loadFromFile("images/live-icon.png")) {
        std::cerr << "\nFailed to load icon for image lives.";
    }
    sf::Sprite liveIcon1(liveIconsTexture);
    liveIcon1.setScale({ 0.8f, 0.8f });
    liveIcon1.setPosition({ 10.f, 45.f });
    sf::Sprite liveIcon2(liveIconsTexture);
    liveIcon2.setScale({ 0.8f, 0.8f });
    liveIcon2.setPosition({ 40.f, 45.f });
    sf::Sprite liveIcon3(liveIconsTexture);
    liveIcon3.setScale({ 0.8f, 0.8f });
    liveIcon3.setPosition({ 70.f, 45.f });

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                player->saveToFile();
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    paused = true;
                    gameMusic->stopDangerMusic();
                    displayPauseMenu(window);
                }
            }
        }

        // Calculate delta time for smooth animation
        float deltaTime = clock.restart().asSeconds();

        // Player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player->move("up", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player->move("down", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player->move("left", deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player->move("right", deltaTime);
        }

        // Fire bullets
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            isFiringBullets = true;
        }

        if (isFiringBullets) {
            gameMusic->playFireSound();
        }
        else if (!isFiringBullets) {
            gameMusic->stopFireSound();
        }


        // Add bullets to bullet container
        if (isFiringBullets && bulletClock.getElapsedTime().asSeconds() > fireRate) {
            Bullet newBullet(5.f);
            newBullet.setPosition(sf::Vector2f((player->getPosition().x + player->getGlobalBounds().width / 2.0f) - 15.0f, player->getPosition().y));
            playerBullets.push_back(newBullet);
            isFiringBullets = false;

            bulletClock.restart();
        }

        // Fire the bullets
        for (auto& bullet : playerBullets) {
            bullet.fire(2.5f);
        }

        // Remove offscreen bullets
        for (auto it = playerBullets.begin(); it != playerBullets.end();) {
            if (it->getPosition().y < 0) {
                it = playerBullets.erase(it);
            }
            else {
                ++it;
            }
        }

        score += 0.5;

        // Update player
        player->updateScore(score);

        // off screen player movement tracking and correction

        if ((player->getPosition().x + player->getGlobalBounds().width) < 0) {
            player->setPosition(sf::Vector2f(window.getSize().x, player->getPosition().y));
        }
        if (player->getPosition().x > window.getSize().x) {
            player->setPosition(sf::Vector2f(0, player->getPosition().y));
        }

        // Update live icons based on player's lives
        if (player->getLives() < 3) {
            liveIcon3.setScale(0, 0);
        }
        if (player->getLives() < 2) {
            liveIcon2.setScale(0, 0);
        }
        if (player->getLives() < 1) {
            liveIcon1.setScale(0, 0);
        }

        // gameover
        if (player->getLives() < 1) {
            displayGameOverMenu(window);
        }




        // Move the dragon downwards
        dragon.move(0, speed);

        // If the dragon reaches the bottom of the window, reset its position to the top
        if (dragon.getPosition().y > window.getSize().y) {
            dragon.setPosition(300, -dragon.getHeight());
        }

        dragon.update(); // Update the dragon's animation

        if (dragon.checkCollision(player->getSprite())) {
            displayGameOverMenu(window);
        }

        if (levelClock.getElapsedTime().asSeconds() >= 20) {
            gameMusic->stopDangerMusic();
            displayGameEndMenu(window);
        }

        window.clear();
        window.draw(backgroundSprite);

        for (auto& bullet : playerBullets) {
            bullet.draw(window);
        }

        player->draw(window);
        window.draw(liveIcon1);
        window.draw(liveIcon2);
        window.draw(liveIcon3);
        window.draw(scoreText);
        window.draw(playerNameText);
        dragon.render(window);
        window.display();
    }
}

// pause menu
void Game::displayPauseMenu(sf::RenderWindow& window) {

    // show the hidden mouse cursor
    window.setMouseCursorVisible(true);

    gameMusic->stopDangerMusic();

    // settting the overlay
    sf::RectangleShape pauseMenuOverlay;
    pauseMenuOverlay.setSize({ 250, 250 });
    pauseMenuOverlay.setPosition({ window.getSize().y / 2 - pauseMenuOverlay.getSize().y / 2, window.getSize().x / 2 - pauseMenuOverlay.getSize().x / 2 });
    pauseMenuOverlay.setFillColor(sf::Color(165, 155, 232, 167));
    pauseMenuOverlay.setOutlineThickness(15);
    pauseMenuOverlay.setOutlineColor(sf::Color(0, 0, 0, 50));


    // graphics for the resume button
    sf::Texture resumeButtonTexture;
    sf::Sprite resumeButtonSprite;

    if (!resumeButtonTexture.loadFromFile("images/buttons/resume.png")) {
        std::cerr << "\nFailed to load resume buttton texture.";
    }

    resumeButtonSprite.setTexture(resumeButtonTexture);
    resumeButtonSprite.setPosition(sf::Vector2f(pauseMenuOverlay.getPosition().x + (pauseMenuOverlay.getSize().x / 2.f) - (resumeButtonSprite.getGlobalBounds().width / 2.f), pauseMenuOverlay.getPosition().y + 50));

    // graphics for the exit button
    sf::Texture exitButtonTexture;
    sf::Sprite exitButtonSprite;

    if (!exitButtonTexture.loadFromFile("images/buttons/exit.png")) {
        std::cerr << "\nFailed to load exit buttton texture.";
    }

    exitButtonSprite.setTexture(exitButtonTexture);
    exitButtonSprite.setPosition(sf::Vector2f(pauseMenuOverlay.getPosition().x + (pauseMenuOverlay.getSize().x / 2.f) - (exitButtonSprite.getGlobalBounds().width / 2.f), pauseMenuOverlay.getPosition().y + 150));


    while (paused == true)
    {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    paused = false;
                    return;
                }
            }
            // Add handling for button clicks here
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (resumeButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    gameMusic->playClickSound();
                    paused = false;
                    return;
                }
                else if (exitButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    gameMusic->playClickSound();
                    player->saveToFile();
                    gameMenu->displayMainMenu(window);
                }
            }
        }

        // updates above this line
        window.draw(pauseMenuOverlay);
        window.draw(resumeButtonSprite);
        window.draw(exitButtonSprite);
        window.display();
    }

}


// game over menu
void Game::displayGameOverMenu(sf::RenderWindow& window) {

    // show the hidden mouse cursor
    window.setMouseCursorVisible(true);

    gameMusic->stopDangerMusic();

    gameMusic->playGameOverMusic();

    // settting the overlay
    sf::RectangleShape pauseMenuOverlay;
    pauseMenuOverlay.setSize({ 400, 340 });
    pauseMenuOverlay.setPosition({ window.getSize().y / 2 - pauseMenuOverlay.getSize().y / 2, window.getSize().x / 2 - pauseMenuOverlay.getSize().x / 2 });
    pauseMenuOverlay.setFillColor(sf::Color(165, 155, 232, 167));
    pauseMenuOverlay.setOutlineThickness(15);
    pauseMenuOverlay.setOutlineColor(sf::Color(0, 0, 0, 50));


    // game over heading text

    sf::Font gameOverFont;
    if (!gameOverFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "\nFailed to load font from the file.";
    }
    sf::Text gameOverHeading;
    gameOverHeading.setFont(gameOverFont);
    gameOverHeading.setString("Game Over");
    gameOverHeading.setCharacterSize(35);
    gameOverHeading.setOutlineThickness(3.f);
    gameOverHeading.setOutlineColor(sf::Color::White);
    gameOverHeading.setFillColor(sf::Color::Red);
    gameOverHeading.setPosition(sf::Vector2f(pauseMenuOverlay.getPosition().x + 100, pauseMenuOverlay.getPosition().y + 50));


    // score text
    sf::Text scoreText;
    scoreText.setFont(gameOverFont);
    scoreText.setString("Score: " + std::to_string(player->getScore()));
    scoreText.setCharacterSize(25);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(sf::Vector2f(pauseMenuOverlay.getPosition().x + 100, pauseMenuOverlay.getPosition().y + 110));


    // graphics for the resume button
    sf::Texture resumeButtonTexture;
    sf::Sprite resumeButtonSprite;

    if (!resumeButtonTexture.loadFromFile("images/buttons/restart.png")) {
        std::cerr << "\nFailed to load resume buttton texture.";
    }

    resumeButtonSprite.setTexture(resumeButtonTexture);
    resumeButtonSprite.setPosition(sf::Vector2f(pauseMenuOverlay.getPosition().x  + 20, pauseMenuOverlay.getPosition().y + 200));

    // graphics for the exit button
    sf::Texture exitButtonTexture;
    sf::Sprite exitButtonSprite;

    if (!exitButtonTexture.loadFromFile("images/buttons/exit.png")) {
        std::cerr << "\nFailed to load exit buttton texture.";
    }

    exitButtonSprite.setTexture(exitButtonTexture);
    exitButtonSprite.setPosition(sf::Vector2f(resumeButtonSprite.getPosition().x + resumeButtonSprite.getGlobalBounds().width + 20, pauseMenuOverlay.getPosition().y + 200));


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Add handling for button clicks here
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (resumeButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    gameMusic->stopGameOverMusic();
                    startGame(window);
                }
                else if (exitButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    gameMusic->playClickSound();
                    player->saveToFile();
                    gameMusic->stopGameOverMusic();
                    gameMenu->displayMainMenu(window);
                }
            }
        }

        // updates above this line
        window.draw(pauseMenuOverlay);
        window.draw(gameOverHeading);
        window.draw(scoreText);
        window.draw(resumeButtonSprite);
        window.draw(exitButtonSprite);
        window.display();
    }

}

// game end menu
void Game::displayGameEndMenu(sf::RenderWindow& window) {

    // show the hidden mouse cursor
    window.setMouseCursorVisible(true);

    gameMusic->stopDangerMusic();
    gameMusic->playGameCompleteMusic();

    // settting the overlay
    sf::RectangleShape pauseMenuOverlay;
    pauseMenuOverlay.setSize({ 400, 340 });
    pauseMenuOverlay.setPosition({ window.getSize().y / 2 - pauseMenuOverlay.getSize().y / 2, window.getSize().x / 2 - pauseMenuOverlay.getSize().x / 2 });
    pauseMenuOverlay.setFillColor(sf::Color(165, 155, 232, 167));
    pauseMenuOverlay.setOutlineThickness(15);
    pauseMenuOverlay.setOutlineColor(sf::Color(0, 0, 0, 50));


    // game over heading text

    sf::Font gameOverFont;
    if (!gameOverFont.loadFromFile("fonts/myFont.ttf")) {
        std::cerr << "\nFailed to load font from the file.";
    }
    sf::Text gameOverHeading;
    gameOverHeading.setFont(gameOverFont);
    gameOverHeading.setString("YOU WIN !");
    gameOverHeading.setCharacterSize(35);
    gameOverHeading.setFillColor(sf::Color::Green);
    gameOverHeading.setOutlineThickness(3.f);
    gameOverHeading.setOutlineColor(sf::Color::White);
    gameOverHeading.setPosition(sf::Vector2f(pauseMenuOverlay.getPosition().x + 100, pauseMenuOverlay.getPosition().y + 50));


    // score text
    sf::Text scoreText;
    scoreText.setFont(gameOverFont);
    scoreText.setString("Score: " + std::to_string(player->getScore()));
    scoreText.setCharacterSize(25);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(sf::Vector2f(pauseMenuOverlay.getPosition().x + 100, pauseMenuOverlay.getPosition().y + 110));


    // graphics for the resume button
    sf::Texture resumeButtonTexture;
    sf::Sprite resumeButtonSprite;

    if (!resumeButtonTexture.loadFromFile("images/buttons/restart.png")) {
        std::cerr << "\nFailed to load resume buttton texture.";
    }

    resumeButtonSprite.setTexture(resumeButtonTexture);
    resumeButtonSprite.setPosition(sf::Vector2f(pauseMenuOverlay.getPosition().x + 20, pauseMenuOverlay.getPosition().y + 200));

    // graphics for the exit button
    sf::Texture exitButtonTexture;
    sf::Sprite exitButtonSprite;

    if (!exitButtonTexture.loadFromFile("images/buttons/exit.png")) {
        std::cerr << "\nFailed to load exit buttton texture.";
    }

    exitButtonSprite.setTexture(exitButtonTexture);
    exitButtonSprite.setPosition(sf::Vector2f(resumeButtonSprite.getPosition().x + resumeButtonSprite.getGlobalBounds().width + 20, pauseMenuOverlay.getPosition().y + 200));


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Add handling for button clicks here
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (resumeButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    gameMusic->stopGameCompleteMusic();
                    startGame(window);
                }
                else if (exitButtonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    gameMusic->playClickSound();
                    player->saveToFile();
                    gameMusic->stopGameCompleteMusic();
                    gameMenu->displayMainMenu(window);
                }
            }
        }

        // updates above this line
        window.draw(pauseMenuOverlay);
        window.draw(gameOverHeading);
        window.draw(scoreText);
        window.draw(resumeButtonSprite);
        window.draw(exitButtonSprite);
        window.display();
    }

}