#pragma once

#include "GameObject.h"
#include "Player.h"
#include "Transition.h"

#include <SFML/Graphics.hpp>

class Field;

/** \brief Klasa bierki.
 *
 * Reprezentuje pion lub damkę.
 */
class Piece final : public GameObject
{
public:
    enum struct PieceType { Man = 0, King };

    Piece(uint16_t pieceNumber, Player::Color color);
    virtual ~Piece();

    virtual void init() override;
    virtual void update(sf::Time time) override;
    virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const override;

    void setPosition(const sf::Vector2f &position);
    void setPosition(float x, float y);

    void transistToPosition(const sf::Vector2f &position, sf::Time startTime);
    void transistToPosition(float x, float y, sf::Time startTime);
    bool isMovingTransitionRunning() const;

    Player::Color getColor() const;

    void setFieldMargin(float margin);
    float getFieldMargin() const;

    void setFontScaleFactor(float scale) override;

    void setCurrentField(Field *field);
    Field *getCurrentField() const;

    PieceType getPieceType() const;
    void upgrade();
    void downgrade();

    void hide();
    void show();

private:
    uint16_t pieceId;
    PieceType pieceType;
    std::string pieceName;
    sf::Text descriptionText;

    Player::Color color;
    sf::CircleShape pieceShape;
    sf::CircleShape crownShape;
    bool hidden;
    float fieldMargin;

    Transition xMovingTransition, yMovingTransition;

    Field *currentField;
};
