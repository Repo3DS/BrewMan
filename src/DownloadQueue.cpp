#include <iostream>
#include <cpp3ds/System/I18n.hpp>
#include "DownloadQueue.hpp"
#include "Notification.hpp"
#include "Installer.hpp"

namespace BrewMan {


DownloadQueue::~DownloadQueue()
{
	// Kill all threads
}


void DownloadQueue::addDownload(AppItem* app)
{
	const std::string destination = "sdmc:/3ds/BrewMan/tmp/"+app->getDirectory()+".bin";

	std::unique_ptr<Download> download(new Download(app->getFilename(), destination));

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
			app->setProgress(static_cast<float>(processed) / total);
		}

		count += len;
		if (clock.getElapsedTime() > cpp3ds::seconds(1.f))
		{
			std::cout << len << " " << processed << " " << count / clock.getElapsedTime().asSeconds() / 1024 << "kb/s" << std::endl;
			count = 0;
			clock.restart();
		}
		return true;
	},
	[=]
	{
		app->setProgress(0.f);
		Notification::spawn(_("%s Downloaded.", app->getTitle().c_str()));
		Installer installer(app, destination);
		installer.run();
	});

	Notification::spawn(_("Queued %s for install.", app->getTitle().c_str()));

	m_downloads.emplace_back(std::move(download));
}


} // namespace BrewMan
