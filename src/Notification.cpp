#include <TweenEngine/Tween.h>
#include "Notification.hpp"

namespace BrewMan {

// Static members
cpp3ds::Texture Notification::m_texture;
float Notification::m_spawnPositionY = 240.f;
std::vector<std::unique_ptr<Notification>> Notification::notifications;


Notification::Notification()
: m_markForDelete(false)
{
	if (m_texture.getSize().x == 0)
		m_texture.loadFromFile("images/notification.9.png");
	setTexture(&m_texture);
	getText().setCharacterSize(12);
}


Notification::~Notification()
{
	m_tweenManager.killAll();
}


void Notification::update(float delta)
{
	for (auto i = notifications.begin(); i != notifications.end();) {
		if ((*i)->m_markForDelete) {
			float offsetY = (*i)->getSize().y + NOTIFICATION_SPACING;
			m_spawnPositionY += offsetY;
			for (auto j = i+1; j != notifications.end(); j++) {
				auto notification = j->get();
				notification->m_destinationY += offsetY;
				TweenEngine::Tween::to(*notification, POSITION_Y, 0.3f)
					.target(notification->m_destinationY)
					.start(notification->m_tweenManager);
			}
			notifications.erase(i);
		} else {
			++i;
		}
	}
	for (auto& notification : notifications)
		notification->m_tweenManager.update(delta);
}


void Notification::spawn(cpp3ds::String message)
{
	std::unique_ptr<Notification> notification(new Notification());
	notification->setString(message);
	notification->setPosition(std::round(200.f - notification->getSize().x / 2.f),
	                          std::round(m_spawnPositionY - notification->getSize().y - 4.f));
	m_spawnPositionY -= NOTIFICATION_SPACING + notification->getSize().y;
	notification->animate();
	notifications.emplace_back(std::move(notification));
}


void Notification::animate()
{
	setColor(cpp3ds::Color(255, 255, 255, 0));
	setTextColor(cpp3ds::Color(60, 60, 60, 0));
	m_destinationY = getPosition().y;
	move(0, -10.f);
	TweenEngine::Tween::to(*this, POSITION_Y, 0.5f).target(m_destinationY).start(m_tweenManager);
	TweenEngine::Tween::to(*this, COLOR_ALPHA, 0.5f).target(225.f).start(m_tweenManager);
	TweenEngine::Tween::to(*this, TEXTCOLOR_ALPHA, 0.5f).target(255.f).start(m_tweenManager);

	// Fade out and mark notification for deletion in update()
	TweenEngine::Tween::to(*this, COLOR_ALPHA, 0.5f).target(0.f).delay(NOTIFICATION_DURATION).start(m_tweenManager);
	TweenEngine::Tween::to(*this, TEXTCOLOR_ALPHA, 0.5f)
		.target(0.f)
		.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween* source) {
			m_markForDelete = true;
		})
		.delay(NOTIFICATION_DURATION).start(m_tweenManager);
}

} // namespace BrewMan
