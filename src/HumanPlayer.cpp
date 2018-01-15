#include "HumanPlayer.h"
#include "Draughts.h"
#include "Field.h"
#include "Piece.h"
#include "StepTree.h"

#include <list>

HumanPlayer::HumanPlayer(Player::Color color, Board *board) :
    Player(color, board),
    fieldClicked_(nullptr),
    turnPiece(nullptr)
{
}

HumanPlayer::~HumanPlayer() {}

void HumanPlayer::fieldClicked(Field *field)
{
    fieldClicked_ = field;
}

void HumanPlayer::turnFinished()
{
    turnPiece = nullptr;
    fieldClicked_ = nullptr;
}

Piece *HumanPlayer::getTurnPiece(const std::map<Piece *, StepTree *> &possibleTurns)
{
    if (fieldClicked_ == nullptr)
        return nullptr;
    Piece *clickedPiece = fieldClicked_->getCurrentPiece();
    if (clickedPiece == nullptr)
        return nullptr;
    if (clickedPiece->getColor() != color)
        return nullptr;
    if (!possibleTurns.at(clickedPiece)->isValid)
        return nullptr;
    turnPiece = clickedPiece;
    return turnPiece;
}

StepTree *HumanPlayer::nextStep(Piece *const movingPiece, StepTree *const narrowedTurns)
{
    if (movingPiece != turnPiece)
        return nullptr;
    if (narrowedTurns == nullptr)
        return nullptr;
    const std::list<StepTree *> &possibleNextSteps = narrowedTurns->nextStepList;
    for (StepTree *step : possibleNextSteps) {
        if (step->field == fieldClicked_)
            return step;
    }
    return nullptr;
}

bool HumanPlayer::isInteractive() const
{
    return true;
}