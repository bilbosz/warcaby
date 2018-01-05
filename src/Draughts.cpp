#include "Draughts.h"
#include "Error.h"

#include <algorithm>
#include <fstream>

#include "boost\tokenizer.hpp"

const sf::Vector2i Draughts::AllOffsets[Draughts::AllOffsetsNumber] = {
    sf::Vector2i(-1, 1), sf::Vector2i(1, 1), sf::Vector2i(1, -1), sf::Vector2i(-1, -1)
};

Draughts::Draughts() :
    renderWindow(),
    view(),
    viewLeftTopCorner(),
    overlayText(),
    fontScaleFactor(),
    isFinished_(false),
    gameOver(false),
    winner(),
    sideFieldsNumber(Resources::SideFieldsNumber),
    board(sideFieldsNumber),
    playerPiecesRowsNumber(Resources::PlayerPiecesRowsNumber),
    piecesNumber(playerPiecesRowsNumber * 2 * sideFieldsNumber / 2),
    pieces(),
    fieldMargin(Resources::FieldMarginThickness),
    whoseTurn(Player::Color::White),
    currentTurn(),
    possibleTurns(),
    narrowedTurns(),
    captures()
{
    pieces = new Piece *[piecesNumber];
    for (uint16_t i = 0U; i < piecesNumber; ++i) {
        Player::Color color = Player::Color::White;
        if (i >= piecesNumber / 2)
            color = Player::Color::Black;
        pieces[i] = new Piece(i, color);
    }
}

Draughts::~Draughts()
{
    for (uint16_t i = 0U; i < piecesNumber; ++i) {
        delete pieces[i];
    }
    delete[] pieces;
}

void Draughts::init()
{
    sf::VideoMode videoMode = sf::VideoMode::getFullscreenModes()[Resources::VideoModeNumber];
    std::string title = Resources::GameTitle;
    sf::ContextSettings settings;
    settings.antialiasingLevel = Resources::AntialiasingLevel;
    renderWindow.setVerticalSyncEnabled(Resources::VerticalSync);
    renderWindow.setKeyRepeatEnabled(Resources::KeyRepeat);
    renderWindow.setMouseCursorGrabbed(Resources::MouseCursorGrab);
	renderWindow.setFramerateLimit(Resources::UpdateRate);

    float viewHeight = static_cast<float>(sideFieldsNumber) + 2.0f;

    fontScaleFactor = viewHeight / videoMode.height;

    view.setCenter((viewHeight - 2.0f) / 2.0f, (viewHeight - 2.0f) / 2.0f);
    view.setSize(fontScaleFactor * videoMode.width, viewHeight);
    view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
    viewLeftTopCorner.x = view.getCenter().x - view.getSize().x / 2.0f;
    viewLeftTopCorner.y = view.getCenter().y - view.getSize().y / 2.0f;

    board.init();
    board.setPosition(0.0f, 0.0f);
    board.setFontScaleFactor(fontScaleFactor);

    std::list<Field *> blackFields = board.getFieldsByColor(Field::Color::Black);
    for (uint16_t i = 0U; i < piecesNumber; ++i) {
        Piece *piece = pieces[i];
        piece->init();
        piece->setFieldMargin(fieldMargin);
        piece->setFontScaleFactor(fontScaleFactor);
    }

    setPiecesPosition(Resources::InitialBoardFile);

    overlayText.setPosition(viewLeftTopCorner);
    overlayText.setFont(Resources::DefaultFont);
    overlayText.setFillColor(Resources::DebugTextColor);
    overlayText.setScale(fontScaleFactor, fontScaleFactor);
    overlayText.setCharacterSize(60);

    addPossibleTurns();

    // okno tworzymy jak najpóźniej, ustawiamy widok po stworzeniu okna
    renderWindow.create(videoMode, title, Resources::WindowStyle, settings);
    renderWindow.setView(view);
}

void Draughts::update(sf::Time time)
{
    Game::update(time);
    retrieveEvents();
    board.update(time);
    for (uint16_t i = 0U; i < piecesNumber; ++i) {
        Piece *piece = pieces[i];
        piece->update(time);
    }
    if (gameOver) {
        if (winner == Player::Color::White)
            overlayText.setString(L"Biały gracz wygrał. Gratulacje!\n");
        else
            overlayText.setString(L"Czarny gracz wygrał. Gratulacje!\n");
    }
}

