#pragma once

class Player
{
public:
    enum Color { White = 0, Black };

    Player(Color color);
    virtual ~Player();

private:
    Color color;
};