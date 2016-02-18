#include "TitleState.hpp"
#include <TweenEngine/Tween.h>
#include <cpp3ds/Window/Window.hpp>
#include <cpp3ds/System/I18n.hpp>
#include <cpp3ds/System/Service.hpp>

using namespace TweenEngine;
using namespace util3ds;

namespace BrewMan {

TitleState::TitleState(StateStack& stack, Context& context)
: State(stack, context)
{
	m_logoTexture.loadFromFile("images/logo.png");
	m_logo.setTexture(m_logoTexture, true);
	m_logo.setColor(cpp3ds::Color::Transparent);
	m_logo.setPosition(380.f - m_logo.getLocalBounds().width, 40.f);

	m_robotTexture.loadFromFile("images/fancybot.png");
	m_robot.setTexture(m_robotTexture, true);
	m_robot.setColor(cpp3ds::Color::Black);
	m_robot.setPosition(50.f, 240.f);
	m_robot.setScale(0.4f, 0.4f);

	// Tween the robot
	Tween::to(m_robot, TweenSprite::POSITION_Y, 0.5f)
			.target(160.f)
			.delay(1.f)
			.start(m_manager);
	Tween::to(m_robot, TweenSprite::POSITION_XY, 1.f)
			.target(30.f, 40.f)
			.delay(2.f)
			.start(m_manager);
	Tween::to(m_robot, TweenSprite::SCALE_XY, 1.f)
			.target(1.f, 1.f)
			.delay(2.f)
			.start(m_manager);
	Tween::to(m_robot, TweenSprite::COLOR_RGB, 1.f)
			.target(255, 255, 255)
			.delay(2.2f)
			.start(m_manager);

	// Tween the logo after the robot
	Tween::to(m_logo, TweenSprite::POSITION_Y, 1.f)
			.targetRelative(30.f)
			.delay(2.8f)
			.start(m_manager);
	Tween::to(m_logo, TweenSprite::COLOR_ALPHA, 1.f)
			.target(255)
			.delay(2.8f)
			.start(m_manager);
}

void TitleState::renderTopScreen(cpp3ds::Window& window)
{
	window.draw(m_logo);
	window.draw(m_robot);
}

void TitleState::renderBottomScreen(cpp3ds::Window& window)
{
	// Nothing
}

bool TitleState::update(float delta)
{
	m_manager.update(delta);
	return true;
}

bool TitleState::processEvent(const cpp3ds::Event& event)
{
	return true;
}

} // namespace BrewMan
