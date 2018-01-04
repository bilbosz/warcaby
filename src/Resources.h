#pragma once

#include <SFML\Graphics.hpp>

#include <filesystem>
#include <random>

namespace Resources
{
	extern std::mt19937 RandomNumberGenerator;

    /// Domyślna czcionka wykorzystywana przez grę.
    extern sf::Font DefaultFont;

	extern sf::Color DebugTextColor;
	extern sf::Color BackgroundColor;
	extern sf::Color BoardColor;
	extern sf::Color BlackFieldColor, WhiteFieldColor;
	extern sf::Color SelectedFieldColor, AvailableFieldColor, AvailablePieceFieldColor;
	extern sf::Color BlackPieceColor, WhitePieceColor; 
	extern sf::Color CrownColor;

	extern uint16_t SideFieldsNumber;
	extern uint16_t PlayerPiecesRowsNumber;
	extern std::experimental::filesystem::path InitialBoardFile;

	extern float FieldMarginThickness;
	extern float BoardBorderSize;
	extern float PieceShapeRadius;
	extern float CrownShapeRadius;
	extern sf::Time PieceMoveLasting;
	
	extern std::string GameTitle;
	extern size_t VideoModeNumber;
	extern unsigned int AntialiasingLevel;
	extern sf::Uint32 WindowStyle;
	extern bool VerticalSync;
	extern bool KeyRepeat;
	extern bool MouseCursorGrab;
	extern uint16_t UpdateRate;

    /// Funkcja, która przygotowuje wartości zmiennych w przestrzeni nazw Resources.
    extern void init();
}
