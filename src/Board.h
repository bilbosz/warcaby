#pragma once
#include "Field.h"
#include "GameObject.h"
#include "Piece.h"

#include "boost\scoped_array.hpp"

#include <cstdint>
#include <list>

class Board final : public GameObject
{
public:
    Board(uint16_t sideFieldNumber);
    virtual ~Board();

    virtual void init() override;
    virtual void update(sf::Time advance) override;
    virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const override;

    void setFontScaleFactor(float scale) override;

    std::list<Field *> getFieldsByColor(Field::Color color) const;
    Field *getFieldByPosition(const sf::Vector2f &position) const;
    Field *getFieldByBoardPosition(const sf::Vector2i &position) const;

    void clearSelection();

    bool isBoardPositionValid(const sf::Vector2i &position) const;
    bool isBoardPositionInvalid(const sf::Vector2i &position) const;
    bool isBoardPositionOccupied(const sf::Vector2i &position) const;

private:
    uint16_t sideFieldsNumber;
    sf::ConvexShape boardShape;
    Field ***fields;
    sf::IntRect validFields;
    float fieldMargin;
};