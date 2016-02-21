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
	void setCurrentScreenshot(int screenshotIndex);
	void loadApp();

private:
	AppList m_appList;
	AppItem* m_currentApp;
	DownloadQueue m_downloads;

	TweenEngine::TweenManager m_tweenManager;

	float m_appListPositionX;

	int m_currentScreenshot;
	util3ds::TweenSprite m_screenshotTop;
	util3ds::TweenSprite m_screenshotBottom;
	util3ds::TweenText m_arrowLeft;
	util3ds::TweenText m_arrowRight;
	util3ds::TweenText m_close;

	// Bottom screen
	cpp3ds::Sprite m_icon;
	cpp3ds::Text m_textTitle;
	cpp3ds::Text m_textDescription;
	float m_descriptionVelocity;
	cpp3ds::RectangleShape m_fadeShape;
	cpp3ds::View m_descriptionView;

	cpp3ds::RectangleShape m_screenshotsBackground;
	cpp3ds::Text m_textScreenshotsEmpty;

	cpp3ds::Thread m_loadThread;
	std::vector<std::unique_ptr<cpp3ds::Texture>> m_screenshotTextures;
	std::vector<std::unique_ptr<cpp3ds::Sprite>> m_screenshots;

	util3ds::TweenText m_textOrderAlpha;
	util3ds::TweenText m_textDownload;
};

} // namespace BrewMan

#endif // BREWMAN_BROWSESTATE_HPP
