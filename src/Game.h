#pragma once

#include "SFML\System.hpp"

#include <sstream>

class Game
{
public:
    Game();
    virtual ~Game();

    virtual void init() = 0;
    virtual void update(sf::Time time);
    virtual void render() = 0;

    virtual bool shouldFinish() const = 0;
    virtual void finish() = 0;

	sf::Time gameTime;
    std::ostringstream gameMessages;
};
