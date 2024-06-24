#pragma once

#include<SFML/Audio.hpp>
#include<iostream>



class GameMusic {
private:
    sf::SoundBuffer clickBuffer;
    sf::Sound clickSound;
    sf::SoundBuffer fireBuffer;
    sf::Sound fireSound;
    sf::Music destroyedSound;
    sf::Music bgMusic;
    sf::Music gamePlayMusic;
    sf::Music dangerMusic;
    sf::Music gameOverMusic;
    sf::Music gameCompleteMusic;
public:

   

    GameMusic() {

        // loading background music

        if (!bgMusic.openFromFile("music/bg-music.ogg")) {
             std::cerr<< "\nFailed to load music.";
        }
        //settings
        bgMusic.setLoop(true);
        bgMusic.setVolume(40);


        // loading gameplay music

        if (!gamePlayMusic.openFromFile("music/gameplay-music.ogg")) {
            std::cerr << "\nFailed to load music.";
        }
        //settings
        gamePlayMusic.setLoop(true);
        gamePlayMusic.setVolume(40);

        // loading destroyed music
        if (!destroyedSound.openFromFile("music/destroyed-music.wav")) {
            std::cerr << "\nFailed to load music.";
        }
        //settings
        destroyedSound.setLoop(true);
        destroyedSound.setVolume(100);

        // set buffer for click sound

        clickBuffer.loadFromFile("music/click.wav");
        clickSound.setBuffer(clickBuffer);
        clickSound.setVolume(100);

        // setup for fire sound
        fireBuffer.loadFromFile("music/gun-fire.ogg");
        fireSound.setBuffer(clickBuffer);
        fireSound.setVolume(35);


        //danger music which plays when monster or dragon appear
        dangerMusic.openFromFile("music/beast-entry-music.wav");
        dangerMusic.setVolume(100);
        dangerMusic.setLoop(true);
        

        //game over music
        gameOverMusic.openFromFile("music/game-over-music.wav");
        gameOverMusic.setVolume(100);

        //game complete music
        gameCompleteMusic.openFromFile("music/level-complete-music.wav");
        gameCompleteMusic.setVolume(100);




    }




    // methods to play different music

    void playBgMusic() {
        bgMusic.play();
    }
    void stopBgMusic() {
        bgMusic.stop();
    }

    void playClickSound() {
        clickSound.play();
    }
    void playFireSound() {
        fireSound.play();
    }
    void stopFireSound() {
        fireSound.stop();
    }
    void playDestroyedSound() {
        destroyedSound.play();
    }
    void stopDestroyedSound() {
        destroyedSound.stop();
    }
    void playGameplayMusic() {
        gamePlayMusic.play();
    }
    void stopGameplayMusic() {
        gamePlayMusic.stop();
    }

    void playDangerMusic() {
        dangerMusic.play();
    }
    void stopDangerMusic() {
        dangerMusic.stop();
    }

    void playGameOverMusic() {
        gameOverMusic.play();
    }
    void stopGameOverMusic() {
        gameOverMusic.stop();
    }

    void playGameCompleteMusic() {
        gameCompleteMusic.play();
    }
    void stopGameCompleteMusic() {
        gameCompleteMusic.stop();
    }



};
