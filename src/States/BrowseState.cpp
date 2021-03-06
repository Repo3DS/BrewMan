#include "BrowseState.hpp"
#include "SyncState.hpp"
#include "../Notification.hpp"
#include "../AssetManager.hpp"
#include "../boot.h"
#include "../Util.hpp"
#include "../Installer.hpp"
#include <TweenEngine/Tween.h>
#include <cpp3ds/Window/Window.hpp>
#include <iostream>
#include <cpp3ds/System/I18n.hpp>
#include <cpp3ds/System/FileSystem.hpp>


namespace BrewMan {

BrowseState::BrowseState(StateStack& stack, Context& context)
: State(stack, context)
, m_appList(std::string(REPO_DIR)+"/apps/")
, m_appListPositionX(0.f)
, m_currentApp(nullptr)
, m_currentScreenshot(0)
, m_descriptionVelocity(0.f)
, m_loadThread(&BrowseState::loadApp, this)
, m_tabs(cpp3ds::Vector2f(320.f, 29.f))
{
	m_tabs.setBackgroundColor(cpp3ds::Color(200, 200, 200, 255));
	m_tabs.setTabBackgroundActiveColor(cpp3ds::Color::White);
	m_tabs.setTabBackgroundOutlineColor(cpp3ds::Color(150, 150, 150, 255));
	m_tabs.setTabBackgroundOutlineActiveColor(cpp3ds::Color(100, 100, 100, 255));
	m_tabs.setPadding(cpp3ds::Vector2f(8.f, 6.f));
	m_tabs.setTabSpacing(10.f);
	m_tabs.setTabHeight(23.f);
	m_tabs.addTab("App Info");
	m_tabs.addTab("Downloads");
	m_tabs.setSelectedIndex(0);

	m_textDownload.setFillColor(cpp3ds::Color::White);
	m_textDownload.setOutlineColor(cpp3ds::Color(0, 0, 0, 200));
	m_textDownload.setOutlineThickness(2.f);
	m_textDownload.setFont(AssetManager<cpp3ds::Font>::get("fonts/fontawesome.ttf"));
	m_textDownload.setString(L"\uf019");
	m_textDownload.setCharacterSize(30);
	m_textDownload.setPosition(67.f, 93.f);
	m_textDelete = m_textDownload;
	m_textDelete.setString(L"\uf1f8");
	m_textDelete.setPosition(70.f, 90.f);
	m_textExecute = m_textDownload;
	m_textExecute.setString(L"\uf01d");
	m_textExecute.setPosition(5.f, 90.f);

	m_arrowLeft.setFont(AssetManager<cpp3ds::Font>::get("fonts/fontawesome.ttf"));
	m_arrowLeft.setCharacterSize(24);
	m_arrowLeft.setFillColor(cpp3ds::Color(255,255,255,150));
	m_arrowLeft.setOutlineColor(cpp3ds::Color(0,0,0,100));
	m_arrowLeft.setOutlineThickness(1.f);
	m_arrowLeft.setPosition(4.f, 100.f);
	m_arrowLeft.setString(L"\uf053");
	m_arrowRight = m_arrowLeft;
	m_arrowRight.setPosition(298.f, 100.f);
	m_arrowRight.setString(L"\uf054");
	m_close = m_arrowLeft;
	m_close.setCharacterSize(30);
	m_close.setPosition(285.f, 4.f);
	m_close.setString(L"\uf00d");

	m_screenshotsBackground.setFillColor(cpp3ds::Color(190, 190, 190, 255));
	m_screenshotsBackground.setSize(cpp3ds::Vector2f(320.f, 74.f));
	m_screenshotsBackground.setPosition(0.f, 166.f);

	m_textScreenshotsEmpty.setCharacterSize(12);
	m_textScreenshotsEmpty.setFillColor(cpp3ds::Color(255, 255, 255, 220));
	m_textScreenshotsEmpty.setOutlineColor(cpp3ds::Color(170, 170, 170, 255));
	m_textScreenshotsEmpty.setStyle(cpp3ds::Text::Bold);
	m_textScreenshotsEmpty.setOutlineThickness(1);
	m_textScreenshotsEmpty.setString(_("No Screenshots"));
	cpp3ds::FloatRect textRect = m_textScreenshotsEmpty.getLocalBounds();
	m_textScreenshotsEmpty.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
	m_textScreenshotsEmpty.setPosition(160.f, 202.f);

	m_textTitle.setCharacterSize(16);
	m_textTitle.setFillColor(cpp3ds::Color::Black);
	m_textTitle.setStyle(cpp3ds::Text::Bold);
	m_textTitle.setPosition(207.f, 28.f);

	m_textDescription.setCharacterSize(13);
	m_textDescription.setFillColor(cpp3ds::Color(100, 100, 100, 255));

	m_textAuthor.setCharacterSize(11);
	m_textAuthor.setFillColor(cpp3ds::Color(100, 100, 100, 255));
	m_textAuthor.setPosition(4.f, 130.f);

	m_textVersion.setCharacterSize(13);
	m_textVersion.setFillColor(cpp3ds::Color::White);
	m_textVersion.setOutlineColor(cpp3ds::Color(0, 0, 0, 150));
	m_textVersion.setOutlineThickness(1.f);
	m_textVersion.setPosition(5.f, 30.f);

	m_fadeShape.setTexture(&AssetManager<cpp3ds::Texture>::get("images/fade.png"));
	m_fadeShape.setSize(cpp3ds::Vector2f(250.f, 8.f));
	m_fadeShape.setOrigin(m_fadeShape.getSize());
	m_fadeShape.setRotation(180.f);
	m_fadeShape.setPosition(102.f, 46.f);

	m_icon.setPosition(2.f, 30.f);
	m_icon.setScale(2.f, 2.f);

	m_descriptionView.reset(cpp3ds::FloatRect(0.f, 46.f, 320.f, 120.f));
	m_descriptionView.setViewport(cpp3ds::FloatRect(0.f, 46.f / 240.f, 1.f, 120.f / 240.f));

	setCurrentApp(m_appList.getSelected());
	setMode(AppInfo);
}

void BrowseState::renderTopScreen(cpp3ds::Window& window)
{
	window.draw(m_appList);

	if (m_currentScreenshot)
	{
		window.draw(m_screenshotTop);
	}
}

void BrowseState::renderBottomScreen(cpp3ds::Window& window)
{
	if (m_mode == AppInfo)
	{
		if (m_currentApp)
		{
			window.draw(m_screenshotsBackground);
			if (m_screenshots.empty())
				window.draw(m_textScreenshotsEmpty);

			window.draw(m_icon);
			window.draw(m_textTitle);

			cpp3ds::View defaultView = window.getView();
			window.setView(m_descriptionView);
			window.draw(m_textDescription);
			window.setView(defaultView);
			window.draw(m_fadeShape);

			window.draw(m_textAuthor);
			window.draw(m_textVersion);

			if (m_currentApp->isInstalled())
			{
				window.draw(m_textExecute);
				window.draw(m_textDelete);
			}
			else
			{
				window.draw(m_textDownload);
			}

			for (auto& screenshot : m_screenshots)
			{
				window.draw(*screenshot);
			}
		}
	}
	else if (m_mode == Downloads)
	{
		window.draw(m_downloads);
	}

	window.draw(m_tabs);

	if (m_currentScreenshot)
	{
		window.draw(m_screenshotBottom);
		if (m_currentScreenshot > 1)
			window.draw(m_arrowLeft);
		if (m_currentScreenshot < m_screenshots.size())
			window.draw(m_arrowRight);
		window.draw(m_close);
	}
}

bool BrowseState::update(float delta)
{
	static size_t activeDownloads;

	m_textDescription.move(0.f, m_descriptionVelocity * delta);
	if (m_textDescription.getPosition().y < 49.f - m_textDescription.getLocalBounds().height + 110.f)
		m_textDescription.setPosition(102.f, 49.f - m_textDescription.getLocalBounds().height + 110.f);
	if (m_textDescription.getPosition().y > 49.f)
		m_textDescription.setPosition(102.f, 49.f);

	if (m_mode == AppInfo)
	{
		if (!m_downloads.isDownloading(m_currentApp) && !m_downloads.isInstalling(m_currentApp))
		{
			m_textDownload.setFillColor(cpp3ds::Color::White);
		}
	}

	if (activeDownloads != m_downloads.getActiveCount())
	{
		activeDownloads = m_downloads.getActiveCount();
		if (activeDownloads == 0)
			m_tabs.getTab(1).setCaption("Downloads");
		else
			m_tabs.getTab(1).setCaption(_("Downloads (%d)", m_downloads.getActiveCount()));
	}


	m_downloads.update(delta);
	m_tweenManager.update(delta);
	return true;
}

bool BrowseState::processEvent(const cpp3ds::Event& event)
{
	if (m_currentScreenshot)
	{
		if (event.type == cpp3ds::Event::KeyPressed)
		{
			switch (event.key.code)
			{
				case cpp3ds::Keyboard::B:
					setCurrentScreenshot(0);
					break;
				case cpp3ds::Keyboard::DPadRight:
					if (m_currentScreenshot < m_screenshots.size())
						setCurrentScreenshot(m_currentScreenshot + 1);
					break;
				case cpp3ds::Keyboard::DPadLeft:
					if (m_currentScreenshot > 1)
						setCurrentScreenshot(m_currentScreenshot - 1);
					break;
				default:
					break;
			}
		}
		else if (event.type == cpp3ds::Event::TouchBegan)
		{
			if (m_close.getGlobalBounds().contains(event.touch.x, event.touch.y))
			{
				setCurrentScreenshot(0);
			}
			else if (m_currentScreenshot > 1 && event.touch.x < 140)
			{
				setCurrentScreenshot(m_currentScreenshot - 1);
			}
			else if (m_currentScreenshot < m_screenshots.size() && event.touch.x > 180)
			{
				setCurrentScreenshot(m_currentScreenshot + 1);
			}
		}
		return false;
	}

	// If any key is pressed, trigger the next screen
	if (event.type == cpp3ds::Event::KeyPressed)
	{
		int index = m_appList.getSelectedIndex();

		switch (event.key.code)
		{
			case cpp3ds::Keyboard::Select:
				requestStackClear();
				return true;
			case cpp3ds::Keyboard::A: {
				m_loadThread.launch();
//				loadApp();
				break;
			}
			case cpp3ds::Keyboard::DPadUp:
				if (index % 4 == 0)
					break;
				setItemIndex(index - 1);
				break;
			case cpp3ds::Keyboard::DPadDown:
				if (index % 4 == 3)
					break;
				setItemIndex(index + 1);
				break;
			case cpp3ds::Keyboard::DPadLeft:
#if EMULATION
			case cpp3ds::Keyboard::X:
#endif
				setItemIndex(index - 4);
				break;
			case cpp3ds::Keyboard::DPadRight:
#ifdef EMULATION
			case cpp3ds::Keyboard::Y:
#endif
				setItemIndex(index + 4);
				break;
			case cpp3ds::Keyboard::L:
				setItemIndex(index - 8);
				break;
			case cpp3ds::Keyboard::R:
				setItemIndex(index + 8);
				break;
			default:
				break;
		}
	}
	else if (event.type == cpp3ds::Event::TouchBegan)
	{
		if (m_mode == AppInfo)
		{
			int i = 0;
			for (auto& screenshot : m_screenshots)
			{
				i++;
				if (screenshot->getGlobalBounds().contains(event.touch.x, event.touch.y))
				{
					setCurrentScreenshot(i);
				}
			}

			if (m_currentApp->isInstalled())
			{
				if (m_textExecute.getGlobalBounds().contains(event.touch.x, event.touch.y))
				{
					TweenEngine::Tween::to(m_textExecute, util3ds::TweenText::OUTLINE_COLOR_RGB, 0.5f)
							.target(0, 180, 0)
							.repeatYoyo(1, 0)
							.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween* source) {
								std::string filename = _("sdmc:/3ds/%s/%s.3dsx", m_currentApp->getDirectory().c_str(), m_currentApp->getDirectory().c_str());
								if (pathExists(filename.c_str()))
								{
									bootApp(filename.substr(5).c_str(), "");
									requestStackClear();
								}
							})
							.start(m_tweenManager);
				}
				else if (m_textDelete.getGlobalBounds().contains(event.touch.x, event.touch.y))
				{
					Installer installer(m_currentApp, "");
					installer.uninstall();
					m_textDownload.setFillColor(cpp3ds::Color::White);
				}
			}
			else
			{
				if (m_textDownload.getGlobalBounds().contains(event.touch.x, event.touch.y))
				{
					if (m_downloads.isDownloading(m_currentApp) || m_downloads.isInstalling(m_currentApp))
					{
						m_downloads.cancelDownload(m_currentApp);
						TweenEngine::Tween::to(m_textDownload, util3ds::TweenText::FILL_COLOR_RGB, 0.5f)
								.target(255, 255, 255)
								.start(m_tweenManager);
					}
					else
					{
						m_downloads.addDownload(m_currentApp);
						m_tabs.getTab(1).setCaption(_("Downloads (%d)", m_downloads.getCount()));
						TweenEngine::Tween::to(m_textDownload, util3ds::TweenText::FILL_COLOR_RGB, 0.5f)
								.target(230, 20, 20)
								.start(m_tweenManager);
					}
				}
			}
		}
		else if (m_mode == Downloads)
		{
			m_downloads.processEvent(event);
		}

