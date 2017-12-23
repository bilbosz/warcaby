#pragma once

#include "Game.h"

/** \brief Klasa obsługująca klasę gry Game.
 *
 * Abstrahując od implementacji ta klasa ma zapewniać prawidłową inicjalizację gry dzięki metodzie GameRunner::init, a
 * także odpowiedni sposób wykonania przez zarządzanie czasem pomiędzy kolejnymi wywołaniami Game::update i
 * Game::render.
 */
class GameRunner
{
public:
    GameRunner(Game *game);
    virtual ~GameRunner();

    virtual void init() = 0;
    virtual void run() = 0;

protected:
    Game *game;
};
