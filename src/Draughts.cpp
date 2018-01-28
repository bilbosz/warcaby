#include "Draughts.h"
#include "ComputerPlayer.h"
#include "Error.h"
#include "HumanPlayer.h"
#include "MenuOption.h"
#include "MenuRedirection.h"
#include "Piece.h"

#include <algorithm>
#include <fstream>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>

const sf::Vector2i Draughts::AllOffsets[Draughts::AllOffsetsNumber] = {
    sf::Vector2i(-1, 1), sf::Vector2i(1, 1), sf::Vector2i(1, -1), sf::Vector2i(-1, -1)
};

Draughts::Draughts() :
    Game(),
    renderWindow(),
    gameplayView(),
    viewLeftTopCorner(),
    overlayText(),
    bannerText(),
    fontScaleFactor(),
    cursor(),
    gameplayTexture(),
    hudTexture(),
    gameStarted(false),
    gameOver(false),
    isFinished_(false),
    sideFieldsNumber(8U),
    board(std::make_unique<Board>(sideFieldsNumber)),
    player1(std::make_unique<HumanPlayer>(Player::Color::White, board.get())),
    player2(std::make_unique<HumanPlayer>(Player::Color::Black, board.get())),
    winner(),
    playerPiecesRowsNumber(3U),
    piecesNumber(playerPiecesRowsNumber * 2 * sideFieldsNumber / 2),
    pieces(),
    fieldMargin(Resources::FieldMarginThickness),
    currentPlayer(nullptr),
    waitingForInteraction(false),
    selectedPiece(),
    currentTurn(),
    possibleTurns(),
    narrowedTurns(),
    captures(),
    mainMenu(std::make_unique<Menu>()),
    newGameMenu(std::make_unique<Menu>()),
    currentMenu(nullptr),
    gameState(GameState::None)
{
}

Draughts::~Draughts() {}

void Draughts::init()
{
	sf::VideoMode videoMode = sf::VideoMode::getFullscreenModes()[0];
    if (!Resources::IsFullscreen) {
	    videoMode = sf::VideoMode::getDesktopMode();
        videoMode.width = static_cast<unsigned int>(videoMode.width * Resources::WindowDesktopWidthRatio);
        videoMode.height = static_cast<unsigned int>(videoMode.height * Resources::WindowDesktopHeightRatio);
    }
    sf::ContextSettings settings;
    settings.antialiasingLevel = Resources::AntialiasingLevel;
    renderWindow.setFramerateLimit(Resources::UpdateRate);
    renderWindow.setVerticalSyncEnabled(Resources::VerticalSync);
    renderWindow.setKeyRepeatEnabled(Resources::KeyRepeat);
    renderWindow.setMouseCursorGrabbed(Resources::MouseCursorGrab);

    gameplayTexture.create(videoMode.width, videoMode.height);
    hudTexture.create(videoMode.width, videoMode.height);

    createMenus();

    gameState = GameState::Menu;
    enterMainMenu();

    cursor.init();

    overlayText.setPosition(0.0f, 0.0f);
    overlayText.setFont(Resources::InfoFont);
    overlayText.setFillColor(Resources::InfoTextColor);

    bannerText.setPosition(0.0f, 0.0f);
    bannerText.setFont(Resources::DefaultFont);
    bannerText.setFillColor(Resources::MenuTitleColor);
    bannerText.setCharacterSize(Resources::MenuCharacterSize);

    renderWindow.create(videoMode, Resources::GameTitle, Resources::WindowStyle, settings);
    renderWindow.setMouseCursorVisible(false);

    gameplayTexture.setView(gameplayView);
    prepareGame();
}

