#include "HumanPlayer.h"
#include "Field.h"

void HumanPlayer::fieldClicked( Field * field )
{

}

Piece* HumanPlayer::getTurnPiece( const std::map<Piece*, StepTree*>& possibleTurns )
{
    return nullptr;
}

StepTree* HumanPlayer::nextStep( Piece * const movingPiece, StepTree * const narrowedTurns )
{
    return nullptr;
}