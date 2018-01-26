#pragma once

#include "GameObject.h"

class MenuEntry : public GameObject
{
public:
    MenuEntry();
    virtual ~MenuEntry();

    virtual void mouseClicked(const sf::Vector2f &pressPoint) = 0;
    virtual void select() = 0;
    virtual void unselect() = 0;

    virtual void init() override;
    virtual void setName(const std::wstring &name);
    virtual std::wstring getName() const;

    virtual void hide();
    virtual void show();
    virtual bool isHidden() const;

protected:
    std::wstring name;
    bool isHidden_;
};
