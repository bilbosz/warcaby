#include "Draughts.h"
#include "Macros.h"
#include "NotImplementedError.h"

#include <functional>
#include <iostream>

#include "boost\bind.hpp"
#include "boost\format.hpp"

const sf::Vector2i Draughts::allOffsets[Draughts::allOffsetsNumber] = {
    sf::Vector2i(-1, 1), sf::Vector2i(1, 1), sf::Vector2i(1, -1), sf::Vector2i(-1, -1)
};

Draughts::Draughts() :
    rng(std::random_device()()),
    renderWindow(),
    view(),
    viewLeftTopCorner(),
    overlayText(),
    fontScaleFactor(),
    isFinished_(false),
    gameOver(false),
    winner(),
    sideFieldsNumber(8U),
    board(8U),
    playerPiecesRowsNumber(3U),
    piecesNumber(playerPiecesRowsNumber * 2 * sideFieldsNumber / 2),
    pieces(),
    fieldMargin(0.1f),
    whoseTurn(Player::Color::White),
    currentTurn(),
    possibleTurns(),
    narrowedTurns()
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
    sf::VideoMode videoMode = sf::VideoMode::getFullscreenModes()[0];
    std::string title = "Warcaby";
    sf::ContextSettings settings;
    settings.antialiasingLevel = 0U;

    fontScaleFactor = 10.0f / videoMode.height;

    view.setCenter(4.0f, 4.0f);
    view.setSize(fontScaleFactor * videoMode.width, 10.0f);
    view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
    viewLeftTopCorner.x = view.getCenter().x - view.getSize().x / 2.0f;
    viewLeftTopCorner.y = view.getCenter().y - view.getSize().y / 2.0f;

    renderWindow.setVerticalSyncEnabled(true);
    renderWindow.setKeyRepeatEnabled(true);
    renderWindow.setMouseCursorGrabbed(false);

    board.init();
    board.setPosition(0.0f, 0.0f);
    board.setFontScaleFactor(fontScaleFactor);

    std::list<Field *> blackFields = board.getFieldsByColor(Field::Color::Black);
    for (uint16_t i = 0; i < piecesNumber; ++i) {
        Piece *piece = pieces[i];
        piece->init();
        piece->setFieldMargin(fieldMargin);
        piece->setFontScaleFactor(fontScaleFactor);
        if (blackFields.empty())
            throw Error("Bierek jest więcej niż pozycji");
        sf::Vector2f fieldPosition;
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
        fieldPosition = field->getPosition();
        piece->setPosition(fieldPosition);
    }

    overlayText.setPosition(viewLeftTopCorner);
    overlayText.setFont(Resources::DefaultFont);
    overlayText.setFillColor(sf::Color::Blue);
    overlayText.setScale(fontScaleFactor, fontScaleFactor);

    addPossibleTurns();

    // okno tworzymy jak najpóźniej, ustawiamy widok po stworzeniu okna
    renderWindow.create(videoMode, title, sf::Style::Fullscreen, settings);
    renderWindow.setView(view);
}

void Draughts::update(sf::Time advance)
{
    retrieveEvents();
    if (gameOver) {
        if (winner == Player::Color::White)
            overlayText.setString(std::wstring(L"Biały gracz wygrał. Gratulacje!\n"));
        else
            overlayText.setString(std::wstring(L"Czarny gracz wygrał. Gratulacje!\n"));
    } else {
        // overlayText.setString(gameMessages.str());
    }
}

