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
    sf::Time lastUpdate = now(), nextUpdate = lastUpdate + updateTick, sleepDuration = sf::Time::Zero,
             sleepAdjustment = sf::Time::Zero, delay = sf::Time::Zero;

    sf::Time measureBeforeUpdate = sf::Time::Zero, measureAfterUpdate = sf::Time::Zero,
             measureBeforeRender = sf::Time::Zero, measureAfterRender = sf::Time::Zero;

    while (true) {
        measureBeforeUpdate = now();
        game->update(nextUpdate);
        if (game->shouldFinish())
            break;
        measureAfterUpdate = now();

        sleepDuration = nextUpdate - now();

        measureBeforeRender = now();
        game->render();
        measureAfterRender = now();

        game->gameMessages.str(std::string());
        game->gameMessages.clear();
        game->gameMessages << (measureAfterRender - lastUpdate).asMilliseconds() << std::endl;

        lastUpdate = nextUpdate;
        nextUpdate = lastUpdate + updateTick;
        ++updateNumber;
    }
    game->finish();
}

sf::Time SingleThreadGameRunner::now() const
{
    return gameClock.getElapsedTime();
}
