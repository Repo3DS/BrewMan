#include "LoadingState.hpp"
#include "../AssetManager.hpp"
#include <TweenEngine/Tween.h>
#include <cpp3ds/Window/Window.hpp>
#include <cpp3ds/System/I18n.hpp>
#include <cpp3ds/System/Service.hpp>


namespace BrewMan {

LoadingState::LoadingState(StateStack& stack, Context& context)
: State(stack, context)
{
	m_background.setSize(cpp3ds::Vector2f(400.f, 240.f));
	m_background.setFillColor(cpp3ds::Color(0, 0, 0, 50));

	m_icon.setFont(AssetManager<cpp3ds::Font>::get("fonts/fontawesome.ttf"));
	m_icon.setFillColor(cpp3ds::Color(110,110,110,255));
	m_icon.setCharacterSize(80);
	m_icon.setString(L"\uf110");
	cpp3ds::FloatRect textRect = m_icon.getLocalBounds();
	m_icon.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + 1.f + textRect.height / 2.f);
	m_icon.setPosition(160.f, 120.f);
	m_icon.setScale(0.5f, 0.5f);
}

void LoadingState::renderTopScreen(cpp3ds::Window& window)
{
	//
}

void LoadingState::renderBottomScreen(cpp3ds::Window& window)
{
	window.draw(m_background);
	window.draw(m_icon);
}

bool LoadingState::update(float delta)
{
	if (m_rotateClock.getElapsedTime() > cpp3ds::milliseconds(100))
	{
		m_icon.rotate(45);
		m_rotateClock.restart();
	}
	return true;
}

bool LoadingState::processEvent(const cpp3ds::Event& event)
{
	return false;
}

} // namespace BrewMan
