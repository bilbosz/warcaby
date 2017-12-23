#pragma once

#include "Board.h"
#include "Game.h"
#include "Resources.h"
#include "Turn.h"

#include "SFML\Graphics.hpp"
#include "SFML\System.hpp"

#include <random>
#include <set>

/** \brief Klasa implementująca grę w warcaby klasyczne. */
class Draughts final : public Game
{
public:
    Draughts();
    virtual ~Draughts();

    virtual void init() override;
    virtual void update(sf::Time advance) override;
    virtual void render() override;

    virtual bool shouldFinish() const override;
    virtual void finish() override;

private:
    static const size_t allOffsetsNumber = 4U;
    static const sf::Vector2i allOffsets[allOffsetsNumber];

    std::mt19937 rng;

    sf::RenderWindow renderWindow;
    sf::View view;
    sf::Vector2f viewLeftTopCorner;
    sf::Text overlayText;
    float fontScaleFactor;

    bool isFinished_;
    bool gameOver;
    Player::Color winner;

    uint16_t sideFieldsNumber;
    Board board;

    uint16_t playerPiecesRowsNumber;
    uint16_t piecesNumber;
    Piece **pieces;

    float fieldMargin;

    Player::Color whoseTurn;

    Turn currentTurn;
    std::multiset<Turn> possibleTurns;
    std::multiset<Turn> narrowedTurns;

    void retrieveEvents();
    void mouseClickedOnBoard(const sf::Vector2f &pressPosition);

    void addPossibleTurns();
    void clearPossibleTurns();
    void addPossiblePieceMoves(Piece *piece);
    void addPossiblePieceJumps(Piece *piece);
    void addPossibleManJumps(Field *startField, Field *lastField, Turn &turnByFar);
    void addPossibleKingJumps(Field *startField, Field *lastField, Turn &turnByFar);

    void narrowTurns();
    void filterMaxCaptures();
    bool isSubturn(const Turn &turn, const Turn &possibleSubturn) const;

    void highlightPossibleSteps();

    void changeTurn();
    void removeCapturesFromBoard();
    void promoteIfPossible();
    void playerWon();
};
