#pragma once

#include "GameObject.h"
#include "Piece.h"

#include "SFML\Graphics.hpp"

class Piece;

class Field final : public GameObject
{
public:
    enum Color { White = 0, Black };
    enum Highlight { None = 0, Selected, AvailableMove, PreviousMove, HighlightCount };

    Field(uint16_t x, uint16_t y, Color color);
    virtual ~Field();

    virtual void init() override;
    virtual void update(sf::Time advance) override;
    virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const override;

    void setPosition(const sf::Vector2f &position);
    void setPosition(float x, float y);

    void setFontScaleFactor(float scale) override;

    void setHighlight(Highlight highlight);
    Highlight getHighlight() const;

    sf::Rect<float> getBoundaries() const;

    Field::Color getColor() const;

    void setFrameThickness(float thickness);
    float getFrameThickness() const;

    void setCurrentPiece(Piece *piece);
    Piece *getCurrentPiece() const;

    sf::Vector2i getBoardPostion() const;

private:
    sf::Vector2i boardPosition;
    std::string fieldName;
    Color color;

    sf::ConvexShape fieldShape;
    Highlight highlight;
    sf::Text descriptionText;
    sf::ConvexShape frameShape;
    float frameThickness;

    Piece *currentPiece;
};