void Draughts::render()
{
    renderWindow.clear(Resources::BackgroundColor);
    renderWindow.draw(board);
    // wszystkie zbite bierki pod spodem
    for (Piece *capture : captures) {
        renderWindow.draw(*capture);
    }
    // wszystkie bierki, które się nie ruszają
    for (uint16_t i = 0U; i < piecesNumber; ++i) {
        Piece *piece = pieces[i];
        // bierka jest zbita
        if (piece->getCurrentField() == nullptr)
            continue;
        if (piece->isMovingTransitionRunning())
            continue;
        renderWindow.draw(*piece);
    }
    // bierki animowane na górze (wyświetlane ostatnie)
    for (uint16_t i = 0U; i < piecesNumber; ++i) {
        Piece *piece = pieces[i];
        // bierka jest zbita
        if (piece->getCurrentField() == nullptr)
            continue;
        if (!piece->isMovingTransitionRunning())
            continue;
        renderWindow.draw(*piece);
    }
    renderWindow.draw(overlayText);
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

void Draughts::setPiecesPosition(std::experimental::filesystem::path file)
{
    std::list<Field *> blackFields = board.getFieldsByColor(Field::Color::Black);
    for (uint16_t i = 0U; i < piecesNumber; ++i) {
        Piece *piece = pieces[i];
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
        field->setCurrentPiece(piece);
        piece->setPosition(field->getPosition());
    }
    if (file == std::experimental::filesystem::path())
        return;
    for (uint16_t i = 0U; i < piecesNumber; ++i) {
        Piece *piece = pieces[i];
        removeCapture(piece);
    }
    // działa tylko dla poprawnego tagu FEN pliku PDN, dla niepoprawnego mamy zachowanie niezdefiniowane
    std::ifstream fileStream(file.string());
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    std::string initialBoardContent(buffer.str());
    std::transform(initialBoardContent.cbegin(), initialBoardContent.cend(), initialBoardContent.begin(), std::tolower);
    boost::char_separator<char> separators(",:");
    boost::tokenizer<boost::char_separator<char>> tokenizer(initialBoardContent, separators);
    uint16_t pieceNumber = 0U;
    for (const std::string &token : tokenizer) {
        Player::Color settingFor;
        bool king = false, innerContinue = false;
        if (token == "w") {
            whoseTurn = Player::Color::White;
            continue;
        }
        if (token == "b") {
            whoseTurn = Player::Color::Black;
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
        Field *field = board.getFieldByName(fieldName);
        Piece *piece = pieces[pieceNumber];
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
            isFinished_ = true;
        } else if (event.type == sf::Event::EventType::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                isFinished_ = true;
            }
        }
        if (gameOver)
            continue;
        if (event.type == sf::Event::EventType::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f pressPoint =
                    renderWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                mouseClickedOnBoard(pressPoint);
            }
        }
    }
}

void Draughts::mouseClickedOnBoard(const sf::Vector2f &pressPoint)
{
    Field *pressedField = board.getFieldByPosition(pressPoint);
    // przyciśnięcie poza planszą
    if (pressedField == nullptr)
        return;
    Field::Highlight pressedHighlight = pressedField->getHighlight();
    // pole zaznaczone jako możliwe do wykonania ruchu
    if (pressedHighlight == Field::Highlight::AvailableMove) {
        // wyczyść zaznaczenie
        board.clearSelection();
        // wykonaj ruch
        Field *earlierSelectedField = *currentTurn.rbegin();
        Piece *earlierSelectedPiece = earlierSelectedField->getCurrentPiece();
        // przemieść bierkę
        earlierSelectedPiece->setCurrentField(pressedField);
        earlierSelectedField->setCurrentPiece(nullptr);
        pressedField->setCurrentPiece(earlierSelectedPiece);
        earlierSelectedPiece->transistToPosition(pressedField->getPosition(), gameTime);
        // zaznacz pole
        currentTurn.push_back(pressedField);
        narrowTurns();
        if (narrowedTurns->nextStepList.empty()) {
            // koniec tury
            removeCapturesFromBoard();
            promoteIfPossible();
            changeTurn();
            clearPossibleTurns();
            addPossibleTurns();
        } else {
            // kontynuacja tury
            highlightPossibleSteps();
        }
        return;
    }
    Piece *pressedPiece = pressedField->getCurrentPiece();
    // brak bierki
    if (pressedPiece == nullptr)
        return;
    Player::Color pieceColor = pressedPiece->getColor();
    // bierka przeciwnika
    if (pieceColor != whoseTurn)
        return;
    // pole nie zaznaczone jako możliwe do wykonania ruchu
    if (pressedHighlight != Field::Highlight::AvailableMove) {
        // jeśli zrobiono ruch
        if (currentTurn.size() >= 2U)
            return;
        board.clearSelection();
        currentTurn.clear();
        currentTurn.push_back(pressedField);
        // zaznacz pole
        narrowTurns();
        highlightPossibleSteps();
        return;
    }
}

