#pragma once

#include "SFML\Graphics.hpp"
#include "SFML\System.hpp"

/** \brief Reprezentacja rysowalnego obiektu w grze.
 *
 * Każdy obiekt rysowalny, "fizyczny" jest instancją tej klasy. Pozycja i rozmiar są względne do rozmiaru widoku, który
 * będzie miał wysokość 10 i odpowiednio przeskalowaną szerokość.
 */
class GameObject : public sf::Drawable, public sf::Transformable
{
public:
    GameObject();
    virtual ~GameObject();

    virtual void init() = 0;
    virtual void update(sf::Time advance) = 0;
    virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const = 0;

    virtual void setSize(const sf::Vector2f &size);
    virtual void setSize(float w, float h);
    virtual sf::Vector2f getSize() const;

    virtual void setFontScaleFactor(float scale);
    virtual float getFontScaleFactor() const;

protected:
    sf::Vector2f size;
    float fontScaleFactor;
};
