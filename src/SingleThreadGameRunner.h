#pragma once

#include "GameRunner.h"

#include "SFML\System.hpp"

#include <cstdint>

/** \brief Odmiana jednowątkowa obsługi obiektu z klasy Game.
 *
 * SingleThreadGameRunner zapewnia obsługę metody Game::update i Game::render, by sprawiały wrażenie płynności. Metody
 * Game::shouldFinish i Game::finish są wykorzystywane do poinformowania i zakończenia działania
 * SingleThreadGameRunner::run. Przed wykonaniem metody SingleThreadGameRunner::run należy wywołać
 * SingleThreadGameRunner::init.
 *
 *  \see Game
 */
class SingleThreadGameRunner final : public GameRunner
{
public:
    SingleThreadGameRunner(Game *game, uint16_t updateRate);
    virtual ~SingleThreadGameRunner();

    virtual void init() override;
    virtual void run() override;

private:
    sf::Clock gameClock;
    uint16_t updateRate;
    sf::Time updateTick;
    uint64_t updateNumber;

    sf::Time now() const;
};
