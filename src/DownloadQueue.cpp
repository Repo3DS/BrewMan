#include <iostream>
#include <cpp3ds/System/I18n.hpp>
#include <TweenEngine/Tween.h>
#include "DownloadQueue.hpp"
#include "Notification.hpp"
#include "Installer.hpp"

namespace BrewMan {


DownloadQueue::~DownloadQueue()
{
	// Cancel all downloads
	for (auto& download : m_downloads)
		delete download.second;
}


void DownloadQueue::addDownload(AppItem* app)
{
	const std::string destination = "sdmc:/3ds/BrewMan/tmp/"+app->getDirectory()+".bin";

	Download* download = new Download(app->getFilename(), destination);
	download->fillFromAppItem(app);
	download->setPosition(3.f, 240.f);

	cpp3ds::Clock clock;
	float count = 0;
	int total = 0;

	download->start([=](const void* data, size_t len, size_t processed, const cpp3ds::Http::Response& response) mutable
	{
		if (total == 0)
		{
			std::string length = response.getField("Content-Length");
			if (!length.empty())
				total = atoi(length.c_str());
		}
		else
		{
			download->setProgress(static_cast<float>(processed) / total / 2);
		}

		count += len;
		if (clock.getElapsedTime() > cpp3ds::seconds(1.f))
		{
			download->setProgressMessage(_("Downloading... %.1fkb/s", count / clock.getElapsedTime().asSeconds() / 1024.f));
			std::cout << len << " " << processed << " " << count / clock.getElapsedTime().asSeconds() / 1024 << "kb/s" << std::endl;
			count = 0;
			clock.restart();
		}

		return true;
	},
	[=](bool canceled) mutable
	{
		if (canceled)
		{
			download->setProgress(0.f);
			download->setProgressMessage("Canceled");
			return;
		}

		download->setProgressMessage("Preparing to install...");
		download->setProgress(0.5f);

		Installer installer(app, destination);
		installer.attachDownload(download);
		if (installer.isValid())
		{
			installer.install();

			// Installer will set this to true on success
			if (app->isInstalled())
			{
				download->setProgress(1.f);
				Notification::spawn(app->getTitle() + " install finished.");
				download->setProgressMessage("Installed!");
			}
			else
			{
				download->setProgress(0.f);

				if (!download->isCanceled())
					Notification::spawn(app->getTitle() + " failed to install.");
			}
		}
		else
		{
			Notification::spawn("File validation failed: " + app->getTitle());
			download->setProgressMessage("Validation failed. Try again.");
		}

		// TODO: start next download in queue
	});

	Notification::spawn(_("Queued %s for install.", app->getTitle().c_str()));

	m_downloads.emplace_back(std::make_pair(app, download));
	realign();
}


void DownloadQueue::draw(cpp3ds::RenderTarget &target, cpp3ds::RenderStates states) const
{
	for (auto& download : m_downloads)
	{
		target.draw(*download.second, states);
	}
}


void DownloadQueue::cancelDownload(AppItem *app)
{
	for (auto it = m_downloads.begin(); it != m_downloads.end(); ++it)
	{
		if (it->first == app)
		{
			Notification::spawn(_("Canceled: %s", app->getTitle().c_str()));
			it->second->cancel(false);
			break;
		}
	}
}


bool DownloadQueue::isDownloading(AppItem *app)
{
	for (auto& download : m_downloads)
	{
		if (download.first == app && download.second->getProgress() < 0.5f)
			return true;
	}
	return false;
}


bool DownloadQueue::isInstalling(AppItem *app)
{
	for (auto& download : m_downloads)
	{
		if (download.first == app && download.second->getProgress() >= 0.5f && download.second->getProgress() < 1.f)
			return true;
	}
	return false;
}


bool DownloadQueue::processEvent(const cpp3ds::Event &event)
{
	for (auto& download : m_downloads)
		download.second->processEvent(event);
	return true;
}

void DownloadQueue::realign()
{
	for (int i = 0; i < m_downloads.size(); ++i)
	{
		TweenEngine::Tween::to(*m_downloads[i].second, util3ds::TweenSprite::POSITION_Y, 0.2f)
			.target(33.f + i * 35.f)
			.start(m_tweenManager);
	};
}

void DownloadQueue::update(float delta)
{
	// Remove downloads marked for delete
	bool changed = false;
	for (auto it = m_downloads.begin(); it != m_downloads.end();)
	{
		if (it->second->markedForDelete())
		{
			delete it->second;
			m_downloads.erase(it);
			changed = true;
		}
		else
			++it;
	}

	if (changed)
		realign();

	m_tweenManager.update(delta);
}

size_t DownloadQueue::getCount()
{
	return m_downloads.size();
}

size_t DownloadQueue::getActiveCount()
{
	size_t count = 0;
	for (auto& download : m_downloads)
		if (download.second->getProgress() < 1.f)
			++count;
	return count;
}

} // namespace BrewMan
