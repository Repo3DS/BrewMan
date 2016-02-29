#ifndef BREWMAN_TITLESTATE_HPP
#define BREWMAN_TITLESTATE_HPP

#include "State.hpp"
#include "../TweenObjects.hpp"
#include <cpp3ds/Graphics/Sprite.hpp>
#include <cpp3ds/Graphics/Text.hpp>
#include <TweenEngine/TweenManager.h>

namespace BrewMan {

class TitleState : public State
{
public:
	TitleState(StateStack& stack, Context& context);

	virtual void renderTopScreen(cpp3ds::Window& window);
	virtual void renderBottomScreen(cpp3ds::Window& window);
	virtual bool update(float delta);
	virtual bool processEvent(const cpp3ds::Event& event);

private:
	cpp3ds::Texture m_logoTexture;
	cpp3ds::Texture m_robotTexture;
	cpp3ds::Text m_textVersion;
	util3ds::TweenSprite m_logo;
	util3ds::TweenSprite m_robot;

	TweenEngine::TweenManager m_manager;
};

} // namespace BrewMan

#endif // BREWMAN_TITLESTATE_HPP
