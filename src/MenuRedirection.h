#pragma once

#include "MenuEntry.h"

#include <SFML/Graphics.hpp>

#include <boost/function.hpp>

class MenuRedirection : public MenuEntry
{
public:
    MenuRedirection();
    virtual ~MenuRedirection();

    virtual void init() override;
    virtual void update(sf::Time time) override;
    virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const override;

	virtual void mouseClicked(const sf::Vector2f &pressPoint) override;
	virtual void setName(const std::wstring &name) override;
	virtual void select() override;
	virtual void unselect() override;

	void setFontScaleFactor(float scale) override;

	void setPosition(float x, float y);
	void setPosition(const sf::Vector2f &position);
	sf::Vector2f getPosition() const override;

	sf::Vector2f getSize() const override;

    void setTarget(boost::function<void()> target);
    boost::function<void()> getTarget() const;

private:
	sf::Text nameText;
	boost::function<void()> target;
};
