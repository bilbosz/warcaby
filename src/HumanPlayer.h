#pragma once

#include "Player.h"
#include "Field.h"
#include "Draughts.h"

class HumanPlayer: public Player
{
    /** Reakcja gracza na klikni�cie na plansz�.
    * Gracz ludzki powinien implementowa� to jako ruch. Gracz komputerowy mo�e zostawi� domy�ln� implementacj�. */
    virtual void fieldClicked( Field *field );

    /** Zwraca bierk�, kt�r� gracz zamierza wykona� ruch. */
    virtual Piece *getTurnPiece( const std::map<Piece *, StepTree *> &possibleTurns ) override;

    /** Zwraca nast�pny, mo�liwy do wykonania ruch z drzewa dost�pnych ruch�w */
    virtual StepTree *nextStep( Piece *const movingPiece, StepTree *const narrowedTurns ) override;
};