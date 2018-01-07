#pragma once

#include "Player.h"
#include "Field.h"
#include "Draughts.h"

class HumanPlayer: public Player
{
    /** Reakcja gracza na klikniêcie na planszê.
    * Gracz ludzki powinien implementowaæ to jako ruch. Gracz komputerowy mo¿e zostawiæ domyœln¹ implementacjê. */
    virtual void fieldClicked( Field *field );

    /** Zwraca bierkê, któr¹ gracz zamierza wykonaæ ruch. */
    virtual Piece *getTurnPiece( const std::map<Piece *, StepTree *> &possibleTurns ) override;

    /** Zwraca nastêpny, mo¿liwy do wykonania ruch z drzewa dostêpnych ruchów */
    virtual StepTree *nextStep( Piece *const movingPiece, StepTree *const narrowedTurns ) override;
};