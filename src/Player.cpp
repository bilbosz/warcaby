#include "Player.h"

Player::Player( Color color ) : color( color ) {}
Player::Player( Color color, Board* board ) : color( color ), board( board ) {}

Player::~Player() {}
