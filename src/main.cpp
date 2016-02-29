#include "BrewMan.hpp"
#include <unistd.h>

int main(int argc, char** argv)
{
#ifndef NDEBUG
	// Console for reading stdout
	cpp3ds::Console::enable(cpp3ds::BottomScreen, cpp3ds::Color::Black);
#endif

	cpp3ds::Service::enable(cpp3ds::Network);
	BrewMan::BrewMan game;

	// Copy temp auto-updated binary to official location
	const char* oldFile = "sdmc:/3ds/BrewMan/tmp/update-copy.3dsx";
	const char* newFile = "sdmc:/3ds/BrewMan/BrewMan.3dsx";
	if (argc > 0 && strcmp(argv[0], "sdmc:/3ds/BrewMan/tmp/update.3dsx") == 0)
	{
		unlink(newFile);
		rename(oldFile, newFile);
	}

	game.run();
	return 0;
}