		m_tabs.processEvent(event);
		m_mode = (m_tabs.getSelectedIndex() == 1) ? Downloads : AppInfo;
	}
	else if (event.type == cpp3ds::Event::JoystickMoved)
	{
		m_descriptionVelocity = event.joystickMove.y * 2.f;
	}

	return true;
}


void BrowseState::setItemIndex(int index)
{
	if (index < 0)
		index = 0;
	else if (index >= m_appList.getCount())
		index = m_appList.getCount() - 1;

	float extra = std::abs(m_appList.getSelectedIndex() - index) == 8.f ? 2.f : 1.f;

	float pos = -200.f * extra * (index / 4);
	if (pos > m_appListPositionX)
		m_appListPositionX = pos;
	else if (pos <= m_appListPositionX - 400.f)
		m_appListPositionX = pos + 200.f * extra;

	TweenEngine::Tween::to(m_appList, AppList::POSITION_X, 0.3f)
			.target(m_appListPositionX)
			.start(m_tweenManager);

	m_appList.setSelectedIndex(index);
}

void BrowseState::setCurrentApp(AppItem *app)
{
	m_currentApp = app;

	if (app)
	{
		m_icon.setTexture(*app->getIcon(), true);
		m_textTitle.setString(app->getTitle());
		m_textTitle.setOrigin(m_textTitle.getLocalBounds().width / 2.f, 0.f);
		m_textAuthor.setString(app->getAuthor());
		m_textVersion.setString(app->getVersion());

		// Calculate word-wrapping for description
		std::stringstream ss(app->getLongDescription());
		std::stringstream description;
		std::string word;
		cpp3ds::Text tmpText = m_textDescription;
		float posX = 0;
		while (std::getline(ss, word, ' '))
		{
			tmpText.setString(word + " ");
			if (posX > 218.f - tmpText.getLocalBounds().width)
			{
				description << std::endl;
				posX = 0;
			}
			posX += tmpText.getLocalBounds().width;
			description << " " << word;
		}

		m_textDescription.setString(description.str());
		m_textDescription.setPosition(102.f, 49.f);

		m_textDownload.setFillColor(cpp3ds::Color::White);
	}
}

