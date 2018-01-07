#pragma once

#include <map>

class Board;
class Field;
class Piece;
struct StepTree;

class Player
{
public:
    enum Color { White = 0, Black };

    Player( Color color );
    Player( Color color, Board* board );

    virtual ~Player();

    /** Reakcja gracza na kliknięcie na planszę.
    * Gracz ludzki powinien implementować to jako ruch. Gracz komputerowy może zostawić domyślną implementację. */
    virtual void fieldClicked( Field *field ) {};

    /** Zwraca bierkę, którą gracz zamierza wykonać ruch. */
    virtual Piece *getTurnPiece( const std::map<Piece *, StepTree *> &possibleTurns ) = 0;

    /** Zwraca następny, możliwy do wykonania ruch z drzewa dostępnych ruchów */
    virtual StepTree *nextStep( Piece *const movingPiece, StepTree *const narrowedTurns ) = 0;

protected:
    Color color;
    Board* board;
};