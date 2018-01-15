#pragma once

#include "GameObject.h"

class Cursor final : public GameObject
{
public:
    enum State { Normal, Pressed };

    Cursor();
    virtual ~Cursor();

    virtual void init() override;
    virtual void update(sf::Time time) override;
    virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const override;

    void setPosition(float x, float y);
    void setPosition(const sf::Vector2f &position);
    sf::Vector2f getPosition() const;

private:
    sf::ConvexShape upperTriangle, lowerTriangle;
};
