#pragma once

#include "Player.h"

class HumanPlayer final : public Player
{
public:
    HumanPlayer(Player::Color color, Board *const board);
    ~HumanPlayer();

    virtual void fieldClicked(Field *const field) override;

    virtual void turnFinished() override;
    virtual Piece *getTurnPiece(const std::map<Piece *, std::unique_ptr<StepTree>> &possibleTurns) override;
    virtual StepTree *nextStep(Piece *const movingPiece, StepTree *const narrowedTurns) override;
    virtual bool isInteractive() const override;

private:
    Field *fieldClicked_;
    Piece *turnPiece;
};