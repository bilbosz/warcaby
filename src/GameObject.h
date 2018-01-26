#pragma once

#include <SFML/Graphics.hpp>

/** \brief Reprezentacja rysowalnego obiektu w grze.
 *
 * Każdy obiekt rysowalny, "fizyczny" jest instancją tej klasy. Pozycja i rozmiar są względne do rozmiaru widoku, który
 * będzie miał wysokość /ref Resources::SideFieldsNumber + 2 i odpowiednio przeskalowaną szerokość.
 */
class GameObject : public sf::Drawable, public sf::Transformable
{
public:
    GameObject();
    virtual ~GameObject();

    virtual void init() = 0;
    virtual void update(sf::Time time) = 0;
    virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const = 0;

	virtual void setPosition(float x, float y);
	virtual void setPosition(const sf::Vector2f &position);
	virtual sf::Vector2f getPosition() const;

    virtual void setSize(float w, float h);
    virtual void setSize(const sf::Vector2f &size);
    virtual sf::Vector2f getSize() const;

    virtual void setFontScaleFactor(float scale);
    virtual float getFontScaleFactor() const;

protected:
    sf::Vector2f size;
    float fontScaleFactor;
};
