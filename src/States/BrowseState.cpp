#include "BrowseState.hpp"
#include "SyncState.hpp"
#include "../Notification.hpp"
#include "../AssetManager.hpp"
#include <TweenEngine/Tween.h>
#include <cpp3ds/Window/Window.hpp>


namespace BrewMan {

BrowseState::BrowseState(StateStack& stack, Context& context)
: State(stack, context)
, m_appList(std::string(REPO_DIR)+"/apps/")
, m_appListPositionX(0.f)
, m_currentApp(nullptr)
{
//	m_transform.translate(0.f, 30.f);

	m_textOrderAlpha.setFont(AssetManager<cpp3ds::Font>::get("fonts/fontawesome.ttf"));
	m_textOrderAlpha.setString(L"\uf15d");
//	m_textOrderAlpha.setString(L"\uf15e");
	m_textOrderAlpha.setColor(cpp3ds::Color::Green);
	m_textOrderAlpha.setCharacterSize(16);
	m_textOrderAlpha.setPosition(200.f, 2.f);

	m_icon.setPosition(5.f, 5.f);
	m_icon.setScale(2.f, 2.f);

	setCurrentApp(m_appList.getSelected());
}

void BrowseState::renderTopScreen(cpp3ds::Window& window)
{
	window.draw(m_appList);
}

void BrowseState::renderBottomScreen(cpp3ds::Window& window)
{
	if (m_currentApp)
	{
		window.draw(m_icon, m_transform);
	}

	window.draw(m_textOrderAlpha);
}

bool BrowseState::update(float delta)
{
	m_tweenManager.update(delta);
	return true;
}

bool BrowseState::processEvent(const cpp3ds::Event& event)
{
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
				AppItem* item = m_appList.getSelected();
				setCurrentApp(item);
//				m_downloads.addDownload(item);
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
	}
}

} // namespace BrewMan