void Draughts::update(sf::Time time)
{
    Game::update(time);

    retrieveEvents();

    updateCursorPosition();
    board->update(time);

    // odśwież położenie bierek
    bool pieceMoving = false;
    for (const std::unique_ptr<Piece> &piece : pieces) {
        piece->update(time);
        if (piece->isMovingTransitionRunning())
            pieceMoving = true;
    }
    if (gameState == GameState::Menu) {
        mainMenu->update(gameTime);
    } else if (gameState == GameState::Gameplay) {
        // gracz komputerowy czeka na koniec animacji
        if (pieceMoving && !currentPlayer->isInteractive())
            return;
        // nie wybrano bierki
        if (selectedPiece == nullptr) {
            selectPiece();
            narrowTurns();
        } else {
            makeStep();
        }
        board->clearSelection();
        if (currentPlayer->isInteractive()) {
            highlightPossibleSteps();
        }
    }
}

void Draughts::render()
{
    gameplayTexture.clear(Resources::BackgroundColor);
    hudTexture.clear(sf::Color::Transparent);
    if (gameState == GameState::Gameplay || gameState == GameState::Menu) {
        gameplayTexture.draw(*board);
        // wszystkie zbite bierki
        for (Piece *capture : captures) {
            // bierka animowana
            if (capture->isMovingTransitionRunning())
                continue;
            gameplayTexture.draw(*capture);
        }
        // wszystkie bierki, które się nie ruszają i nie są zbite
        for (const std::unique_ptr<Piece> &piece : pieces) {
            // bierka jest zbita
            if (piece->getCurrentField() == nullptr)
                continue;
            if (piece->isMovingTransitionRunning())
                continue;
            gameplayTexture.draw(*piece);
        }
        // bierki animowane na górze (wyświetlane ostatnie)
        for (const std::unique_ptr<Piece> &piece : pieces) {
            if (!piece->isMovingTransitionRunning())
                continue;
            gameplayTexture.draw(*piece);
        }
        hudTexture.draw(bannerText);
        if (gameState == GameState::Menu) {
            hudTexture.draw(*currentMenu);
        }
    }
    hudTexture.draw(overlayText);
    hudTexture.draw(cursor);
    renderWindow.clear(Resources::BackgroundColor);
    gameplayTexture.display();
    hudTexture.display();
    sf::Sprite gameplaySprite(gameplayTexture.getTexture()), hudSprite(hudTexture.getTexture());
    renderWindow.draw(gameplaySprite);
    renderWindow.draw(hudSprite);
    renderWindow.display();
}

bool Draughts::shouldFinish() const
{
    return isFinished_;
}

void Draughts::finish()
{
    renderWindow.close();
}

