#include "BrewMan.hpp"

int main(int argc, char** argv)
{
	cpp3ds::Console::enableBasic(cpp3ds::BottomScreen); // Console for reading stdout
//	cpp3ds::Console::enable(cpp3ds::BottomScreen, cpp3ds::Color::Black); // Console for reading stdout
	cpp3ds::Service::enable(cpp3ds::Network);
	BrewMan::BrewMan game;
	game.run();
	return 0;
}
