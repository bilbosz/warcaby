#include <filesystem>

#include "Error.h"
#include "Resources.h"

namespace Resources
{
    sf::Font DefaultFont = sf::Font();

    void init()
    {
        std::experimental::filesystem::path fontPath = R"(res\font\Courier New\cour.ttf)";
        if (!Resources::DefaultFont.loadFromFile(fontPath.string()))
            throw Error("Failed to load the font");
    }
}