void Draughts::createMenus()
{
    // menu główne
    mainMenu->init();
    mainMenu->setTitle(Resources::GameTitle);

    std::unique_ptr<MenuRedirection> newGameEntry = std::make_unique<MenuRedirection>(),
                                     continueGameEntry = std::make_unique<MenuRedirection>(),
                                     exitGameEntry = std::make_unique<MenuRedirection>();

    newGameEntry->init();
    newGameEntry->setName(L"Nowa gra");
    newGameEntry->setTarget(boost::bind(boost::mem_fn(&Draughts::enterNewGameMenu), this));
    mainMenu->addEntry(std::move(newGameEntry));

    continueGameEntry->init();
    continueGameEntry->setName(L"Kontynuuj");
    continueGameEntry->setTarget(boost::bind(boost::mem_fn(&Draughts::startGame), this));
    mainMenu->addEntry(std::move(continueGameEntry));
    hideContinueEntry();
    mainMenu->hideEntryByName(L"Kontynuuj");

    exitGameEntry->init();
    exitGameEntry->setName(L"Wyjdź z gry");
    exitGameEntry->setTarget(boost::bind(boost::mem_fn(&Draughts::quit), this));
    mainMenu->addEntry(std::move(exitGameEntry));

    // menu nowej gry
    newGameMenu->init();
    newGameMenu->setTitle(L"Nowa gra");

    std::unique_ptr<MenuRedirection> startGameEntry = std::make_unique<MenuRedirection>(),
                                     backToMainMenuEntry = std::make_unique<MenuRedirection>();
    std::unique_ptr<MenuOption> modeEntry = std::make_unique<MenuOption>(),
                                whitePlayerEntry = std::make_unique<MenuOption>(),
                                blackPlayerEntry = std::make_unique<MenuOption>();

    startGameEntry->init();
    startGameEntry->setName(L"Rozpocznij nową grę");
    startGameEntry->setTarget(boost::bind(boost::mem_fn(&Draughts::startGame), this));
    newGameMenu->addEntry(std::move(startGameEntry));

    modeEntry->init();
    modeEntry->setName(L"Tryb gry");
    modeEntry->addValue(L"64 pola", boost::bind(boost::mem_fn(&Draughts::set8FieldMode), this));
    modeEntry->addValue(L"100 pól", boost::bind(boost::mem_fn(&Draughts::set10FieldMode), this));
    modeEntry->updateEntryText();
    newGameMenu->addEntry(std::move(modeEntry));

    whitePlayerEntry->init();
    whitePlayerEntry->setName(L"Grający białymi");
    whitePlayerEntry->addValue(L"Człowiek", [this]() -> void {
        player1 = std::make_unique<HumanPlayer>(Player::Color::White, board.get());
        prepareGame();
    });
    whitePlayerEntry->addValue(L"Komputer", [this]() -> void {
        player1 = std::make_unique<ComputerPlayer>(Player::Color::White, board.get());
        prepareGame();
    });
    whitePlayerEntry->updateEntryText();
    newGameMenu->addEntry(std::move(whitePlayerEntry));

    blackPlayerEntry->init();
    blackPlayerEntry->setName(L"Grający czarnymi");
    blackPlayerEntry->addValue(L"Człowiek", [this]() -> void {
        player2 = std::make_unique<HumanPlayer>(Player::Color::Black, board.get());
        prepareGame();
    });
    blackPlayerEntry->addValue(L"Komputer", [this]() -> void {
        player2 = std::make_unique<ComputerPlayer>(Player::Color::Black, board.get());
        prepareGame();
    });
    blackPlayerEntry->updateEntryText();
    newGameMenu->addEntry(std::move(blackPlayerEntry));

    backToMainMenuEntry->init();
    backToMainMenuEntry->setName(L"Wróć do menu głównego");
    backToMainMenuEntry->setTarget(boost::bind(boost::mem_fn(&Draughts::enterMainMenu), this));
    newGameMenu->addEntry(std::move(backToMainMenuEntry));
}

void Draughts::recreateBoard()
{
    board = std::make_unique<Board>(sideFieldsNumber);
    board->init();
    board->setPosition(0.0f, 0.0f);
    board->setFontScaleFactor(fontScaleFactor);
}

void Draughts::recreatePieces()
{
    pieces.clear();
    pieces.resize(piecesNumber);
    for (uint16_t i = 0U; i < piecesNumber; ++i) {
        Player::Color color = Player::Color::White;
        if (i >= piecesNumber / 2)
            color = Player::Color::Black;
        pieces[i] = std::make_unique<Piece>(i, color);
    }

    for (const std::unique_ptr<Piece> &piece : pieces) {
        piece->init();
        piece->setFieldMargin(fieldMargin);
        piece->setFontScaleFactor(fontScaleFactor);
    }
}

void Draughts::prepareGame()
{
    hideContinueEntry();
    gameOver = false;
    winner = nullptr;
    clearBannerText();
    captures.clear();
    selectedPiece = nullptr;
    currentTurn.clear();
    narrowedTurns = nullptr;
    clearPossibleTurns();

    piecesNumber = playerPiecesRowsNumber * 2 * sideFieldsNumber / 2;

    recreateBoard();
    recreatePieces();

    const sf::Vector2u &renderWindowSize = renderWindow.getSize();
    float gameplayViewHeight = static_cast<float>(sideFieldsNumber) + 2.0f;
    fontScaleFactor = gameplayViewHeight / renderWindowSize.y;

    gameplayView.setCenter((gameplayViewHeight - 2.0f) / 2.0f, (gameplayViewHeight - 2.0f) / 2.0f);
    gameplayView.setSize(fontScaleFactor * renderWindowSize.x, gameplayViewHeight);
    gameplayView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
    viewLeftTopCorner.x = gameplayView.getCenter().x - gameplayView.getSize().x / 2.0f;
    viewLeftTopCorner.y = gameplayView.getCenter().y - gameplayView.getSize().y / 2.0f;
    gameplayTexture.setView(gameplayView);

    currentPlayer = player1.get();

    setPiecesPosition();

    addPossibleTurns();
}

