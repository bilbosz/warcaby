#pragma once

#include "Field.h"
#include "Piece.h"

#include <list>

class Field;
class Piece;

/** \brief Kolejka gracza.
 *
 * Na jedną kolejkę gracza składać się będzie pojedynczy ruch bierką albo sekwencja bić. Na potrzeby sortowania ruchów
 * zaimplementowano operator<, który pomoże stwierdzić czy dany ruch zawiera maksymalną możliwą liczbę bić bierek
 * przeciwnika.
 */
struct Turn {
    Turn();
    virtual ~Turn();

    std::list<Field *> stepList;
    std::list<Piece *> capturesList;
    uint16_t capturesNumber;

    void reset();

    std::string getInterpretation() const;
    uint16_t getCapturesNumber() const;
    bool operator<(const Turn &that) const;
};