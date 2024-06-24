
#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include "player.h"
#include "Menu.h"
#include "GameMusic.h"
#include "Bullet.h"

using namespace std;

//const char title[] = "OOP-Project, Spring-2024";


class Game
{
public:
    sf::Sprite background; //Game background sprite
    sf::Texture bg_texture;
    sf::Image gameIcon;
    Player* player; //player 
    Menu* gameMenu;
    GameMusic* gameMusic;
    Bullet* playerBullet;
    bool paused = false;
    sf::Texture alphaTexture;
    sf::Texture betaTexture;
    sf::Texture gammaTexture;
    

    // add other game attributes

    Game(sf::RenderWindow& window)
    {
        
        gameMenu = new Menu(window, this);
        
        // create new player with the name entered in the createNewPlayer menu
        //player = new Player(gameMenu->getEnteredName(), sf::Vector2f(0.5f, 0.5f)); 
        

        // loading game icon

        if (!gameIcon.loadFromFile("images/player_ship.png")) {
            std::cerr << "\nFailed to load game icon image.";
        }

        // set icon for the window

        window.setIcon(gameIcon.getSize().x, gameIcon.getSize().y, gameIcon.getPixelsPtr());

        // game music object
        gameMusic = new GameMusic();

        // bullet initialization

        playerBullet = new Bullet(5.f);

        //gameMusic->playBgMusic();
        
        if (!bg_texture.loadFromFile("images/background_2.jpg")) {
            std::cerr << "Failed to load background.";
        }

        // loading textures for different enemies
        if (!alphaTexture.loadFromFile("images/alpha_invader.png")) {
            std::cerr << "\nFailed to load alpha invader texture.";
        }
        if (!betaTexture.loadFromFile("images/beta_invader.png")) {
            std::cerr << "\nFailed to load alpha invader texture.";
        }
        if (!gammaTexture.loadFromFile("images/gamma_invader.png")) {
            std::cerr << "\nFailed to load alpha invader texture.";
        }



        background.setTexture(bg_texture);
        background.setScale(1, 1);

        gameMenu->displayMainMenu(window);

    }
    
    void startGame(sf::RenderWindow&);
    void loadL1phase1(sf::Texture& invaderTexture, int rows, int cols, float spacing, sf::RenderWindow& window);
    void loadL1phase2(sf::Texture& invaderTexture, int height, float spacing, sf::RenderWindow& window, Player* player, GameMusic* gameMusic);
    void loadL1phase3(sf::Texture& invaderTexture, int armLength, float spacing, sf::RenderWindow& window, Player* player, GameMusic* gameMusic);
    void loadL2phase1(sf::Texture& invaderTexture, int numInvaders, float radius, sf::RenderWindow& window, Player* player, GameMusic* gameMusic);
    void loadL2phase2(sf::Texture& invaderTexture, int armLength, sf::RenderWindow& window, Player* player, GameMusic* gameMusic);
    void loadL3phase1(sf::Texture& invaderTexture, int height, sf::RenderWindow& window, Player* player, GameMusic* gameMusic);
    void loadL3phase2(sf::Texture& invaderTexture, int rows, int cols, sf::RenderWindow& window, Player* player, GameMusic* gameMusic);
    void addMonster(sf::RenderWindow&);
    void addDragon(sf::RenderWindow&);
    void displayPauseMenu(sf::RenderWindow&);
    void displayGameOverMenu(sf::RenderWindow&);
    void displayGameEndMenu(sf::RenderWindow&);

    

    ~Game() {
        delete player;
        delete gameMenu;
        delete gameMusic;
        gameMusic = NULL;
        player = NULL;
        gameMenu = NULL;
       
    }

    


};

