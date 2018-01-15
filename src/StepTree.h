#pragma once

#include <cstdint>
#include <list>

class Field;
class Piece;

/** \brief Drzewo możliwych do wykonania kroków.
 *
 * Krok może być zarówno biciem jak i ruchem.
 */
struct StepTree {
    StepTree();
    ~StepTree();

    /* Oblicza maksymalną liczbę zbić dla tego kroku i jemu następnych. */
    void evalCapturesNumber();
    void validate(uint16_t capturesNumber);
    /* Wskazuje na rodzica danego wierzchołka. Jeśli nullptr to wierzchołek jest wierzchołkiem drzewa. */
    StepTree *parent;
    /* Wskazuje na pole na którym dana bierka została przemieszczona. */
    Field *field;
    /* Jeśli nastąpiło bicie bierki przeciwnika ta zmienna będzie przechowywać wskaźnik na tą bierkę, jeśli nie
     * nullptr. */
    Piece *capture;
    /* Lista kroków które są dostępne po obecnym ruchu. Ta lista jest pusta jeśli to ostatni krok. */
    std::list<StepTree *> nextStepList;
    /* Ustala czy wybrany ruch może być dodany do tury, ograniczeniem będą zasady gry w warcaby. Przykładowo gracz musi
     * wykonać turę o maksymalnej liczbie bić bierek przeciwnika. */
    bool isValid;
    uint16_t capturesNumber;
};