void Draughts::addPossibleTurns()
{
    for (uint16_t i = 0U; i < piecesNumber; ++i) {
        Piece *piece = pieces[i];
        // runda przeciwnika
        if (whoseTurn != piece->getColor())
            continue;
        Field *field = piece->getCurrentField();
        // bierka zbita
        if (field == nullptr)
            continue;
        createPossibleEmptyTurn(piece);
        addPossiblePieceMoves(piece);
        addPossiblePieceJumps(piece);
    }
    validatePossibleTurns();
    narrowedTurns = nullptr;
    // \bug jeśli wszystkie bierki nie mają ruchu zakończ
    bool isValidStep = false;
    for (std::pair<Piece *, StepTree *> turnByPiece : possibleTurns) {
        const std::list<StepTree *> &nextStepList = turnByPiece.second->nextStepList;
        for (StepTree *nextStep : nextStepList) {
            if (nextStep->isValid) {
                isValidStep = true;
                break;
            }
        }
        if (isValidStep)
            break;
    }
    if (!isValidStep) {
        winner = Player::Color::White;
        if (whoseTurn == Player::Color::White)
            winner = Player::Color::Black;
        playerWon();
        return;
    }
    highlightPossibleSteps();
}

void Draughts::clearPossibleTurns()
{
    for (std::pair<Piece *, StepTree *> turnByPiece : possibleTurns) {
        delete turnByPiece.second;
    }
    possibleTurns.clear();
}

