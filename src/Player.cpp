#include "Player.h"

Player::Player(Color color, Board *board) : color(color), board(board) {}

Player::~Player() {}

void Player::fieldClicked(Field * const field) {}

Player::Color Player::getColor() const
{
    return color;
}
