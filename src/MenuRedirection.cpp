#include "MenuRedirection.h"
#include "Resources.h"

MenuRedirection::MenuRedirection() : MenuEntry(), target() {}

MenuRedirection::~MenuRedirection() {}

void MenuRedirection::init()
{
    MenuEntry::init();
    nameText.setFont(Resources::DefaultFont);
    nameText.setFillColor(Resources::MenuUnselectedEntryColor);
    nameText.setCharacterSize(Resources::MenuEntryCharacterSize);
}

void MenuRedirection::update(sf::Time time) {}

void MenuRedirection::draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const
{
	if (isHidden_)
		return;
    renderTarget.draw(nameText, states);
}

void MenuRedirection::mouseClicked(const sf::Vector2f &pressPoint)
{
	if (isHidden_)
		return;
	target();
}

void MenuRedirection::setName(const std::wstring &name)
{
    MenuEntry::setName(name);
	nameText.setString(this->name);
}

void MenuRedirection::select()
{
    nameText.setFillColor(Resources::MenuSelectedEntryColor);
}

void MenuRedirection::unselect()
{
    nameText.setFillColor(Resources::MenuUnselectedEntryColor);
}

void MenuRedirection::setFontScaleFactor(float scale)
{
    fontScaleFactor = scale;
    nameText.scale(fontScaleFactor, fontScaleFactor);
}

void MenuRedirection::setPosition(float x, float y)
{
    nameText.setPosition(x, y);
    GameObject::setPosition(x, y);
}

void MenuRedirection::setPosition(const sf::Vector2f &position)
{
    setPosition(position.x, position.y);
}

void MenuRedirection::setTarget(boost::function<void()> target)
{
    this->target = target;
}

boost::function<void()> MenuRedirection::getTarget() const
{
    return target;
}

sf::Vector2f MenuRedirection::getPosition() const
{
	return sf::Vector2f(nameText.getGlobalBounds().left, nameText.getGlobalBounds().top);
}

sf::Vector2f MenuRedirection::getSize() const
{
    return sf::Vector2f(nameText.getGlobalBounds().width, nameText.getGlobalBounds().height);
}