void Draughts::createPossibleEmptyTurn(Piece *piece)
{
    possibleTurns[piece] = new StepTree();
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
    StepTree *previousStep = possibleTurns[piece];
    if (pieceType == Piece::PieceType::Man) {
        const size_t forwardMovesOffsetsNumber = 2U;
        const sf::Vector2i forwardMovesOffsets[forwardMovesOffsetsNumber] = { sf::Vector2i(-1, forwardDirection),
                                                                              sf::Vector2i(1, forwardDirection) };
        std::list<sf::Vector2i> forwardMoves;
        for (const sf::Vector2i &offset : forwardMovesOffsets) {
            sf::Vector2i targetBoardPosition(boardPosition + offset);
            if (board.isBoardPositionInvalid(targetBoardPosition))
                continue;
            if (board.isBoardPositionOccupied(targetBoardPosition))
                continue;
            Field *targetField = board.getFieldByBoardPosition(targetBoardPosition);
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
                if (board.isBoardPositionInvalid(targetBoardPosition))
                    break;
                if (board.isBoardPositionOccupied(targetBoardPosition))
                    break;
                allDirectionMoves.push_back(targetBoardPosition);
            }
        }
        for (const sf::Vector2i &move : allDirectionMoves) {
            if (board.isBoardPositionOccupied(move))
                continue;
            Field *targetField = board.getFieldByBoardPosition(move);
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
    StepTree *stepTree = possibleTurns[piece];
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
        if (board.isBoardPositionInvalid(attackPosition))
            continue;
        if (!board.isBoardPositionOccupied(attackPosition))
            continue;
        Field *attackField = board.getFieldByBoardPosition(attackPosition);
        Piece *attackPiece = attackField->getCurrentPiece();
        if (currentCaptures.find(attackPiece) != currentCaptures.end())
            continue;
        Player::Color attackColor = attackPiece->getColor();
        if (attackColor == startColor)
            continue;
        sf::Vector2i targetPosition = lastBoardPosition + 2 * offset;
        if (board.isBoardPositionInvalid(targetPosition))
            continue;
        if (targetPosition != startBoardPosition && board.isBoardPositionOccupied(targetPosition))
            continue;
        Field *targetField = board.getFieldByBoardPosition(targetPosition);
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
            if (board.isBoardPositionInvalid(attackPosition))
                break;
            if (!board.isBoardPositionOccupied(attackPosition))
                continue;
            Field *attackField = board.getFieldByBoardPosition(attackPosition);
            Piece *attackPiece = attackField->getCurrentPiece();
            if (std::find(currentCaptures.begin(), currentCaptures.end(), attackPiece) != currentCaptures.end())
                break;
            Player::Color attackColor = attackPiece->getColor();
            if (attackColor == startColor)
                break;
            for (int j = i + 1; j < sideFieldsNumber; ++j) {
                sf::Vector2i targetPosition = lastBoardPosition + j * offset;
                if (board.isBoardPositionInvalid(targetPosition)) {
                    break2 = true;
                    break;
                }
                if (targetPosition != startBoardPosition && board.isBoardPositionOccupied(targetPosition)) {
                    break2 = true;
                    break;
                }
                Field *targetField = board.getFieldByBoardPosition(targetPosition);
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
    Field *currentField = currentTurn.back();
    if (currentTurn.size() == 1U) {
        narrowedTurns = possibleTurns[currentField->getCurrentPiece()];
        return;
    }
    for (StepTree *step : narrowedTurns->nextStepList) {
        if (step->field == currentField) {
            narrowedTurns = step;
            return;
        }
    }
    throw Error("Nie można wykonanać tego kroku, nie ma go na liście możliwych pozycji");
}

void Draughts::validatePossibleTurns()
{
    uint16_t maxCaptures = 0U;
    for (std::pair<Piece *, StepTree *> turnByPiece : possibleTurns) {
        StepTree *turn = turnByPiece.second;
        turn->evalCapturesNumber();
        maxCaptures = std::max(maxCaptures, turn->capturesNumber);
    }
    for (std::pair<Piece *, StepTree *> turnByPiece : possibleTurns) {
        StepTree *turn = turnByPiece.second;
        turn->validate(maxCaptures);
    }
}

void Draughts::highlightPossibleSteps()
{
    if (currentTurn.size() <= 1U) {
        for (std::pair<Piece *, StepTree *> turnByPiece : possibleTurns) {
            StepTree *firstStep = turnByPiece.second;
            bool canMove = false;
            for (StepTree *secondStep : firstStep->nextStepList) {
                if (!secondStep->isValid)
                    continue;
                canMove = true;
            }
            if (!canMove)
                continue;
            firstStep->field->setHighlight(Field::Highlight::AvailablePiece);
        }
    }
    if (currentTurn.empty())
        return;
    Field *currentField = *currentTurn.rbegin();
    currentField->setHighlight(Field::Highlight::Selected);
    for (StepTree *step : narrowedTurns->nextStepList) {
        if (!step->isValid)
            continue;
        step->field->setHighlight(Field::Highlight::AvailableMove);
    }
}

void Draughts::changeTurn()
{
    if (whoseTurn == Player::Color::White)
        whoseTurn = Player::Color::Black;
    else
        whoseTurn = Player::Color::White;
    currentTurn.clear();
    narrowedTurns = nullptr;
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
    std::uniform_real_distribution<float> xRandomizer(0.0f, 1.0f);
    std::uniform_real_distribution<float> yRandomizer(-static_cast<float>(sideFieldsNumber) / 4.0f,
                                                      +static_cast<float>(sideFieldsNumber) / 4.0f);
    capture->transistToPosition(
        static_cast<float>(sideFieldsNumber) + 1.0f + xRandomizer(Resources::RandomNumberGenerator),
        static_cast<float>(sideFieldsNumber) / 2.0f + yRandomizer(Resources::RandomNumberGenerator),
        gameTime);
}

void Draughts::promoteIfPossible()
{
    uint16_t promotionLine = (whoseTurn == Player::White ? sideFieldsNumber - 1U : 0U);
    Field *targetField = (*currentTurn.rbegin());
    Piece *movedPiece = targetField->getCurrentPiece();
    if (targetField->getBoardPostion().y == promotionLine)
        movedPiece->upgrade();
}

void Draughts::playerWon()
{
    gameOver = true;
    if (winner == Player::Color::White) {
        // \TODO pokazać jakiś tekst
    } else {
    }
}