void Draughts::startGame()
{
    gameState = GameState::Gameplay;
    showContinueEntry();
    clearBannerText();
}

void Draughts::updateCursorPosition()
{
    sf::Vector2f cursorPosition = hudTexture.mapPixelToCoords(sf::Mouse::getPosition(renderWindow));
    cursor.setPosition(cursorPosition);
}

void Draughts::setPiecesPosition(const std::string &file)
{
    std::list<Field *> blackFields = board->getFieldsByColor(Field::Color::Black);
    for (const std::unique_ptr<Piece> &piece : pieces) {
        if (blackFields.empty())
            throw Error("Bierek jest więcej niż pozycji");
        Field *field;
        if (piece->getColor() == Player::Color::White) {
            field = blackFields.front();
            blackFields.pop_front();
        } else {
            field = blackFields.back();
            blackFields.pop_back();
        }
        piece->setCurrentField(field);
        field->setCurrentPiece(piece.get());
        piece->setPosition(field->getPosition());
    }
    if (file == std::string())
        return;
    for (const std::unique_ptr<Piece> &piece : pieces) {
        removeCapture(piece.get());
    }
    // działa tylko dla poprawnego tagu FEN pliku PDN, dla niepoprawnego mamy zachowanie niezdefiniowane
    std::ifstream fileStream(file);
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    std::string initialBoardContent(buffer.str());
    std::transform(initialBoardContent.begin(), initialBoardContent.end(), initialBoardContent.begin(), ::tolower);
    boost::char_separator<char> separators(",:");
    boost::tokenizer<boost::char_separator<char>> tokenizer(initialBoardContent, separators);
    uint16_t pieceNumber = 0U;
    for (const std::string &token : tokenizer) {
        Player::Color settingFor;
        bool king = false;
        if (token == "w") {
            currentPlayer = player1.get();
            continue;
        }
        if (token == "b") {
            currentPlayer = player2.get();
            continue;
        }
        std::string::const_iterator iter = token.cbegin(), iter2 = token.cend() - 2U;
        std::string fieldName(iter2, token.cend());

        if (iter != iter2 && *iter == 'w') {
            settingFor = Player::Color::White;
            pieceNumber = pieceNumber % (piecesNumber / 2U);
            ++iter;
        } else if (iter != iter2 && *iter == 'b') {
            settingFor = Player::Color::Black;
            pieceNumber = pieceNumber % (piecesNumber / 2U) + (piecesNumber / 2U);
            ++iter;
        }
        if (iter != iter2 && *iter == 'k') {
            king = true;
            ++iter;
        }
        Field *field = board->getFieldByName(fieldName);
        Piece *piece = pieces[pieceNumber].get();
        piece->setCurrentField(field);
        field->setCurrentPiece(piece);
        piece->setPosition(field->getPosition());
        if (king)
            piece->upgrade();
        ++pieceNumber;
    }
}

void Draughts::retrieveEvents()
{
    sf::Event event;
    while (renderWindow.pollEvent(event)) {
        if (event.type == sf::Event::EventType::Closed) {
            quit();
            continue;
        }
        if (event.type == sf::Event::EventType::KeyPressed) {
            if (event.key.alt && event.key.code == sf::Keyboard::F4) {
                quit();
                continue;
            }
            if (event.key.code == sf::Keyboard::F12) {
                makeScreenshot("screenshot.png");
                continue;
            }
        }
        if (gameState == GameState::Menu) {
            if (event.type == sf::Event::EventType::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    if (!gameStarted)
                        continue;
                    startGame();
                    continue;
                }
            }
            if (event.type == sf::Event::EventType::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f pressPoint =
                        hudTexture.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    mouseClickedOnMenu(pressPoint);
                }
            } else if (event.type == sf::Event::EventType::MouseMoved) {
                sf::Vector2f movePoint =
                    hudTexture.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                mouseHoverOnMenu(movePoint);
            }
        } else if (gameState == GameState::Gameplay) {
            if (event.type == sf::Event::EventType::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    gameState = GameState::Menu;
                    enterMainMenu();
                    setBannerText(L"Pauza");
                    continue;
                }
            }
            if (gameOver)
                continue;
            if (!currentPlayer->isInteractive())
                continue;
            if (event.type == sf::Event::EventType::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f pressPoint =
                        gameplayTexture.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    mouseClickedOnBoard(pressPoint);
                }
            }
        }
    }
}

