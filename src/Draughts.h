#pragma once

#include "Board.h"
#include "Cursor.h"
#include "Game.h"
#include "Menu.h"
#include "Resources.h"
#include "StepTree.h"
#include "Transition.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <memory>
#include <set>

class Field;
class Piece;
class Player;

/** \brief Klasa implementująca grę w warcaby klasyczne. */
class Draughts final : public Game
{
public:
    enum struct GameState { None = 0, Menu, Gameplay };

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
    sf::View gameplayView;
    sf::Vector2f viewLeftTopCorner;
    sf::Text overlayText;
    sf::Text bannerText;
    float fontScaleFactor;
    Cursor cursor;
    sf::RenderTexture gameplayTexture;
    sf::RenderTexture hudTexture;

    bool gameStarted;
    bool gameOver;
    bool isFinished_;

    uint16_t sideFieldsNumber;
    std::unique_ptr<Board> board;

    std::unique_ptr<Player> player1, player2;
    Player *winner;

    uint16_t playerPiecesRowsNumber;
    uint16_t piecesNumber;

    std::vector<std::unique_ptr<Piece>> pieces;

    float fieldMargin;

    Player *currentPlayer;
    bool waitingForInteraction;

    Piece *selectedPiece;
    std::list<Field *> currentTurn;
    std::map<Piece *, std::unique_ptr<StepTree>> possibleTurns;
    StepTree *narrowedTurns;

    std::list<Piece *> captures;

    std::unique_ptr<Menu> mainMenu, newGameMenu;
    Menu *currentMenu;

    GameState gameState;

    void createMenus();
    void recreateBoard();
    void recreatePieces();
    void prepareGame();
    void startGame();

    void updateCursorPosition();

    void setPiecesPosition(const std::string &file = std::string());

    void retrieveEvents();
    void mouseClickedOnBoard(const sf::Vector2f &pressPosition);
    void mouseClickedOnMenu(const sf::Vector2f &pressPosition);
    void mouseHoverOnMenu(const sf::Vector2f &movePosition);
    void makeScreenshot(const std::string &path) const;

    void enterNewGameMenu();
    void enterMainMenu();
    void set8FieldMode();
    void set10FieldMode();

    void addPossibleTurns();
    void clearPossibleTurns();
    void createPossibleEmptyTurn(Piece *piece);
    void addPossiblePieceMoves(Piece *piece);
    void addPossiblePieceJumps(Piece *piece);
    void addPossibleManJumps(StepTree *currentStep, std::set<Piece *> &currentCaptures, Piece *startPiece);
    void addPossibleKingJumps(StepTree *currentStep, std::set<Piece *> &currentCaptures, Piece *startPiece);

    void narrowTurns();
    void validatePossibleTurns();
    bool isPossibleTurn() const;

    void highlightPossibleSteps();

    void changeTurn();
    void removeCapturesFromBoard();
    void removeCapture(Piece *capture);
    void promoteIfPossible();
    void playerWon();
    void selectPiece();
    void makeStep();

    void setBannerText(const std::wstring &text);
    void clearBannerText();
    void showContinueEntry();
    void hideContinueEntry();

    void quit();
};
