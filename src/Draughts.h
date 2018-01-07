#pragma once

#include "Board.h"
#include "Game.h"
#include "Resources.h"
#include "StepTree.h"
#include "Transition.h"

#include "SFML\Graphics.hpp"
#include "SFML\System.hpp"

#include <filesystem>
#include <random>
#include <set>

class Field;
class Piece;

/** \brief Klasa implementująca grę w warcaby klasyczne. */
class Draughts final : public Game
{
public:
    Draughts();
    virtual ~Draughts();

    virtual void init() override;
    virtual void update(sf::Time time) override;
    virtual void render() override;

    virtual bool shouldFinish() const override;
    virtual void finish() override;

private:
    static const size_t AllOffsetsNumber = 4U;
    static const sf::Vector2i AllOffsets[AllOffsetsNumber];

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

    std::list<Field *> currentTurn;
    std::map<Piece *, StepTree *> possibleTurns;
    StepTree *narrowedTurns;

	std::list<Piece *> captures;

    bool goodPiceSelected( Field* pressedField );
    void MakeAMove( Field* pressedField );
    void showPosibleSteps( Field *pressedField );

    void setPiecesPosition(std::experimental::filesystem::path file = std::experimental::filesystem::path());

    void retrieveEvents();
    void mouseClickedOnBoard(const sf::Vector2f &pressPosition);

    void addPossibleTurns();
    void clearPossibleTurns();
    void createPossibleEmptyTurn(Piece *piece);
    void addPossiblePieceMoves(Piece *piece);
    void addPossiblePieceJumps(Piece *piece);
    void addPossibleManJumps(StepTree *currentStep, std::set<Piece *> &currentCaptures, Piece *startPiece);
    void addPossibleKingJumps(StepTree *currentStep, std::set<Piece *> &currentCaptures, Piece *startPiece);

    void narrowTurns();
    void validatePossibleTurns();

    void highlightPossibleSteps();

    void changeTurn();
    void removeCapturesFromBoard();
    void removeCapture(Piece *capture);
    void promoteIfPossible();
    void playerWon();
};
