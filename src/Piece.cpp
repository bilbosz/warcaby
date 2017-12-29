#include "Piece.h"
#include "NotImplementedError.h"
#include "Resources.h"

#include "boost\lexical_cast.hpp"

Piece::Piece(uint16_t pieceNumber, Player::Color color) :
    pieceId(pieceNumber),
    pieceType(PieceType::Man),
    pieceName(),
    descriptionText(),
    color(color),
    pieceShape(),
    crownShape(),
    hidden(false),
    fieldMargin(),
    currentField(nullptr)
{
}

Piece::~Piece() {}

void Piece::init()
{
    sf::Color color = sf::Color::White, crownColor = sf::Color::Yellow;
    if (this->color == Player::Color::Black) {
        color = sf::Color::Black;
    }
    pieceShape.setRadius(0.5f);
    pieceShape.setFillColor(color);

    crownShape.setRadius(0.2f);
    crownShape.setFillColor(crownColor);

    pieceName = boost::lexical_cast<std::string>(pieceId);

    descriptionText.setFont(Resources::DefaultFont);
    descriptionText.setString(pieceName);
    descriptionText.setFillColor(sf::Color::Green);
}

void Piece::update(sf::Time advance)
{
    throw NotImplementedError();
}

void Piece::draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const
{
    if (hidden)
        return;
    renderTarget.draw(pieceShape, states);
    if (pieceType == PieceType::King)
        renderTarget.draw(crownShape, states);
    //renderTarget.draw(descriptionText, states);
}

void Piece::setPosition(const sf::Vector2f &position)
{
    pieceShape.setPosition(position.x + fieldMargin, position.y + fieldMargin);
    crownShape.setPosition(position.x + (0.5f - 0.2f), position.y + (0.5f - 0.2f));
    descriptionText.setPosition(position.x + (0.5f - fieldMargin), position.y + (0.5f - fieldMargin));
    Transformable::setPosition(position);
}

void Piece::setPosition(float x, float y)
{
    pieceShape.setPosition(x + fieldMargin, y + fieldMargin);
    crownShape.setPosition(x + (0.5f - 0.2f), y + (0.5f - 0.2f));
    descriptionText.setPosition(x + (0.5f - fieldMargin), y + (0.5f - fieldMargin));
    Transformable::setPosition(x, y);
}

Player::Color Piece::getColor() const
{
    return color;
}

void Piece::setFieldMargin(float margin)
{
    fieldMargin = margin;
    pieceShape.setRadius(0.5f - fieldMargin);
}

float Piece::getFieldMargin() const
{
    return fieldMargin;
}

void Piece::setFontScaleFactor(float scale)
{
    fontScaleFactor = scale;
    descriptionText.scale(fontScaleFactor, fontScaleFactor);
}

void Piece::setCurrentField(Field *field)
{
    currentField = field;
}

Field *Piece::getCurrentField() const
{
    return currentField;
}

Piece::PieceType Piece::getPieceType() const
{
    return pieceType;
}

void Piece::promote()
{
    pieceType = Piece::PieceType::King;
}

void Piece::hide()
{
    hidden = true;
}
