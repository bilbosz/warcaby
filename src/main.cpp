#include "Draughts.h"
#include "ProjectConfig.h"
#include "Resources.h"
#include "SingleThreadGameRunner.h"

#include <cstdlib>
#include <memory>
#ifdef CONSOLE_STOP
#    include <iostream>
#endif

int main(int, char *[])
{
    Resources::init();

    std::unique_ptr<Game> draughts = std::make_unique<Draughts>();
    std::unique_ptr<GameRunner> gameRunner =
        std::make_unique<SingleThreadGameRunner>(draughts.get(), Resources::UpdateRate);

    gameRunner->init();
    gameRunner->run();

#ifdef CONSOLE_STOP
    std::cout << "Naciśnij dowolny przycisk by zakończyć..." << std::endl;
    getchar();
#endif

    return EXIT_SUCCESS;
}
