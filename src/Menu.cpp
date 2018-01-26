#include "Menu.h"
#include "Resources.h"

Menu::Menu() : title(), entries(), titleText() {}

Menu::~Menu() {}

void Menu::init()
{
    titleText.setFont(Resources::DefaultFont);
    titleText.setCharacterSize(Resources::MenuCharacterSize);
    titleText.setFillColor(Resources::MenuTitleColor);
    titleText.setPosition(0.0f, 0.0f);
}

void Menu::update(sf::Time time) {}

void Menu::draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const
{
    renderTarget.draw(titleText, states);
    for (const std::unique_ptr<MenuEntry> &entry : entries) {
        renderTarget.draw(*entry, states);
    }
}

void Menu::setTitle(const std::wstring &title)
{
    this->title = title;
    titleText.setString(this->title);
}

std::wstring Menu::getTitle() const
{
    return title;
}

void Menu::setFontScaleFactor(float scale)
{
    fontScaleFactor = scale;
    titleText.setScale(fontScaleFactor, fontScaleFactor);
    for (std::unique_ptr<MenuEntry> &entry : entries) {
        entry->setFontScaleFactor(fontScaleFactor);
    }
}

void Menu::addEntry(std::unique_ptr<MenuEntry> &&entry)
{
    entries.push_back(std::move(entry));
    rearrangeEntriesPosition();
}

void Menu::hideEntryByName(const std::wstring &name)
{
    for (std::unique_ptr<MenuEntry> &entry : entries) {
        if (entry->getName() == name) {
            entry->hide();
        }
    }
    rearrangeEntriesPosition();
}

void Menu::showEntryByName(const std::wstring &name)
{
    for (std::unique_ptr<MenuEntry> &entry : entries) {
        if (entry->getName() == name) {
            entry->show();
        }
    }
    rearrangeEntriesPosition();
}

void Menu::unselectAll()
{
    for (std::unique_ptr<MenuEntry> &entry : entries) {
        entry->unselect();
    }
}

void Menu::mouseClicked(const sf::Vector2f &pressPoint)
{
    for (std::unique_ptr<MenuEntry> &entry : entries) {
        sf::FloatRect entryRect(entry->getPosition(), entry->getSize());
        if (entryRect.contains(pressPoint)) {
            entry->mouseClicked(pressPoint);
        }
    }
}

void Menu::mouseHovered(const sf::Vector2f &hoverPoint)
{
    for (std::unique_ptr<MenuEntry> &entry : entries) {
        sf::FloatRect entryRect(entry->getPosition(), entry->getSize());
        if (entryRect.contains(hoverPoint)) {
            entry->select();
        } else {
            entry->unselect();
        }
    }
}

void Menu::rearrangeEntriesPosition()
{
    float nextEntryTop;
    sf::FloatRect lastElemBounds = titleText.getGlobalBounds();
    nextEntryTop = lastElemBounds.top + lastElemBounds.height + Resources::MenuCharacterSize * fontScaleFactor;
    for (const std::unique_ptr<MenuEntry> &entry : entries) {
        if (entry->isHidden()) {
            continue;
        }
        entry->setPosition(0.0f, nextEntryTop);
        nextEntryTop = entry->getPosition().y + entry->getSize().y;
    }
}
