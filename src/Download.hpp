#ifndef BREWMAN_DOWNLOAD_HPP
#define BREWMAN_DOWNLOAD_HPP

#include <functional>
#include <cpp3ds/System/Thread.hpp>
#include <cpp3ds/Network/Http.hpp>
#include <cpp3ds/System/Clock.hpp>
#include <cpp3ds/Graphics/Sprite.hpp>
#include <cpp3ds/Window/Event.hpp>
#include "TweenObjects.hpp"
#include "GUI/NinePatch.hpp"
#include "AppItem.hpp"


namespace BrewMan {

typedef std::function<bool(const void*,size_t,size_t)> DownloadDataCallback;
typedef std::function<void(bool)> DownloadFinishCallback;

class Download : public cpp3ds::Drawable, public util3ds::TweenTransformable<cpp3ds::Transformable>{
public:
	Download(const std::string& url, const std::string& destination);
	~Download();

	void setProgressMessage(const std::string& message);
	const std::string& getProgressMessage() const;

	void setProgress(float progress);
	float getProgress() const;

	void setSize(float width, float height);

	void fillFromAppItem(AppItem* app);

	void processEvent(const cpp3ds::Event& event);

	void setUrl(const std::string& url);

	void start(cpp3ds::Http::RequestCallback onData, DownloadFinishCallback onFinish);

	void run();

	void cancel(bool wait = true);
	bool isCanceled();

	bool markedForDelete();

protected:
	virtual void draw(cpp3ds::RenderTarget &target, cpp3ds::RenderStates states) const;

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
	bool m_markedForDelete;
	FILE* m_file;
	std::vector<char> m_buffer;

	// For queue UI
	gui3ds::NinePatch m_background;
	cpp3ds::RectangleShape m_icon;
	cpp3ds::RectangleShape m_progressBar;

	cpp3ds::Text m_textTitle;
	cpp3ds::Text m_textProgress;

	util3ds::TweenText m_textCancel;

	float m_progress;
	cpp3ds::Vector2f m_size;
	std::string m_progressMessage;
};

} // namespace BrewMan


#endif // BREWMAN_DOWNLOAD_HPP
