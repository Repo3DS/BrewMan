#ifndef BREWMAN_DOWNLOAD_HPP
#define BREWMAN_DOWNLOAD_HPP

#include <functional>
#include <cpp3ds/System/Thread.hpp>
#include <cpp3ds/Network/Http.hpp>
#include <cpp3ds/System/Clock.hpp>


namespace BrewMan {

typedef std::function<bool(const void*,size_t,size_t)> DownloadDataCallback;
typedef std::function<void()> DownloadFinishCallback;

class Download {
public:
	Download(const std::string& url, const std::string& destination);

	void setUrl(const std::string& url);

	void start(cpp3ds::Http::RequestCallback onData, DownloadFinishCallback onFinish);

	void run();

private:
	cpp3ds::Http m_http;
	cpp3ds::Http::Request m_request;
	std::string m_host;
	std::string m_uri;
	std::string m_destination;
	cpp3ds::Http::RequestCallback m_onData;
	DownloadFinishCallback m_onFinish;
	cpp3ds::Thread m_thread;

	bool m_cancelFlag;
	FILE* m_file;
	std::vector<char> m_buffer;
};

} // namespace BrewMan


#endif // BREWMAN_DOWNLOAD_HPP
