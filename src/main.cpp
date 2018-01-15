#include "Draughts.h"
#include "ProjectConfig.h"
#include "Resources.h"
#include "SingleThreadGameRunner.h"

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#ifdef CONSOLE_STOP
#    include <iostream>
#endif

int main(int, char *[])
{
	Resources::init();

	boost::scoped_ptr<Game> draughts(new Draughts());
	boost::scoped_ptr<GameRunner> gameRunner(new SingleThreadGameRunner(draughts.get(), Resources::UpdateRate));

	gameRunner->init();
	gameRunner->run();

#ifdef CONSOLE_STOP
	std::cout << "Naciśnij dowolny przycisk by zakończyć..." << std::endl;
	getchar();
#endif

	return EXIT_SUCCESS;
}