void BrowseState::loadApp()
{
	AppItem* item = m_appList.getSelected();
	m_tabs.setSelectedIndex(0);

	if (item->getScreenshots().empty())
	{
		m_screenshots.clear();
		m_screenshotTextures.clear();
	}
	else
	{
		requestStackPush(States::Loading);

		size_t oldCount = m_currentApp->getScreenshots().size();
		std::vector<std::unique_ptr<util3ds::TweenSprite>> newSprites;
		float startX = std::round((320.f - 61.f * item->getScreenshots().size()) / 2.f);

		int i = 0;
		for (auto& url : item->getScreenshots())
		{
			std::stringstream dest;
			dest << "sdmc:/3ds/BrewMan/cache/" << item->getDirectory() << "_screenshot" << i << ".bin";

			// Check if screenshot is already in cache
			if (!pathExists(dest.str().c_str()))
			{
				Download download(url, dest.str());
				std::cout << url << " - " << dest.str() << std::endl;
				download.run();
			}

			std::unique_ptr<cpp3ds::Texture> texture(new cpp3ds::Texture());
			std::unique_ptr<util3ds::TweenSprite> sprite(new util3ds::TweenSprite());
			texture->loadFromFile(dest.str());
			sprite->setTexture(*texture, true);

			sprite->setPosition(startX + i * 61.f, 167.f);
			sprite->setScale(0.15f, 0.15f);

			newSprites.emplace_back(std::move(sprite));
			m_screenshotTextures.emplace_back(std::move(texture));
			++i;
		}

		for (auto& sprite : newSprites)
			m_screenshots.emplace_back(std::move(sprite));

		for (i = 0; i < oldCount; ++i)
		{
			m_screenshots.erase(m_screenshots.begin());
			m_screenshotTextures.erase(m_screenshotTextures.begin());
		}

		requestStackPop();
	}

	setCurrentApp(item);
	setMode(AppInfo);
}


