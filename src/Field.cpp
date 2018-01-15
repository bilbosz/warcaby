#include "Field.h"
#include "Resources.h"

#include <boost/format.hpp>

Field::Field(uint16_t x, uint16_t y, Color color) :
    GameObject(),
    boardPosition(x, y),
    fieldName(),
    color(color),
    fieldShape(4U),
    highlight(None),
    descriptionText(),
    frameShape(4U),
    frameThickness(Resources::FieldMarginThickness),
    currentPiece(nullptr)
{
}

Field::~Field() {}

void Field::init()
{
    size.x = 1.0f;
    size.y = 1.0f;

    fieldShape.setPoint(0U, sf::Vector2f(0.0f, 0.0f));
    fieldShape.setPoint(1U, sf::Vector2f(1.0f, 0.0f));
    fieldShape.setPoint(2U, sf::Vector2f(1.0f, 1.0f));
    fieldShape.setPoint(3U, sf::Vector2f(0.0f, 1.0f));

    sf::Color color = Resources::WhiteFieldColor;
    if (this->color == Color::Black)
        color = Resources::BlackFieldColor;
    fieldShape.setFillColor(color);

    fieldName = (boost::format("%c%d") % char(uint16_t('a') + boardPosition.x) % (boardPosition.y + 1)).str();

    descriptionText.setFont(Resources::InfoFont);
    descriptionText.setFillColor(Resources::InfoTextColor);
    descriptionText.setString(fieldName);

    frameShape.setPoint(0U, sf::Vector2f(frameThickness, frameThickness));
    frameShape.setPoint(1U, sf::Vector2f(1.0f - frameThickness, frameThickness));
    frameShape.setPoint(2U, sf::Vector2f(1.0f - frameThickness, 1.0f - frameThickness));
    frameShape.setPoint(3U, sf::Vector2f(frameThickness, 1.0f - frameThickness));
    frameShape.setFillColor(sf::Color::Transparent);
    frameShape.setOutlineThickness(frameThickness);
    frameShape.setOutlineColor(sf::Color::Transparent);
}

void Field::update(sf::Time time) {}

void Field::draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const
{
    renderTarget.draw(fieldShape, states);
    renderTarget.draw(frameShape, states);
}

void Field::setPosition(const sf::Vector2f &position)
{
    fieldShape.setPosition(position);
    frameShape.setPosition(position.x, position.y);
    descriptionText.setPosition(position);
    Transformable::setPosition(position);
}

void Field::setPosition(float x, float y)
{
    fieldShape.setPosition(x, y);
    frameShape.setPosition(x, y);
    descriptionText.setPosition(x, y);
    Transformable::setPosition(x, y);
}

void Field::setFontScaleFactor(float scale)
{
    descriptionText.scale(scale, scale);
    fontScaleFactor = scale;
}

void Field::setHighlight(Highlight highlight)
{
    this->highlight = highlight;
    if (highlight == None) {
        frameShape.setOutlineColor(sf::Color::Transparent);
    } else if (highlight == Selected) {
        frameShape.setOutlineColor(Resources::SelectedFieldColor);
    } else if (highlight == AvailableMove) {
        frameShape.setOutlineColor(Resources::AvailableFieldColor);
    } else if (highlight == AvailablePiece) {
        frameShape.setOutlineColor(Resources::AvailablePieceFieldColor);
    }
}

Field::Highlight Field::getHighlight() const
{
    return highlight;
}

sf::Rect<float> Field::getBoundaries() const
{
    sf::Vector2f pos = getPosition();
    return sf::Rect<float>(pos.x, pos.y, size.x, size.y);
}

const std::string &Field::getName() const
{
    return fieldName;
}

Field::Color Field::getColor() const
{
    return color;
}

void Field::setFrameThickness(float thickness)
{
    frameShape.setOutlineThickness(thickness);
    frameThickness = thickness;
}

float Field::getFrameThickness() const
{
    return frameThickness;
}

void Field::setCurrentPiece(Piece *piece)
{
    currentPiece = piece;
}

Piece *Field::getCurrentPiece() const
{
    return currentPiece;
}

sf::Vector2i Field::getBoardPostion() const
{
    return boardPosition;
}
