#include "Draughts.h"
#include "GameRunner.h"
#include "Resources.h"
#include "SingleThreadGameRunner.h"

#include "boost\scoped_ptr.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[])
{
    Resources::init();

    boost::scoped_ptr<Game> draughts(new Draughts());
    boost::scoped_ptr<GameRunner> gameRunner(new SingleThreadGameRunner(draughts.get(), 60U));

    gameRunner->init();
    gameRunner->run();

    // std::getchar();

    return EXIT_SUCCESS;
}