#define SCREENSHOT_BUTTON_FADEIN(object) \
	object.setFillColor(cpp3ds::Color(255,255,255,190)); \
	object.setOutlineColor(cpp3ds::Color(0,0,0,100)); \
	TweenEngine::Tween::from(object, util3ds::TweenText::FILL_COLOR_ALPHA, 4.f) \
		.target(0).start(m_tweenManager); \
	TweenEngine::Tween::from(object, util3ds::TweenText::OUTLINE_COLOR_ALPHA, 4.f) \
		.target(0).start(m_tweenManager);
#define SCREENSHOT_BUTTON_FADEOUT(object) \
	TweenEngine::Tween::to(object, util3ds::TweenText::FILL_COLOR_ALPHA, 0.5f) \
		.target(0).start(m_tweenManager); \
	TweenEngine::Tween::to(object, util3ds::TweenText::OUTLINE_COLOR_ALPHA, 0.5f) \
		.target(0).start(m_tweenManager);

void BrowseState::setCurrentScreenshot(int screenshotIndex)
{
	if (screenshotIndex != 0)
	{
		cpp3ds::Sprite* screenshot = m_screenshots[screenshotIndex-1].get();

		m_screenshotTop.setTexture(*screenshot->getTexture(), true);
		m_screenshotBottom.setTexture(*screenshot->getTexture(), true);

		// If another screenshot not already showing
		if (m_currentScreenshot == 0)
		{
			SCREENSHOT_BUTTON_FADEIN(m_close);
			SCREENSHOT_BUTTON_FADEIN(m_arrowLeft);
			SCREENSHOT_BUTTON_FADEIN(m_arrowRight);

			m_screenshotTop.setPosition(screenshot->getPosition() + cpp3ds::Vector2f(0.f, 270.f));
			m_screenshotTop.setScale(screenshot->getScale());
			m_screenshotBottom.setPosition(screenshot->getPosition());
			m_screenshotBottom.setScale(screenshot->getScale());

			TweenEngine::Tween::to(m_screenshotBottom, util3ds::TweenSprite::SCALE_XY, 0.5f)
					.target(0.18f, 0.18f)
					.ease(TweenEngine::TweenEquations::easeOutElastic)
					.start(m_tweenManager);
			TweenEngine::Tween::to(m_screenshotBottom, util3ds::TweenSprite::POSITION_XY, 0.5f)
					.targetRelative(-6.f, -6.f)
					.ease(TweenEngine::TweenEquations::easeOutElastic)
					.start(m_tweenManager);

			TweenEngine::Tween::to(m_screenshotTop, util3ds::TweenSprite::SCALE_XY, 0.7f)
					.target(1.f, 1.f)
					.delay(0.6f)
					.start(m_tweenManager);
			TweenEngine::Tween::to(m_screenshotTop, util3ds::TweenSprite::POSITION_XY, 0.7f)
					.target(0.f, 0.f)
					.delay(0.6f)
					.start(m_tweenManager);
			TweenEngine::Tween::to(m_screenshotBottom, util3ds::TweenSprite::SCALE_XY, 0.7f)
					.target(1.f, 1.f)
					.delay(0.6f)
					.start(m_tweenManager);
			TweenEngine::Tween::to(m_screenshotBottom, util3ds::TweenSprite::POSITION_XY, 0.7f)
					.target(-40.f, -240.f)
					.delay(0.6f)
					.start(m_tweenManager);
		}
	}
	else if (m_currentScreenshot != 0)
	{
		// Close screenshot
		cpp3ds::Sprite* screenshot = m_screenshots[m_currentScreenshot-1].get();

		SCREENSHOT_BUTTON_FADEOUT(m_close);
		SCREENSHOT_BUTTON_FADEOUT(m_arrowLeft);
		SCREENSHOT_BUTTON_FADEOUT(m_arrowRight);

		TweenEngine::Tween::to(m_screenshotTop, util3ds::TweenSprite::SCALE_XY, 0.7f)
				.target(0.15f, 0.15f)
				.start(m_tweenManager);
		TweenEngine::Tween::to(m_screenshotTop, util3ds::TweenSprite::POSITION_XY, 0.7f)
				.target(screenshot->getPosition().x, screenshot->getPosition().y + 270.f)
				.start(m_tweenManager);
		TweenEngine::Tween::to(m_screenshotBottom, util3ds::TweenSprite::SCALE_XY, 0.7f)
				.target(0.15f, 0.15f)
				.start(m_tweenManager);
		TweenEngine::Tween::to(m_screenshotBottom, util3ds::TweenSprite::POSITION_XY, 0.7f)
				.target(screenshot->getPosition().x, screenshot->getPosition().y)
				.setCallback(TweenEngine::TweenCallback::COMPLETE, [=](TweenEngine::BaseTween* source) {
					m_currentScreenshot = screenshotIndex;
				})
				.start(m_tweenManager);
		return;
	}

	m_currentScreenshot = screenshotIndex;
}


void BrowseState::setMode(BrowseState::Mode mode)
{
	if (mode == AppInfo)
	{
		//
	}
	else if (mode == Downloads)
	{

	}

	m_mode = mode;
}

} // namespace BrewMan