void Draughts::render()
{
    renderWindow.clear(sf::Color(230U, 230U, 230U));
    renderWindow.draw(board);
    for (uint16_t i = 0U; i < piecesNumber; ++i) {
        renderWindow.draw(*pieces[i]);
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
        Field *earlierSelectedField = *currentTurn.stepList.rbegin();
        Piece *earlierSelectedPiece = earlierSelectedField->getCurrentPiece();
        // przemieść bierkę
        earlierSelectedPiece->setCurrentField(pressedField);
        earlierSelectedField->setCurrentPiece(nullptr);
        pressedField->setCurrentPiece(earlierSelectedPiece);
        earlierSelectedPiece->setPosition(pressedField->getPosition());
        // zaznacz pole
        currentTurn.stepList.push_back(pressedField);
        narrowTurns();
        if (narrowedTurns.size() == 1U && narrowedTurns.begin()->stepList == currentTurn.stepList) {
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
        if (currentTurn.stepList.size() >= 2U)
            return;
        board.clearSelection();
        currentTurn.reset();
        currentTurn.stepList.push_back(pressedField);
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
        addPossiblePieceMoves(piece);
        addPossiblePieceJumps(piece);
    }
    if (possibleTurns.empty()) {
        winner = Player::Color::White;
        if (whoseTurn == Player::Color::White)
            winner = Player::Color::Black;
        playerWon();
        return;
    }
    filterMaxCaptures();
    std::copy(possibleTurns.begin(), possibleTurns.end(), std::inserter(narrowedTurns, narrowedTurns.end()));
}

void Draughts::clearPossibleTurns()
{
    possibleTurns.clear();
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
    if (pieceType == Piece::PieceType::Man) {
        const size_t forwardMovesOffsetsNumber = 2U;
        const sf::Vector2i forwardMovesOffsets[forwardMovesOffsetsNumber] = { sf::Vector2i(-1, forwardDirection),
                                                                              sf::Vector2i(1, forwardDirection) };
        std::list<sf::Vector2i> forwardMoves;
        foreach (const sf::Vector2i &offset in forwardMovesOffsets) {
            sf::Vector2i targetBoardPosition(boardPosition + offset);
            if (board.isBoardPositionInvalid(targetBoardPosition))
                continue;
            if (board.isBoardPositionOccupied(targetBoardPosition))
                continue;
            Field *targetField = board.getFieldByBoardPosition(targetBoardPosition);
            Turn turn;
            turn.stepList.push_back(field);
            turn.stepList.push_back(targetField);
            possibleTurns.insert(turn);
        }
    } else if (pieceType == Piece::PieceType::King) {
        std::list<sf::Vector2i> allDirectionMoves;
        foreach (const sf::Vector2i &directionOffset in allOffsets) {
            for (int i = 1; i <= sideFieldsNumber; ++i) {
                sf::Vector2i targetBoardPosition(boardPosition + i * directionOffset);
                if (board.isBoardPositionInvalid(targetBoardPosition))
                    break;
                if (board.isBoardPositionOccupied(targetBoardPosition))
                    break;
                allDirectionMoves.push_back(targetBoardPosition);
            }
        }
        foreach (const sf::Vector2i &move in allDirectionMoves) {
            if (board.isBoardPositionOccupied(move))
                continue;
            Field *targetField = board.getFieldByBoardPosition(move);
            Turn turn;
            turn.stepList.push_back(field);
            turn.stepList.push_back(targetField);
            possibleTurns.insert(turn);
        }
    }
}

void Draughts::addPossiblePieceJumps(Piece *piece)
{
    Field *field = piece->getCurrentField();
    Piece::PieceType pieceType = piece->getPieceType();
    Turn turn;
    turn.stepList.push_back(field);
    if (pieceType == Piece::PieceType::Man) {
        addPossibleManJumps(field, field, turn);
    } else if (pieceType == Piece::PieceType::King) {
        addPossibleKingJumps(field, field, turn);
    }
}

void Draughts::addPossibleManJumps(Field *startField, Field *lastField, Turn &turnByFar)
{
    Piece *startPiece = startField->getCurrentPiece();
    sf::Vector2i lastBoardPosition = lastField->getBoardPostion(), startBoardPosition = startField->getBoardPostion();
    Player::Color startColor = startPiece->getColor();
    bool foundCapture = false;
    foreach (const sf::Vector2i &offset in allOffsets) {
        sf::Vector2i attackPosition = lastBoardPosition + offset;
        if (board.isBoardPositionInvalid(attackPosition))
            continue;
        if (!board.isBoardPositionOccupied(attackPosition))
            continue;
        Field *attackField = board.getFieldByBoardPosition(attackPosition);
        Piece *attackPiece = attackField->getCurrentPiece();
        if (std::find(turnByFar.capturesList.begin(), turnByFar.capturesList.end(), attackPiece) !=
            turnByFar.capturesList.end())
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
        turnByFar.capturesNumber += 1U;
        turnByFar.capturesList.push_back(attackPiece);
        turnByFar.stepList.push_back(targetField);
        addPossibleManJumps(startField, targetField, turnByFar);
        turnByFar.capturesNumber -= 1U;
        turnByFar.capturesList.pop_back();
        turnByFar.stepList.pop_back();
        foundCapture = true;
    }
    if (foundCapture)
        return;
    if (turnByFar.capturesNumber == 0U)
        return;
    possibleTurns.insert(turnByFar);
}

void Draughts::addPossibleKingJumps(Field *startField, Field *lastField, Turn &turnByFar)
{
    Piece *startPiece = startField->getCurrentPiece();
    sf::Vector2i lastBoardPosition = lastField->getBoardPostion(), startBoardPosition = startField->getBoardPostion();
    Player::Color startColor = startPiece->getColor();
    bool foundCapture = false;
    bool break2 = false;
    foreach (const sf::Vector2i &offset in allOffsets) {
        for (int i = 1; i < sideFieldsNumber; ++i) {
            sf::Vector2i attackPosition = lastBoardPosition + i * offset;
            if (board.isBoardPositionInvalid(attackPosition))
                break;
            if (!board.isBoardPositionOccupied(attackPosition))
                continue;
            Field *attackField = board.getFieldByBoardPosition(attackPosition);
            Piece *attackPiece = attackField->getCurrentPiece();
            if (std::find(turnByFar.capturesList.begin(), turnByFar.capturesList.end(), attackPiece) !=
                turnByFar.capturesList.end())
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
                turnByFar.capturesNumber += 1U;
                turnByFar.capturesList.push_back(attackPiece);
                turnByFar.stepList.push_back(targetField);
                addPossibleManJumps(startField, targetField, turnByFar);
                turnByFar.capturesNumber -= 1U;
                turnByFar.capturesList.pop_back();
                turnByFar.stepList.pop_back();
                foundCapture = true;
            }
            if (break2 == true) {
                break2 = false;
                break;
            }
        }
    }
    if (foundCapture)
        return;
    if (turnByFar.capturesNumber == 0U)
        return;
    possibleTurns.insert(turnByFar);
}

