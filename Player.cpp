#include "Player.h"
#include <SFML/Graphics.hpp>
#include<iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include "AddOn.h"

using json = nlohmann::json;

const std::string FILENAME = "ssPlayers.json";


// Player Constructor
Player::Player(std::string playerName, sf::Vector2f scale) : name(playerName) {
	position = sf::Vector2f(360, 600);

	// Setting up player graphics
	playerTexture.loadFromFile("images/player_ship.png"); // assuming a default texture file
	playerSprite.setTexture(playerTexture);
	playerSprite.setPosition(position);
	playerSprite.setScale(scale);

	// Initializing values
	moveSpeed = 400.5f;
	score = 0;
	highestScore = 0;
	levelProgress = 1;
	maxLives = 3;
	currentLives = 3;
	alive = true;

	// Check if the file "ssPlayers.json" exists, if not, create it with default values
	std::ifstream file(FILENAME);
	if (!file.good()) {
		// File doesn't exist, create an empty array and save it
		std::ofstream outFile(FILENAME);
		json emptyData = json::array();
		outFile << emptyData.dump(4);  // Pretty print with 4 spaces
		outFile.close();
	}
	else {
		// File exists, load the player data from the file
		loadFromFile();
	}
	file.close();
}


// update the player 

void Player::update(sf::Sprite enemyORbullet) {
	takeDamage();  // reduces live by 1 on collision
	destroyAnimation();  // calls destroy animation if lives become 0

}


// move the player

void Player::move(std::string direction, float deltaTime) {
	if (alive) {
		if (direction == "up" && playerSprite.getPosition().y > 50) {
			playerSprite.move(sf::Vector2f(0.0f, -moveSpeed * deltaTime));
		}
		if (direction == "down" && playerSprite.getPosition().y < 650) {
			playerSprite.move(sf::Vector2f(0.0f, moveSpeed * deltaTime));
		}
		if (direction == "left") {
			playerSprite.move(sf::Vector2f(-moveSpeed * deltaTime, 0.0f));
		}
		if (direction == "right") {
			playerSprite.move(sf::Vector2f(moveSpeed * deltaTime, 0.0f));
		}

	}
}

// draw the player to the target window

void Player::draw(sf::RenderWindow& window) {
	window.draw(playerSprite);
}

// get the player's position

sf::Vector2f Player::getPosition() {
	return playerSprite.getPosition();
}

void Player::setPosition(sf::Vector2f position) {
	playerSprite.setPosition(position);
}

// get the player's Global Bounds

sf::FloatRect Player::getLocalBounds() {
	return playerSprite.getLocalBounds();
}

// get global bounds
sf::FloatRect Player::getGlobalBounds() {
	return playerSprite.getGlobalBounds();
}

// check for collision with the bullet / enemy

bool Player::checkCollision(sf::Sprite enemyORbullet) {
	if (playerSprite.getGlobalBounds().intersects(enemyORbullet.getGlobalBounds())) {
		return true;
	}

	return false;
}

bool Player::checkCollision(sf::CircleShape enemyORbullet) {
	if (playerSprite.getGlobalBounds().intersects(enemyORbullet.getGlobalBounds())) {
		return true;
	}

	return false;
}

// destroy the player graphic

void Player::destroyAnimation() {
	if (currentLives <= 0) {
		//enemyORbullet.setScale({0,0});
		playerSprite.setScale({0,0});
		alive = false;
	}
}

// update the player's score

void Player::updateScore(int newScore) {
	score = newScore;
	if (score > highestScore) {
		highestScore = score;
	}
}

// update the level progress

void Player::updateLevelProgress(int progress) {
	levelProgress = progress;
}

// update lives

void Player::addLife(int life) {
	if (currentLives < maxLives) {
		currentLives += life;
	}
}

// get score

int Player::getScore() {
	return score;
}

// get highest score

int Player::getHighestScore() {
	return highestScore;
}

// get level progress

int Player::getLevelProgress() {
	return levelProgress;
}

// get lives

int Player::getLives() {
	return currentLives;
}

// reduce lives when player collides with bullets or enemies

void Player::takeDamage() {
		currentLives -= 1;
}

// check whether the Player exists / is alive

bool Player::isAlive() {
	return alive;
}


// get name of the player

std::string Player::getName() {
	return name;
}


// set the player's name

void Player::setName(std::string newName) {
	name = newName;
}



// file handling


// Save player data to the JSON file


void Player::saveToFile() {
	// Load existing data
	std::ifstream file(FILENAME);
	json playerDataArray;
	file >> playerDataArray;
	file.close();

	// Find if player already exists
	bool playerFound = false;
	for (auto& playerData : playerDataArray) {
		if (playerData["name"] == name) {
			// Update existing player data
			playerData["position"]["x"] = playerSprite.getPosition().x;
			playerData["position"]["y"] = playerSprite.getPosition().y;
			playerData["score"] = score;
			playerData["highestScore"] = highestScore;
			playerData["levelProgress"] = levelProgress;
			playerData["currentLives"] = currentLives;
			playerData["alive"] = alive;
			playerFound = true;
			exists = true;
			break;
		}
	}

	if (!playerFound) {
		// Add new player data
		json newPlayerData;
		newPlayerData["name"] = name;
		newPlayerData["position"]["x"] = playerSprite.getPosition().x;
		newPlayerData["position"]["y"] = playerSprite.getPosition().y;
		newPlayerData["score"] = score;
		newPlayerData["highestScore"] = highestScore;
		newPlayerData["levelProgress"] = levelProgress;
		newPlayerData["currentLives"] = currentLives;
		newPlayerData["alive"] = alive;
		playerDataArray.push_back(newPlayerData);
	}

	// Save updated data back to file
	std::ofstream outFile(FILENAME);
	outFile << playerDataArray.dump(4);  // Pretty print with 4 spaces
	outFile.close();
}


//load

// Load player data from the JSON file
void Player::loadFromFile() {
	std::ifstream file(FILENAME);
	if (file.is_open()) {
		json playerDataArray;
		file >> playerDataArray;
		file.close();

		for (const auto& playerData : playerDataArray) {
			if (playerData["name"] == name) {
				// Load existing player data
				position.x = playerData["position"]["x"];
				position.y = playerData["position"]["y"];
				score = playerData["score"];
				highestScore = playerData["highestScore"];
				levelProgress = playerData["levelProgress"];
				currentLives = playerData["currentLives"];
				alive = playerData["alive"];

				playerSprite.setPosition(position);
				//playerSprite.setScale(sf::Vector2f(1.0f, 1.0f));  // Resetting scale to default if necessary
				break;
			}
		}
	}
	else {
		std::cerr << "Unable to open file for loading: " << FILENAME << std::endl;
	}
}









