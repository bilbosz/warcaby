#include "Error.h"
#include "Resources.h"
#include "ProjectConfig.h"

#include <boost/format.hpp>

namespace Resources
{
    std::mt19937 RandomNumberGenerator;

    sf::Font InfoFont, DefaultFont;

	const sf::Color InfoTextColor(sf::Color::Blue),
		MenuTitleColor(sf::Color::Black),
		BackgroundColor(178U, 166U, 146U),
		BoardColor(159U, 117U, 69U),
		BlackFieldColor(178U, 147U, 97U),
		WhiteFieldColor(255U, 255U, 240U),
		SelectedFieldColor(186U, 0U, 0U, 153U),
		AvailableFieldColor(59U, 71U, 140U, 153U),
		AvailablePieceFieldColor(AvailableFieldColor),
		BlackPieceColor(68U, 48U, 17U),
		WhitePieceColor(224U, 224U, 192U),
		CursorColor(MenuTitleColor),
		CrownColor(207U, 190U, 108U),
		MenuUnselectedEntryColor(sf::Color::Black),
		MenuSelectedEntryColor(sf::Color::White);

    const uint16_t SideFieldsNumber(8U), 
		PlayerPiecesRowsNumber(3U);
    const std::string InitialBoardFile;

    const float FieldMarginThickness(0.1f), 
		BoardBorderSize(0.2f), 
		PieceShapeRadius(0.5f - FieldMarginThickness), 
		CrownShapeRadius(0.2f);
	const sf::Time PieceMoveLasting(sf::seconds(0.6f));

    const std::wstring GameTitle(L"Warcaby");

#ifdef FULLSCREEN
    const bool IsFullscreen(true);
    const sf::Uint32 WindowStyle(sf::Style::Fullscreen);
    const float WindowDesktopWidthRatio(1.0f),
		WindowDesktopHeightRatio(1.0f);
#else
    const bool IsFullscreen(false);
    const sf::Uint32 WindowStyle(sf::Style::Default);
    const float WindowDesktopWidthRatio(0.5f),
		WindowDesktopHeightRatio(0.5f);
#endif
	const unsigned int MenuCharacterSize(static_cast<unsigned int>(85U * WindowDesktopHeightRatio)),
		MenuEntryCharacterSize(static_cast<unsigned int>(MenuCharacterSize * 0.43f));
    const unsigned int AntialiasingLevel(1U);
    const bool VerticalSync(true), 
		KeyRepeat(true), 
		MouseCursorGrab(false);
    const uint16_t UpdateRate(60U);

    void init()
    {
        RandomNumberGenerator.seed(std::random_device()());

        std::string infoFontPath = R"(res/font/Courier New/cour.ttf)";
        if (!InfoFont.loadFromFile(infoFontPath))
            throw Error("Nie można załadować czcionki");

        std::string defaultFontPath = R"(res/font/antt-ttf/AntykwaTorunska-Regular.ttf)";
        if (!DefaultFont.loadFromFile(defaultFontPath))
            throw Error("Nie można załadować czcionki");


    }
}