void Draughts::mouseClickedOnBoard(const sf::Vector2f &pressPoint)
{
    Field *pressedField = board->getFieldByPosition(pressPoint);
    if (pressedField == nullptr)
        return;
    if (!currentPlayer->isInteractive())
        return;
    currentPlayer->fieldClicked(pressedField);
}

void Draughts::mouseClickedOnMenu(const sf::Vector2f &pressPosition)
{
    currentMenu->mouseClicked(pressPosition);
}

void Draughts::mouseHoverOnMenu(const sf::Vector2f &movePosition)
{
    currentMenu->mouseHovered(movePosition);
}

void Draughts::makeScreenshot(const std::string &path) const
{
    sf::Vector2u windowSize = renderWindow.getSize();
    sf::Texture texture;
    texture.create(windowSize.x, windowSize.y);
    texture.update(renderWindow);
    texture.copyToImage().saveToFile(path);
}

void Draughts::enterNewGameMenu()
{
    currentMenu = newGameMenu.get();
    currentMenu->unselectAll();
    prepareGame();
}

void Draughts::enterMainMenu()
{
    currentMenu = mainMenu.get();
    currentMenu->unselectAll();
}

void Draughts::set8FieldMode()
{
    sideFieldsNumber = 8U;
    playerPiecesRowsNumber = 3U;
    prepareGame();
}

void Draughts::set10FieldMode()
{
    sideFieldsNumber = 10U;
    playerPiecesRowsNumber = 4U;
    prepareGame();
}

void Draughts::addPossibleTurns()
{
    for (const std::unique_ptr<Piece> &piece : pieces) {
        if (currentPlayer->getColor() != piece->getColor())
            continue;
        Field *field = piece->getCurrentField();
        if (field == nullptr)
            continue;
        createPossibleEmptyTurn(piece.get());
        addPossiblePieceMoves(piece.get());
        addPossiblePieceJumps(piece.get());
    }
    validatePossibleTurns();
    narrowedTurns = nullptr;
    if (isPossibleTurn())
        return;
    playerWon();
    board->clearSelection();
    return;
}

void Draughts::clearPossibleTurns()
{
    possibleTurns.clear();
}

void Draughts::createPossibleEmptyTurn(Piece *piece)
{
    possibleTurns[piece] = std::make_unique<StepTree>();
    possibleTurns[piece]->field = piece->getCurrentField();
}

