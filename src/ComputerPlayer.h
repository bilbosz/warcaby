#pragma once

#include "Player.h"

class ComputerPlayer final : public Player
{
public:
    ComputerPlayer(Player::Color color, Board * const board);
    virtual ~ComputerPlayer();

    virtual void turnFinished() override;
    virtual Piece *getTurnPiece(const std::map<Piece *, std::unique_ptr<StepTree>> &possibleTurns) override;
    virtual StepTree *nextStep(Piece *const movingPiece, StepTree *const narrowedTurns) override;
    virtual bool isInteractive() const override;

private:
    Piece *turnPiece;
};
