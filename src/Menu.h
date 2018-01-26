#pragma once

#include "GameObject.h"
#include "MenuEntry.h"

#include <SFML/Graphics.hpp>

#include <memory>
#include <vector>

class Menu final : public GameObject
{
public:
    Menu();
    virtual ~Menu();
    
	virtual void init() override;
    virtual void update(sf::Time time) override;
    virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates states) const override;
	
	void setTitle(const std::wstring &title);
	std::wstring getTitle() const;
	
	void setFontScaleFactor(float scale) override;

	void addEntry(std::unique_ptr<MenuEntry> &&entry);
	void hideEntryByName(const std::wstring &name);
	void showEntryByName(const std::wstring &name);

	void unselectAll();

	void mouseClicked(const sf::Vector2f &pressPoint);
	void mouseHovered(const sf::Vector2f &hoverPoint);

private:
    std::wstring title;
    std::vector<std::unique_ptr<MenuEntry>> entries;
	sf::Text titleText;

	void rearrangeEntriesPosition();
};
