#include "MenuOption.h"
#include "Resources.h"

MenuOption::MenuOption() : entryText(), values(), selectedValue(values.end()) {}

MenuOption::~MenuOption() {}

void MenuOption::init()
{
    MenuEntry::init();
    entryText.setFont(Resources::DefaultFont);
    entryText.setFillColor(Resources::MenuUnselectedEntryColor);
    entryText.setCharacterSize(Resources::MenuEntryCharacterSize);
}

void MenuOption::update(sf::Time time) {}

void MenuOption::draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const
{
	if (isHidden_)
		return;
    renderTarget.draw(entryText, states);
}

void MenuOption::mouseClicked(const sf::Vector2f &pressPoint)
{
    selectNextValue();
}

void MenuOption::setName(const std::wstring &name)
{
    MenuEntry::setName(name);
    updateEntryText();
}

void MenuOption::select()
{
    entryText.setFillColor(Resources::MenuSelectedEntryColor);
}

void MenuOption::unselect()
{
    entryText.setFillColor(Resources::MenuUnselectedEntryColor);
}

void MenuOption::setPosition(float x, float y)
{
    entryText.setPosition(x, y);
    GameObject::setPosition(x, y);
}

void MenuOption::setPosition(const sf::Vector2f &position)
{
    setPosition(position.x, position.y);
}

sf::Vector2f MenuOption::getPosition() const
{
    return sf::Vector2f(entryText.getGlobalBounds().left, entryText.getGlobalBounds().top);
}

sf::Vector2f MenuOption::getSize() const
{
    return sf::Vector2f(entryText.getGlobalBounds().width, entryText.getGlobalBounds().height);
}

void MenuOption::addValue(const std::wstring &value, const boost::function<void()> &callback)
{
    values.push_back(std::make_pair(value, callback));
    selectedValue = values.begin();
}

void MenuOption::selectNextValue()
{
    ++selectedValue;
    if (selectedValue == values.end())
        selectedValue = values.begin();
    updateEntryText();
    selectedValue->second();
}

void MenuOption::updateEntryText()
{
    if (selectedValue == values.end()) {
        entryText.setString(name);
    } else {
        entryText.setString(name + L": " + selectedValue->first);
    }
}
