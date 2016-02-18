#ifndef BREWMAN_BREWMAN_HPP
#define BREWMAN_BREWMAN_HPP

#include <TweenEngine/TweenManager.h>
#include <cpp3ds/Graphics.hpp>
#include <cpp3ds/Network.hpp>
#include "States/StateStack.hpp"


namespace BrewMan {

class BrewMan: public cpp3ds::Game {
public:
	BrewMan();
	~BrewMan();
	void update(float delta);
	void processEvent(cpp3ds::Event& event);
	void renderTopScreen(cpp3ds::Window& window);
	void renderBottomScreen(cpp3ds::Window& window);

private:
	StateStack m_stateStack;
	cpp3ds::Text textFPS;

	// Shared State context variables
	cpp3ds::String m_name;
};

}

#endif // BREWMAN_BREWMAN_HPP
