#pragma once

#include "Player.h"

class HumanPlayer final : public Player
{
public:
    HumanPlayer(Player::Color color, Board *board);
    ~HumanPlayer();

    virtual void fieldClicked(Field *field);

    virtual void turnFinished() override;
    virtual Piece *getTurnPiece(const std::map<Piece *, StepTree *> &possibleTurns) override;
    virtual StepTree *nextStep(Piece *const movingPiece, StepTree *const narrowedTurns) override;
    virtual bool isInteractive() const override;

private:
    Field *fieldClicked_;
    Piece *turnPiece;
};