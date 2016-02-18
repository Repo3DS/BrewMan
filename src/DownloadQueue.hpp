#ifndef BREWMAN_DOWNLOADQUEUE_HPP
#define BREWMAN_DOWNLOADQUEUE_HPP

#include <bits/unique_ptr.h>
#include "Download.hpp"
#include "AppItem.hpp"

namespace BrewMan {

class DownloadQueue {
public:
	~DownloadQueue();

	void addDownload(AppItem* app);

private:
	std::vector<std::unique_ptr<Download>> m_downloads;
};

} // namespace BrewMan

#endif // BREWMAN_DOWNLOADQUEUE_HPP
