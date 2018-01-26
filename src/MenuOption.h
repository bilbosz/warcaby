#pragma once

#include "MenuEntry.h"

#include <list>
#include <string>
#include <tuple>

#include <SFML/Graphics.hpp>

#include <boost/function.hpp>

class MenuOption : public MenuEntry
{
public:
    MenuOption();
    virtual ~MenuOption();

    void init() override;
    virtual void update(sf::Time time) override;
    virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const override;

    virtual void mouseClicked(const sf::Vector2f &pressPoint) override;

    virtual void setName(const std::wstring &name) override;

    virtual void select() override;
    virtual void unselect() override;

    void setPosition(float x, float y);
    void setPosition(const sf::Vector2f &position);
    sf::Vector2f getPosition() const override;

    sf::Vector2f getSize() const override;

    void addValue(const std::wstring &name, const boost::function<void()> &callback);
    void selectNextValue();

    void updateEntryText();

private:
    sf::Text entryText;
    std::list<std::pair<std::wstring, boost::function<void()>>> values;
    std::list<std::pair<std::wstring, boost::function<void()>>>::iterator selectedValue;
};
