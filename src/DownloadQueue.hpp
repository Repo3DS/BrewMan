#ifndef BREWMAN_DOWNLOADQUEUE_HPP
#define BREWMAN_DOWNLOADQUEUE_HPP

#include <bits/unique_ptr.h>
#include <cpp3ds/Window/Event.hpp>
#include <TweenEngine/TweenManager.h>
#include "Download.hpp"
#include "AppItem.hpp"

namespace BrewMan {

class DownloadQueue : public cpp3ds::Drawable{
public:
	~DownloadQueue();

	void addDownload(AppItem* app);
	void cancelDownload(AppItem* app);
	bool isDownloading(AppItem* app);
	bool isInstalling(AppItem* app);

	size_t getCount();
	size_t getActiveCount();

	void update(float delta);
	bool processEvent(const cpp3ds::Event& event);

protected:
	void realign();
	virtual void draw(cpp3ds::RenderTarget& target, cpp3ds::RenderStates states) const;

private:
	std::vector<std::pair<AppItem*, Download*>> m_downloads;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace BrewMan

#endif // BREWMAN_DOWNLOADQUEUE_HPP