void Draughts::narrowTurns()
{
    narrowedTurns.clear();
    foreach (const Turn &turn in possibleTurns) {
        std::multiset<Turn>::iterator iter = possibleTurns.begin();
        if (isSubturn(turn, currentTurn))
            narrowedTurns.insert(turn);
    }
}

void Draughts::filterMaxCaptures()
{
    uint16_t maxCaptures = 0;
    foreach (const Turn &turn in possibleTurns) {
        if (turn.capturesNumber > maxCaptures)
            maxCaptures = turn.capturesNumber;
    }
    for (std::multiset<Turn>::iterator iter = possibleTurns.begin(); iter != possibleTurns.end();) {
        if (iter->capturesNumber == maxCaptures) {
            ++iter;
            continue;
        }
        iter = possibleTurns.erase(iter);
    }
}

bool Draughts::isSubturn(const Turn &turn, const Turn &possibleSubturn) const
{
    const std::list<Field *> &stepList = turn.stepList, &subStepList = possibleSubturn.stepList;
    std::list<Field *>::const_iterator iter = stepList.cbegin(), subIter = subStepList.cbegin();
    while (subIter != subStepList.cend()) {
        if ((*iter) != (*subIter))
            return false;
        ++subIter;
        ++iter;
    }
    return true;
}

void Draughts::highlightPossibleSteps()
{
    (*currentTurn.stepList.rbegin())->setHighlight(Field::Highlight::Selected);
    foreach (Turn turn in narrowedTurns) {
        std::list<Field *>::const_iterator iter = turn.stepList.cbegin();
        std::advance(iter, currentTurn.stepList.size());
        if (iter == turn.stepList.cend())
            continue;
        (*iter)->setHighlight(Field::Highlight::AvailableMove);
    }
}

void Draughts::changeTurn()
{
    if (whoseTurn == Player::Color::White)
        whoseTurn = Player::Color::Black;
    else
        whoseTurn = Player::Color::White;
    currentTurn.reset();
}

void Draughts::removeCapturesFromBoard()
{
    foreach (Piece *piece in narrowedTurns.begin()->capturesList) {
        Field *field = piece->getCurrentField();
        field->setCurrentPiece(nullptr);
        piece->setCurrentField(nullptr);
        std::uniform_real_distribution<float> xRandomizer(0.0f, 1.0f);
        std::uniform_real_distribution<float> yRandomizer(-static_cast<float>(sideFieldsNumber) / 4.0f,
                                                          +static_cast<float>(sideFieldsNumber) / 4.0f);
        piece->setPosition(static_cast<float>(sideFieldsNumber) + 1.0f + xRandomizer(rng),
                           static_cast<float>(sideFieldsNumber) / 2.0f + yRandomizer(rng));
        // piece->hide();
    }
}

void Draughts::promoteIfPossible()
{
    uint16_t promotionLine = (whoseTurn == Player::White ? sideFieldsNumber - 1U : 0U);
    Field *targetField = (*currentTurn.stepList.rbegin());
    Piece *movedPiece = targetField->getCurrentPiece();
    if (targetField->getBoardPostion().y == promotionLine)
        movedPiece->promote();
}

void Draughts::playerWon()
{
    gameOver = true;
    if (winner == Player::Color::White) {
        // \TODO pokazać jakiś tekst
    } else {
    }
}
