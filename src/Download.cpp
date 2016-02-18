#include <iostream>
#include <cpp3ds/System/FileSystem.hpp>
#include "Download.hpp"

namespace BrewMan {


Download::Download(const std::string &url, const std::string &destination)
: m_thread(&Download::run, this)
, m_destination(destination)
{
	setUrl(url);
}


void Download::setUrl(const std::string &url)
{
	size_t pos;

	pos = url.find("://");
	if (pos != std::string::npos) {
		pos = url.find("/", pos + 3);
		if (pos != std::string::npos) {
			m_host = url.substr(0, pos);
			m_uri = url.substr(pos);
		}
	}

	m_http.setHost(m_host);
	m_request.setUri(m_uri);
}


void Download::start(cpp3ds::Http::RequestCallback onData, DownloadFinishCallback onFinish)
{
	if (m_destination.empty())
		return;

	m_cancelFlag = false;
	m_buffer.clear();

	m_onData = onData;
	m_onFinish = onFinish;
	m_thread.launch();
}


void Download::run()
{
	cpp3ds::Http::Response response;

	m_file = fopen(cpp3ds::FileSystem::getFilePath(m_destination).c_str(), "w");

	cpp3ds::Http::RequestCallback dataCallback = [&](const void* data, size_t len, size_t processed, const cpp3ds::Http::Response& response)
	{
		if (response.getStatus() == 200)
		{
			const char *bufdata = reinterpret_cast<const char*>(data);
			m_buffer.insert(m_buffer.end(), bufdata, bufdata + len);

			if (m_buffer.size() > 1024 * 50)
			{
				fwrite(&m_buffer[0], sizeof(char), m_buffer.size(), m_file);
				m_buffer.clear();
			}
		}

		return !m_cancelFlag && m_onData(data, len, processed, response);
	};

	// Follow all redirects
	response = m_http.sendRequest(m_request, cpp3ds::Time::Zero, dataCallback);
	while (response.getStatus() == 301 || response.getStatus() == 302)
	{
		setUrl(response.getField("Location"));
		response = m_http.sendRequest(m_request, cpp3ds::Time::Zero, dataCallback);
	}

	// Write remaining buffer and close downloaded file
	if (m_buffer.size() > 0)
		fwrite(&m_buffer[0], sizeof(char), m_buffer.size(), m_file);
	fclose(m_file);

	m_onFinish();
}


} // namespace BrewMan
