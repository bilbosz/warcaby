#include "Board.h"
#include "Resources.h"

Board::Board(uint16_t sideFieldNumber) :
    GameObject(),
    sideFieldsNumber(sideFieldNumber),
    boardShape(4U),
    fields(),
    validFields(0, 0, sideFieldNumber, sideFieldNumber),
    fieldMargin(Resources::FieldMarginThickness)
{
    fields = new Field **[sideFieldNumber];
    for (uint16_t y = 0U; y < sideFieldNumber; ++y) {
        fields[y] = new Field *[sideFieldNumber];
        for (uint16_t x = 0U; x < sideFieldNumber; ++x) {
            Field::Color color = static_cast<Field::Color>((1 + x + y) % 2);
            fields[y][x] = new Field(x, y, color);
        }
    }
}

Board::~Board()
{
    for (uint16_t y = 0U; y < sideFieldsNumber; ++y) {
        for (uint16_t x = 0U; x < sideFieldsNumber; ++x) {
            delete fields[y][x];
        }
        delete[] fields[y];
    }
    delete[] fields;
}

void Board::init()
{
    size.x = static_cast<float>(sideFieldsNumber);
    size.y = static_cast<float>(sideFieldsNumber);

    float borderSize = Resources::BoardBorderSize;
    boardShape.setPoint(0U, sf::Vector2f(-borderSize, -borderSize));
    boardShape.setPoint(1U, sf::Vector2f(size.x + borderSize, -borderSize));
    boardShape.setPoint(2U, sf::Vector2f(size.x + borderSize, size.y + borderSize));
    boardShape.setPoint(3U, sf::Vector2f(-borderSize, size.y + borderSize));

    boardShape.setFillColor(Resources::BoardColor);

    for (uint16_t y = 0U; y < sideFieldsNumber; ++y) {
        for (uint16_t x = 0U; x < sideFieldsNumber; ++x) {
            Field *field = fields[y][x];
            field->init();
            field->Field::setPosition(static_cast<float>(x), static_cast<float>(sideFieldsNumber - 1 - y));
            field->setFrameThickness(fieldMargin);
        }
    }
}

void Board::update(sf::Time time)
{
    for (uint16_t y = 0U; y < sideFieldsNumber; ++y) {
        for (uint16_t x = 0U; x < sideFieldsNumber; ++x) {
            Field *field = fields[y][x];
            field->update(time);
        }
    }
}

void Board::draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const
{
    renderTarget.draw(boardShape);
    for (uint16_t y = 0U; y < sideFieldsNumber; ++y) {
        for (uint16_t x = 0U; x < sideFieldsNumber; ++x) {
            Field *field = fields[y][x];
            renderTarget.draw(*field);
        }
    }
}

void Board::setFontScaleFactor(float scale)
{
    fontScaleFactor = scale;
    for (uint16_t y = 0U; y < sideFieldsNumber; ++y) {
        for (uint16_t x = 0U; x < sideFieldsNumber; ++x) {
            fields[y][x]->setFontScaleFactor(fontScaleFactor);
        }
    }
}

std::list<Field *> Board::getFieldsByColor(Field::Color color) const
{
    std::list<Field *> resultList;
    for (uint16_t y = 0U; y < sideFieldsNumber; ++y) {
        for (uint16_t x = 0U; x < sideFieldsNumber; ++x) {
            Field *field = fields[y][x];
            if (field->getColor() == color)
                resultList.push_back(field);
        }
    }
    return resultList;
}

Field *Board::getFieldByPosition(const sf::Vector2f &position) const
{
    sf::Rect<float> boardRect(0.0f, 0.0f, static_cast<float>(sideFieldsNumber), static_cast<float>(sideFieldsNumber));
    if (!boardRect.contains(position))
        return nullptr;
    return fields[sideFieldsNumber - 1 - static_cast<uint16_t>(position.y)][static_cast<uint16_t>(position.x)];
}

Field *Board::getFieldByBoardPosition(const sf::Vector2i &position) const
{
    return fields[position.y][position.x];
}

Field *Board::getFieldByName(const std::string &name) const
{
    for (uint16_t y = 0U; y < sideFieldsNumber; ++y) {
        for (uint16_t x = 0U; x < sideFieldsNumber; ++x) {
            Field *field = fields[y][x];
            if (field->getName() == name)
                return field;
        }
    }
    return nullptr;
}

void Board::clearSelection()
{
    for (uint16_t y = 0U; y < sideFieldsNumber; ++y) {
        for (uint16_t x = 0U; x < sideFieldsNumber; ++x) {
            Field *field = fields[y][x];
            field->setHighlight(Field::Highlight::None);
        }
    }
}

bool Board::isBoardPositionValid(const sf::Vector2i &position) const
{
    return validFields.contains(position);
}

bool Board::isBoardPositionInvalid(const sf::Vector2i &position) const
{
    return !validFields.contains(position);
}

bool Board::isBoardPositionOccupied(const sf::Vector2i &position) const
{
    return fields[position.y][position.x]->getCurrentPiece() != nullptr;
}
