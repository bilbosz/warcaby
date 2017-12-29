#pragma once

#include "SFML\System.hpp"

#include <sstream>

class Game
{
public:
    Game();
    virtual ~Game();

    virtual void init() = 0;
    virtual void update(sf::Time advance) = 0;
    virtual void render() = 0;

    virtual bool shouldFinish() const = 0;
    virtual void finish() = 0;

    std::ostringstream gameMessages;
};
