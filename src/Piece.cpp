#include "Piece.h"
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
    xMovingTransition(),
    yMovingTransition(),
    currentField(nullptr)
{
}

Piece::~Piece() {}

void Piece::init()
{
    sf::Color color = Resources::WhitePieceColor;
    if (this->color == Player::Color::Black) {
        color = Resources::BlackPieceColor;
    }
    pieceShape.setRadius(Resources::PieceShapeRadius);
    pieceShape.setFillColor(color);

    crownShape.setRadius(Resources::CrownShapeRadius);
    crownShape.setFillColor(Resources::CrownColor);

    pieceName = boost::lexical_cast<std::string>(pieceId);

    descriptionText.setFont(Resources::DefaultFont);
    descriptionText.setString(pieceName);
    descriptionText.setFillColor(Resources::DebugTextColor);

    xMovingTransition.setEasingCurve(ShrinkedSin);
    xMovingTransition.setLasting(Resources::PieceMoveLasting);
    yMovingTransition.setEasingCurve(ShrinkedSin);
    yMovingTransition.setLasting(Resources::PieceMoveLasting);
}

void Piece::update(sf::Time time)
{
    if (isMovingTransitionRunning()) {
        float x = xMovingTransition.evaluate(time);
        float y = yMovingTransition.evaluate(time);
        setPosition(x, y);
    }
}

void Piece::draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const
{
    if (hidden)
        return;
    renderTarget.draw(pieceShape, states);
    if (pieceType == PieceType::King)
        renderTarget.draw(crownShape, states);
}

void Piece::setPosition(const sf::Vector2f &position)
{
    pieceShape.setPosition(position.x + fieldMargin, position.y + fieldMargin);
    crownShape.setPosition(position.x + (0.5f - crownShape.getRadius()), position.y + (0.5f - crownShape.getRadius()));
    descriptionText.setPosition(position.x + (0.5f - fieldMargin), position.y + (0.5f - fieldMargin));
    Transformable::setPosition(position);
}

void Piece::setPosition(float x, float y)
{
    pieceShape.setPosition(x + fieldMargin, y + fieldMargin);
    crownShape.setPosition(x + (0.5f - crownShape.getRadius()), y + (0.5f - crownShape.getRadius()));
    descriptionText.setPosition(x + (0.5f - fieldMargin), y + (0.5f - fieldMargin));
    Transformable::setPosition(x, y);
}

void Piece::transistToPosition(const sf::Vector2f &position, sf::Time startTime)
{
    transistToPosition(position.x, position.y, startTime);
}

void Piece::transistToPosition(float x, float y, sf::Time startTime)
{
    const sf::Vector2f &startPosition = this->getPosition();

    float startX = startPosition.x;
    xMovingTransition.setMinValue(startX);
    xMovingTransition.setMaxValue(x);
    xMovingTransition.setStartTime(startTime);

    float startY = startPosition.y;
    yMovingTransition.setMinValue(startY);
    yMovingTransition.setMaxValue(y);
    yMovingTransition.setStartTime(startTime);
}

bool Piece::isMovingTransitionRunning() const
{
    return !xMovingTransition.isFinished() || !yMovingTransition.isFinished();
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

void Piece::upgrade()
{
    pieceType = Piece::PieceType::King;
}

void Piece::downgrade()
{
    pieceType = Piece::PieceType::Man;
}

void Piece::hide()
{
    hidden = true;
}

void Piece::show()
{
    hidden = false;
}
