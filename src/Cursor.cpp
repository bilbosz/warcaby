#include "Cursor.h"
#include "Resources.h"

Cursor::Cursor() : GameObject(), upperTriangle(3U), lowerTriangle(3U) {}

Cursor::~Cursor() {}

void Cursor::init()
{
    upperTriangle.setPoint(0U, sf::Vector2f(0.0f, 0.0f));
    upperTriangle.setPoint(1U, sf::Vector2f(50.0f, 30.0f));
    upperTriangle.setPoint(2U, sf::Vector2f(30.0f, 30.0f));
    upperTriangle.setFillColor(Resources::CursorColor);

    lowerTriangle.setPoint(0U, sf::Vector2f(0.0f, 0.0f));
    lowerTriangle.setPoint(1U, sf::Vector2f(30.0f, 30.0f));
    lowerTriangle.setPoint(2U, sf::Vector2f(30.0f, 50.0f));
    lowerTriangle.setFillColor(Resources::CursorColor);
}

void Cursor::update(sf::Time time) {}

void Cursor::draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const
{
    renderTarget.draw(upperTriangle, states);
    renderTarget.draw(lowerTriangle, states);
}

void Cursor::setPosition(float x, float y)
{
    GameObject::setPosition(x, y);
    upperTriangle.setPosition(x, y);
    lowerTriangle.setPosition(x, y);
}

void Cursor::setPosition(const sf::Vector2f &position)
{
    setPosition(position.x, position.y);
}

sf::Vector2f Cursor::getPosition() const
{
    return GameObject::getPosition();
}
