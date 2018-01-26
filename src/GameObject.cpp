#include "GameObject.h"

GameObject::GameObject() : size(), fontScaleFactor(1.0f) {}

GameObject::~GameObject() {}

void GameObject::setPosition(float x, float y)
{
	Transformable::setPosition(x, y);
}

void GameObject::setPosition(const sf::Vector2f &position)
{
	setPosition(position.x, position.y);
}

sf::Vector2f GameObject::getPosition() const
{
	return Transformable::getPosition();
}

void GameObject::setSize(float w, float h)
{
    size.x = w;
    size.y = h;
}

void GameObject::setSize(const sf::Vector2f &size)
{
    this->size = size;
}

sf::Vector2f GameObject::getSize() const
{
    return size;
}

void GameObject::setFontScaleFactor(float scale)
{
    fontScaleFactor = scale;
}

float GameObject::getFontScaleFactor() const
{
    return fontScaleFactor;
}