void Draughts::addPossiblePieceMoves(Piece *piece)
{
    Field *field = piece->getCurrentField();
    sf::Vector2i boardPosition = field->getBoardPostion();
    Piece::PieceType pieceType = piece->getPieceType();
    Player::Color color = piece->getColor();
    int forwardDirection = 1;
    if (color == Player::Color::Black)
        forwardDirection = -1;
    StepTree *previousStep = possibleTurns[piece].get();
    if (pieceType == Piece::PieceType::Man) {
        const size_t forwardMovesOffsetsNumber = 2U;
        const sf::Vector2i forwardMovesOffsets[forwardMovesOffsetsNumber] = { sf::Vector2i(-1, forwardDirection),
                                                                              sf::Vector2i(1, forwardDirection) };
        std::list<sf::Vector2i> forwardMoves;
        for (const sf::Vector2i &offset : forwardMovesOffsets) {
            sf::Vector2i targetBoardPosition(boardPosition + offset);
            if (board->isBoardPositionInvalid(targetBoardPosition))
                continue;
            if (board->isBoardPositionOccupied(targetBoardPosition))
                continue;
            Field *targetField = board->getFieldByBoardPosition(targetBoardPosition);
            StepTree *targetStep = new StepTree();
            targetStep->parent = previousStep;
            targetStep->field = targetField;
            previousStep->nextStepList.push_back(targetStep);
        }
    } else if (pieceType == Piece::PieceType::King) {
        std::list<sf::Vector2i> allDirectionMoves;
        for (const sf::Vector2i &directionOffset : AllOffsets) {
            for (int i = 1; i <= sideFieldsNumber; ++i) {
                sf::Vector2i targetBoardPosition(boardPosition + i * directionOffset);
                if (board->isBoardPositionInvalid(targetBoardPosition))
                    break;
                if (board->isBoardPositionOccupied(targetBoardPosition))
                    break;
                allDirectionMoves.push_back(targetBoardPosition);
            }
        }
        for (const sf::Vector2i &move : allDirectionMoves) {
            if (board->isBoardPositionOccupied(move))
                continue;
            Field *targetField = board->getFieldByBoardPosition(move);
            StepTree *targetStep = new StepTree();
            targetStep->parent = previousStep;
            targetStep->field = targetField;
            previousStep->nextStepList.push_back(targetStep);
        }
    }
}

void Draughts::addPossiblePieceJumps(Piece *piece)
{
    Field *field = piece->getCurrentField();
    Piece::PieceType pieceType = piece->getPieceType();
    StepTree *stepTree = possibleTurns[piece].get();
    std::set<Piece *> captures;
    if (pieceType == Piece::PieceType::Man) {
        addPossibleManJumps(stepTree, captures, piece);
    } else if (pieceType == Piece::PieceType::King) {
        addPossibleKingJumps(stepTree, captures, piece);
    }
}

void Draughts::addPossibleManJumps(StepTree *currentStep, std::set<Piece *> &currentCaptures, Piece *startPiece)
{
    Player::Color startColor = startPiece->getColor();
    Field *startField = startPiece->getCurrentField();
    sf::Vector2i lastBoardPosition = currentStep->field->getBoardPostion(),
                 startBoardPosition = startField->getBoardPostion();
    for (const sf::Vector2i &offset : AllOffsets) {
        sf::Vector2i attackPosition = lastBoardPosition + offset;
        if (board->isBoardPositionInvalid(attackPosition))
            continue;
        if (!board->isBoardPositionOccupied(attackPosition))
            continue;
        Field *attackField = board->getFieldByBoardPosition(attackPosition);
        Piece *attackPiece = attackField->getCurrentPiece();
        if (currentCaptures.find(attackPiece) != currentCaptures.end())
            continue;
        Player::Color attackColor = attackPiece->getColor();
        if (attackColor == startColor)
            continue;
        sf::Vector2i targetPosition = lastBoardPosition + 2 * offset;
        if (board->isBoardPositionInvalid(targetPosition))
            continue;
        if (targetPosition != startBoardPosition && board->isBoardPositionOccupied(targetPosition))
            continue;
        Field *targetField = board->getFieldByBoardPosition(targetPosition);
        StepTree *targetStep = new StepTree();
        targetStep->parent = currentStep;
        targetStep->field = targetField;
        targetStep->capture = attackPiece;
        currentCaptures.insert(attackPiece);
        currentStep->nextStepList.push_back(targetStep);
        addPossibleManJumps(targetStep, currentCaptures, startPiece);
        currentCaptures.erase(attackPiece);
    }
}

