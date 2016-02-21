#include "BrowseState.hpp"
#include "SyncState.hpp"
#include "../Notification.hpp"
#include "../AssetManager.hpp"
#include <TweenEngine/Tween.h>
#include <cpp3ds/Window/Window.hpp>
#include <sys/stat.h>
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
{
	m_textOrderAlpha.setFont(AssetManager<cpp3ds::Font>::get("fonts/fontawesome.ttf"));
	m_textOrderAlpha.setString(L"hello \uf15d");
//	m_textOrderAlpha.setString(L"\uf15e");
	m_textOrderAlpha.setFillColor(cpp3ds::Color::White);
	m_textOrderAlpha.setOutlineColor(cpp3ds::Color::Black);
	m_textOrderAlpha.setOutlineThickness(2.f);
	m_textOrderAlpha.setCharacterSize(16);
	m_textOrderAlpha.setPosition(200.f, 2.f);

	m_textDownload = m_textOrderAlpha;
	m_textDownload.setString(L"\uf019");
//	m_textDownload.setString(L"\uf1f8");
	m_textDownload.setCharacterSize(30);
	m_textDownload.setPosition(67.f, 93.f);

	TweenEngine::Tween::to(m_textDownload, util3ds::TweenText::OUTLINE_COLOR_RGB, 0.3f)
			.target(100, 100, 100)
			.repeatYoyo(-1, 0.f)
			.start(m_tweenManager);

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

	m_fadeShape.setTexture(&AssetManager<cpp3ds::Texture>::get("images/fade.png"));
	m_fadeShape.setSize(cpp3ds::Vector2f(250.f, 8.f));
	m_fadeShape.setOrigin(m_fadeShape.getSize());
//	m_fadeShape.setFillColor(cpp3ds::Color::Black);
	m_fadeShape.setRotation(180.f);
	m_fadeShape.setPosition(102.f, 46.f);

	m_icon.setPosition(2.f, 30.f);
	m_icon.setScale(2.f, 2.f);

	m_descriptionView.reset(cpp3ds::FloatRect(0.f, 46.f, 320.f, 120.f));
	m_descriptionView.setViewport(cpp3ds::FloatRect(0.f, 46.f / 240.f, 1.f, 120.f / 240.f));

	setCurrentApp(m_appList.getSelected());
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

		if (m_currentApp->isInstalled())
		{

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

	window.draw(m_textOrderAlpha);

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
	m_textDescription.move(0.f, m_descriptionVelocity * delta);
	if (m_textDescription.getPosition().y < 49.f - m_textDescription.getLocalBounds().height + 110.f)
		m_textDescription.setPosition(102.f, 49.f - m_textDescription.getLocalBounds().height + 110.f);
	if (m_textDescription.getPosition().y > 49.f)
		m_textDescription.setPosition(102.f, 49.f);
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
//				m_appList.setSelectedIndex(6);
//				AppItem* item = m_appList.getSelected();
//				setCurrentApp(item);
//				m_downloads.addDownload(item);
//				m_loadThread.launch();
				loadApp();
				break;
			}
			case cpp3ds::Keyboard::B: {
				Notification::spawn("Download completed test."); break;
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
			case cpp3ds::Keyboard::X:
				setItemIndex(index - 4);
				break;
			case cpp3ds::Keyboard::DPadRight:
			case cpp3ds::Keyboard::Y:
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
		int i = 0;
		for (auto& screenshot : m_screenshots)
		{
			i++;
			if (screenshot->getGlobalBounds().contains(event.touch.x, event.touch.y))
			{
				setCurrentScreenshot(i);
			}
		}

		if (m_textDownload.getGlobalBounds().contains(event.touch.x, event.touch.y))
		{
//			AppItem* item = m_appList.getSelected();
//			m_downloads.addDownload(m_currentApp);
			if (true)
			{
				TweenEngine::Tween::to(m_textDownload, util3ds::TweenText::FILL_COLOR_RGB, 0.5f)
						.target(230, 20, 20)
						.start(m_tweenManager);
			}
			else
			{
				TweenEngine::Tween::to(m_textDownload, util3ds::TweenText::FILL_COLOR_RGB, 0.5f)
						.target(0, 0, 0)
						.start(m_tweenManager);
			}
		}
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

	m_appList.setSelectedIndex(index);

	float pos = -200.f * (index / 4);
	if (pos > m_appListPositionX)
		m_appListPositionX = pos;
	else if (pos <= m_appListPositionX - 400.f)
		m_appListPositionX = pos + 200.f;

	TweenEngine::Tween::to(m_appList, AppList::POSITION_X, 0.3f)
			.target(m_appListPositionX)
			.start(m_tweenManager);
}

void BrowseState::setCurrentApp(AppItem *app)
{
	m_currentApp = app;

	if (app)
	{
		m_icon.setTexture(*app->getIcon(), true);
		m_textTitle.setString(app->getTitle());
		m_textTitle.setOrigin(m_textTitle.getLocalBounds().width / 2.f, 0.f);

		// Calculate word-wrapping for description
		std::stringstream ss(app->getLongDescription());
		std::stringstream description;
		std::string word;
		cpp3ds::Text tmpText = m_textDescription;
		float posX = 0;
		while (ss >> word)
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
//		m_textDescription.setString(app->getLongDescription());

		m_textDownload.setFillColor(cpp3ds::Color::White);
	}
}

void BrowseState::loadApp()
{
	AppItem* item = m_appList.getSelected();

//	setCurrentApp(nullptr);
//	m_screenshots.clear();
//	m_screenshotTextures.clear();


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
		float startX = (320.f - 61.f * item->getScreenshots().size()) / 2.f;

		int i = 0;
		for (auto& url : item->getScreenshots())
		{
			std::stringstream dest;
			dest << "sdmc:/3ds/BrewMan/cache/" << item->getDirectory() << "_screenshot" << i << ".bin";

			// Check if screenshot is already in cache
			struct stat buffer;
			if (stat(cpp3ds::FileSystem::getFilePath(dest.str()).c_str(), &buffer))
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
//			m_screenshots.emplace_back(std::move(sprite));
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
//	std::cout << "thread count: " << m_screenshots.size() << std::endl;
	setCurrentApp(item);
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

} // namespace BrewMan
