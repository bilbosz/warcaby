#include "Draughts.h"
#include "Resources.h"
#include "SingleThreadGameRunner.h"

#include "boost\scoped_ptr.hpp"

#include <cstdlib>

int main(int argc, char *argv[])
{
    Resources::init();

    boost::scoped_ptr<Game> draughts(new Draughts());
    boost::scoped_ptr<GameRunner> gameRunner(new SingleThreadGameRunner(draughts.get(), Resources::UpdateRate));

    gameRunner->init();
    gameRunner->run();

    return EXIT_SUCCESS;
}