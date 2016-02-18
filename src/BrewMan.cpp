#include "BrewMan.hpp"
#include "States/TitleState.hpp"
#include "Notification.hpp"
#include "States/LoadingState.hpp"
#include "States/SyncState.hpp"
#include "States/BrowseState.hpp"


using namespace cpp3ds;
using namespace TweenEngine;

namespace BrewMan {


BrewMan::BrewMan()
: m_stateStack(State::Context(m_name))
{
	m_stateStack.registerState<TitleState>(States::Title);
	m_stateStack.registerState<LoadingState>(States::Loading);
	m_stateStack.registerState<SyncState>(States::Sync);
	m_stateStack.registerState<BrowseState>(States::Browse);
//	m_stateStack.pushState(States::Browse);
//	m_stateStack.pushState(States::Loading);
	m_stateStack.pushState(States::Sync);
//	m_stateStack.pushState(States::Title);

	textFPS.setColor(cpp3ds::Color::Red);
	textFPS.setCharacterSize(20);
}

BrewMan::~BrewMan()
{
	// Destructor called when game exits
}

void BrewMan::update(float delta)
{
	// Need to update before checking if empty
	m_stateStack.update(delta);
	if (m_stateStack.isEmpty())
		exit();

	Notification::update(delta);

	static int i;
	if (i++ % 10 == 0) {
		textFPS.setString(_("%.1f fps", 1.f / delta));
		textFPS.setPosition(395 - textFPS.getGlobalBounds().width, 2.f);
	}
}

void BrewMan::processEvent(Event& event)
{
	m_stateStack.processEvent(event);
}

void BrewMan::renderTopScreen(Window& window)
{
	window.clear(Color::White);
	m_stateStack.renderTopScreen(window);
	window.setView(window.getDefaultView());
	for (auto& notification : Notification::notifications)
		window.draw(*notification);
	window.draw(textFPS);
}

void BrewMan::renderBottomScreen(Window& window)
{
	window.clear(Color::White);
	m_stateStack.renderBottomScreen(window);
}


} // namespace BrewMan
