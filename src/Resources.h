#pragma once

#include <SFML/Graphics.hpp>

#include <random>

namespace Resources
{
    extern std::mt19937 RandomNumberGenerator;

    extern sf::Font InfoFont, 
		DefaultFont;

	extern const sf::Color InfoTextColor,
		MenuTitleColor,
		BackgroundColor,
		BoardColor,
		BlackFieldColor,
		WhiteFieldColor,
		SelectedFieldColor,
		AvailableFieldColor,
		AvailablePieceFieldColor,
		BlackPieceColor,
		WhitePieceColor,
		CursorColor,
		CrownColor,
		MenuUnselectedEntryColor,
		MenuSelectedEntryColor;

    extern const uint16_t SideFieldsNumber, 
		PlayerPiecesRowsNumber;
    extern const std::string InitialBoardFile;

    extern const float FieldMarginThickness, 
		BoardBorderSize, 
		PieceShapeRadius, 
		CrownShapeRadius;
    extern const sf::Time PieceMoveLasting;

    extern const std::wstring GameTitle;
    extern const bool IsFullscreen;
    extern const sf::Uint32 WindowStyle;
	extern const float WindowDesktopWidthRatio, 
		WindowDesktopHeightRatio;
    extern const unsigned int MenuCharacterSize,
		MenuEntryCharacterSize;
    extern const unsigned int AntialiasingLevel;
    extern const bool VerticalSync, 
		KeyRepeat, 
		MouseCursorGrab;
    extern const uint16_t UpdateRate;

    /// Funkcja, która przygotowuje wartości zmiennych w przestrzeni nazw Resources.
    extern void init();
}
