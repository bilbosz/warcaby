#include "ComputerPlayer.h"
#include "Error.h"
#include "Resources.h"
#include "StepTree.h"

#include <random>

ComputerPlayer::ComputerPlayer(Player::Color color, Board *board) : Player(color, board), turnPiece(nullptr) {}

ComputerPlayer::~ComputerPlayer() {}

void ComputerPlayer::turnFinished()
{
    turnPiece = nullptr;
}

Piece *ComputerPlayer::getTurnPiece(const std::map<Piece *, std::unique_ptr<StepTree>> &possibleTurns)
{
    std::vector<Piece *> possiblePieces;
    for (std::pair<Piece *, const std::unique_ptr<StepTree> &> turnByPiece : possibleTurns) {
        StepTree *turn = turnByPiece.second.get();
        if (turn->isValid)
            possiblePieces.push_back(turnByPiece.first);
    }
    if (possiblePieces.empty())
        throw Error("Brak mo¿liwych ruchów do wykonania");
    std::uniform_int_distribution<size_t> chooser(0U, possiblePieces.size() - 1U);
    return possiblePieces[chooser(Resources::RandomNumberGenerator)];
}

StepTree *ComputerPlayer::nextStep(Piece *const movingPiece, StepTree *const narrowedTurns)
{
    std::vector<StepTree *> possibleSteps;
    for (StepTree *step : narrowedTurns->nextStepList) {
        if (step->isValid)
            possibleSteps.push_back(step);
    }
    if (possibleSteps.empty())
        throw Error("Brak mo¿liwych ruchów do wykonania");
    std::uniform_int_distribution<size_t> chooser(0U, possibleSteps.size() - 1U);
    return possibleSteps[chooser(Resources::RandomNumberGenerator)];
}

bool ComputerPlayer::isInteractive() const
{
    return false;
}
