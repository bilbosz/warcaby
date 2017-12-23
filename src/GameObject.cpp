#include "GameObject.h"

GameObject::GameObject() : size(), fontScaleFactor(1.0f) {}

GameObject::~GameObject() {}

void GameObject::setSize(const sf::Vector2f &size)
{
    this->size = size;
}

void GameObject::setSize(float w, float h)
{
    size.x = w;
    size.y = h;
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