#include "Game.h"

Game::Game() : gameTime(sf::Time::Zero), gameMessages() {}

Game::~Game() {}

void Game::update(sf::Time time)
{
    gameTime = time;
}