void Draughts::addPossibleKingJumps(StepTree *currentStep, std::set<Piece *> &currentCaptures, Piece *startPiece)
{
    Player::Color startColor = startPiece->getColor();
    Field *startField = startPiece->getCurrentField();
    sf::Vector2i lastBoardPosition = currentStep->field->getBoardPostion(),
                 startBoardPosition = startField->getBoardPostion();
    bool break2 = false;
    for (const sf::Vector2i &offset : AllOffsets) {
        for (int i = 1; i < sideFieldsNumber; ++i) {
            sf::Vector2i attackPosition = lastBoardPosition + i * offset;
            if (board->isBoardPositionInvalid(attackPosition))
                break;
            if (!board->isBoardPositionOccupied(attackPosition))
                continue;
            Field *attackField = board->getFieldByBoardPosition(attackPosition);
            Piece *attackPiece = attackField->getCurrentPiece();
            if (std::find(currentCaptures.begin(), currentCaptures.end(), attackPiece) != currentCaptures.end())
                break;
            Player::Color attackColor = attackPiece->getColor();
            if (attackColor == startColor)
                break;
            for (int j = i + 1; j < sideFieldsNumber; ++j) {
                sf::Vector2i targetPosition = lastBoardPosition + j * offset;
                if (board->isBoardPositionInvalid(targetPosition)) {
                    break2 = true;
                    break;
                }
                if (targetPosition != startBoardPosition && board->isBoardPositionOccupied(targetPosition)) {
                    break2 = true;
                    break;
                }
                Field *targetField = board->getFieldByBoardPosition(targetPosition);
                StepTree *targetStep = new StepTree();
                targetStep->parent = currentStep;
                targetStep->field = targetField;
                targetStep->capture = attackPiece;
                currentCaptures.insert(attackPiece);
                currentStep->nextStepList.push_back(targetStep);
                addPossibleKingJumps(targetStep, currentCaptures, startPiece);
                currentCaptures.erase(attackPiece);
            }
            if (break2 == true) {
                break2 = false;
                break;
            }
        }
    }
}

void Draughts::narrowTurns()
{
    if (currentTurn.empty()) {
        narrowedTurns = nullptr;
        return;
    }
    Field *currentField = currentTurn.back();
    if (currentTurn.size() == 1U) {
        narrowedTurns = possibleTurns[currentField->getCurrentPiece()].get();
        return;
    }
    for (StepTree *step : narrowedTurns->nextStepList) {
        if (step->field == currentField) {
            narrowedTurns = step;
            return;
        }
    }
}

void Draughts::validatePossibleTurns()
{
    uint16_t maxCaptures = 0U;
    for (std::pair<Piece *, const std::unique_ptr<StepTree> &> turnByPiece : possibleTurns) {
        StepTree *turn = turnByPiece.second.get();
        turn->evalCapturesNumber();
        maxCaptures = std::max(maxCaptures, turn->capturesNumber);
    }
    for (std::pair<Piece *, const std::unique_ptr<StepTree> &> turnByPiece : possibleTurns) {
        StepTree *turn = turnByPiece.second.get();
        turn->validate(maxCaptures);
    }
}

bool Draughts::isPossibleTurn() const
{
    for (std::pair<Piece *, const std::unique_ptr<StepTree> &> turnByPiece : possibleTurns) {
        const std::list<StepTree *> &nextStepList = turnByPiece.second->nextStepList;
        for (StepTree *nextStep : nextStepList) {
            if (nextStep->isValid) {
                return true;
            }
        }
    }
    return false;
}

void Draughts::highlightPossibleSteps()
{
    board->clearSelection();
    if (currentTurn.size() <= 0U) {
        for (std::pair<Piece *, const std::unique_ptr<StepTree> &> turnByPiece : possibleTurns) {
            StepTree *firstStep = turnByPiece.second.get();
            if (!firstStep->isValid)
                continue;
            firstStep->field->setHighlight(Field::Highlight::AvailablePiece);
        }
    }
    if (currentTurn.empty())
        return;
    Field *currentField = currentTurn.back();
    currentField->setHighlight(Field::Highlight::Selected);
    for (StepTree *step : narrowedTurns->nextStepList) {
        if (!step->isValid)
            continue;
        step->field->setHighlight(Field::Highlight::AvailableMove);
    }
}

void Draughts::changeTurn()
{
    selectedPiece = nullptr;
    currentPlayer->turnFinished();
    if (currentPlayer == player1.get())
        currentPlayer = player2.get();
    else
        currentPlayer = player1.get();
    currentTurn.clear();
    narrowedTurns = nullptr;
    clearPossibleTurns();
    addPossibleTurns();
}

