#include "SingleThreadGameRunner.h"

SingleThreadGameRunner::SingleThreadGameRunner(Game *game, uint16_t updateRate) :
    GameRunner(game),
    gameClock(),
    updateRate(updateRate),
    updateTick(sf::microseconds(1'000'000U / updateRate)),
    updateNumber(0ULL)
{
}

SingleThreadGameRunner::~SingleThreadGameRunner() {}

void SingleThreadGameRunner::init()
{
    game->init();
    gameClock.restart();
}

void SingleThreadGameRunner::run()
{
    sf::Time beforeRender = gameClock.getElapsedTime();
    while (true) {
        game->update(gameClock.getElapsedTime());
        if (game->shouldFinish()) {
            break;
        }
        while (gameClock.getElapsedTime() < beforeRender + updateTick)
            ;
        beforeRender = gameClock.getElapsedTime();
        game->render();
        ++updateNumber;
    }
    game->finish();
}

sf::Time SingleThreadGameRunner::now() const
{
    return gameClock.getElapsedTime();
}
