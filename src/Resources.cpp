#include "Error.h"
#include "Resources.h"

namespace Resources
{
    std::mt19937 RandomNumberGenerator;

    sf::Font DefaultFont = sf::Font();

    sf::Color DebugTextColor(88U, 44U, 1U);
    sf::Color BackgroundColor(230U, 230U, 230U);
    sf::Color BoardColor(200U, 200U, 200U);
    sf::Color BlackFieldColor(88U, 44U, 1U), WhiteFieldColor(255U, 255U, 240U);
    sf::Color SelectedFieldColor(220U, 20U, 60U), AvailableFieldColor(170U, 150U, 200U),
        AvailablePieceFieldColor(51U, 102U, 204U);
    sf::Color BlackPieceColor(sf::Color::Black), WhitePieceColor(sf::Color::White);
    sf::Color CrownColor(255U, 215U, 0U);

    uint16_t SideFieldsNumber(8U);
    uint16_t PlayerPiecesRowsNumber(3U);
    std::experimental::filesystem::path InitialBoardFile;

    float FieldMarginThickness(0.1f);
    float BoardBorderSize(0.2f);
    float PieceShapeRadius(0.5f);
    float CrownShapeRadius(0.2f);
    sf::Time PieceMoveLasting(sf::milliseconds(500));

    std::string GameTitle("Warcaby");
    size_t VideoModeNumber(0U);
    unsigned int AntialiasingLevel(1U);
    sf::Uint32 WindowStyle(sf::Style::Fullscreen);
    bool VerticalSync(true);
    bool KeyRepeat(true);
    bool MouseCursorGrab(false);
    uint16_t UpdateRate(60U);

    void init()
    {
        RandomNumberGenerator.seed(std::random_device()());

        std::experimental::filesystem::path fontPath = R"(res\font\antt-ttf\AntykwaTorunska-Regular.ttf)";
        if (!DefaultFont.loadFromFile(fontPath.string()))
            throw Error("Nie można załadować czcionki");
    }
}