void Draughts::removeCapturesFromBoard()
{
    StepTree *iter = narrowedTurns;
    for (; iter != nullptr; iter = iter->parent) {
        if (iter->capture == nullptr)
            continue;
        removeCapture(iter->capture);
    }
}

void Draughts::removeCapture(Piece *capture)
{
    captures.push_back(capture);
    Field *field = capture->getCurrentField();
    field->setCurrentPiece(nullptr);
    capture->setCurrentField(nullptr);
    float viewWidth = gameplayView.getSize().x;
    float boardWidth = board->getSize().x;
    float oneSideSpaceWidth = (viewWidth - boardWidth) / 2.0f;
    float boardTop = 0.0f;
    float boardBottom = board->getSize().y;
    float minX = boardWidth + Resources::BoardBorderSize - Resources::FieldMarginThickness,
          maxX = boardWidth + oneSideSpaceWidth - 1.0f + Resources::FieldMarginThickness,
          minY = boardTop - Resources::FieldMarginThickness,
          maxY = boardBottom - 1.0f + Resources::FieldMarginThickness;
    std::uniform_real_distribution<float> xRandomizer(minX, maxX);
    std::uniform_real_distribution<float> yRandomizer(minY, maxY);
    capture->transistToPosition(
        xRandomizer(Resources::RandomNumberGenerator), yRandomizer(Resources::RandomNumberGenerator), gameTime);
}

void Draughts::promoteIfPossible()
{
    uint16_t promotionLine = (currentPlayer->getColor() == Player::Color::White ? sideFieldsNumber - 1U : 0U);
    Field *targetField = currentTurn.back();
    Piece *movedPiece = targetField->getCurrentPiece();
    if (targetField->getBoardPostion().y == promotionLine) {
        movedPiece->upgrade();
    }
}

void Draughts::playerWon()
{
    gameOver = true;
    hideContinueEntry();

    winner = player1.get();
    if (currentPlayer == player1.get()) {
        winner = player2.get();
    }
    if (winner->getColor() == Player::Color::White) {
        setBannerText(L"Biały gracz wygrał!");
    } else {
        setBannerText(L"Czarny gracz wygrał!");
    }
    currentMenu = mainMenu.get();
    gameState = GameState::Menu;
}

void Draughts::selectPiece()
{
    selectedPiece = currentPlayer->getTurnPiece(possibleTurns);
    if (selectedPiece != nullptr) {
        Field *selectedField = selectedPiece->getCurrentField();
        currentTurn.push_back(selectedField);
    }
}

void Draughts::makeStep()
{
    StepTree *nextStep = currentPlayer->nextStep(selectedPiece, narrowedTurns);
    if (nextStep != nullptr) {
        Field *earlierSelectedField = currentTurn.back();
        Field *nextField = nextStep->field;

        selectedPiece->setCurrentField(nextField);
        earlierSelectedField->setCurrentPiece(nullptr);
        nextField->setCurrentPiece(selectedPiece);
        selectedPiece->transistToPosition(nextField->getPosition(), gameTime);

        currentTurn.push_back(nextField);
        narrowTurns();
        if (narrowedTurns->nextStepList.empty()) {
            removeCapturesFromBoard();
            promoteIfPossible();
            changeTurn();
        }
    }
}

void Draughts::setBannerText(const std::wstring &text)
{
    bannerText.setString(text);
    sf::FloatRect bannerRect = bannerText.getGlobalBounds();
    sf::Vector2f viewCenter = hudTexture.getView().getCenter();
    bannerText.setPosition(viewCenter.x - bannerRect.width / 2.0f, viewCenter.y - 1.5f * bannerRect.height / 2.0f);
}

void Draughts::clearBannerText()
{
    bannerText.setString(std::wstring());
}

void Draughts::showContinueEntry()
{
    gameStarted = true;
    mainMenu->showEntryByName(L"Kontynuuj");
}

void Draughts::hideContinueEntry()
{
    gameStarted = false;
    mainMenu->hideEntryByName(L"Kontynuuj");
}

void Draughts::quit()
{
    isFinished_ = true;
}
