#ifndef BREWMAN_BROWSESTATE_HPP
#define BREWMAN_BROWSESTATE_HPP

#include "State.hpp"
#include "../AppList.hpp"
#include "../DownloadQueue.hpp"
#include <cpp3ds/Graphics/Sprite.hpp>
#include <cpp3ds/Graphics/Texture.hpp>
#include <cpp3ds/System/Clock.hpp>
#include <cpp3ds/Graphics/RectangleShape.hpp>
#include <TweenEngine/TweenManager.h>

namespace BrewMan {

class BrowseState : public State
{
public:
	BrowseState(StateStack& stack, Context& context);

	virtual void renderTopScreen(cpp3ds::Window& window);
	virtual void renderBottomScreen(cpp3ds::Window& window);
	virtual bool update(float delta);
	virtual bool processEvent(const cpp3ds::Event& event);

private:
	void setItemIndex(int index);
	void setCurrentApp(AppItem* app);

private:
	AppList m_appList;
	AppItem* m_currentApp;
	DownloadQueue m_downloads;

	TweenEngine::TweenManager m_tweenManager;

	float m_appListPositionX;

	// Bottom screen
	cpp3ds::Transform m_transform;
	cpp3ds::Sprite m_icon;
	cpp3ds::Text m_textOrderAlpha;
};

} // namespace BrewMan

#endif // BREWMAN_BROWSESTATE_HPP
