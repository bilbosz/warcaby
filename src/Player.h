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

    Player(Color color, Board *board);
    virtual ~Player();

    /** Reakcja gracza na kliknięcie na planszę.
     *
     * Gracz interaktywny powinien implementować to jako ruch. Gracz komputerowy może zostawić domyślną implementację.
     */
    virtual void fieldClicked(Field *field){};

    /** Wywoływane przez grę w momencie skończenia ruchu przez tego gracza. */
    virtual void turnFinished() = 0;

    /** Zwraca bierkę, którą gracz zamierza wykonać ruch. */
    virtual Piece *getTurnPiece(const std::map<Piece *, StepTree *> &possibleTurns) = 0;

    /** Zwraca następny, możliwy do wykonania ruch z drzewa dostępnych ruchów */
    virtual StepTree *nextStep(Piece *const movingPiece, StepTree *const narrowedTurns) = 0;

    /* W trakcie tury gracza interaktywnego pola zostają podświtlone i gra oczekuje na jego reakcję */
    virtual bool isInteractive() const = 0;

    /** Zwraca kolor bierek gracza */
    Color getColor() const;

protected:
    Color color;
    Board *board;
};