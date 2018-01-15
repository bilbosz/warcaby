#include "Player.h"

Player::Player(Color color, Board *board) : color(color), board(board) {}

Player::~Player() {}

Player::Color Player::getColor() const
{
    return color;
}
