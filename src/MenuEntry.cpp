#include "MenuEntry.h"
#include "Resources.h"

MenuEntry::MenuEntry() : name(), isHidden_(false) {}

MenuEntry::~MenuEntry() {}

void MenuEntry::init() {}

void MenuEntry::setName(const std::wstring &name)
{
    this->name = name;
}

std::wstring MenuEntry::getName() const
{
    return name;
}

void MenuEntry::hide()
{
    isHidden_ = true;
}

void MenuEntry::show()
{
    isHidden_ = false;
}

bool MenuEntry::isHidden() const
{
    return isHidden_;
}
