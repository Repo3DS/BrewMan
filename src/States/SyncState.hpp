#ifndef BREWMAN_SYNCSTATE_HPP
#define BREWMAN_SYNCSTATE_HPP

#include "State.hpp"
#include "../TweenObjects.hpp"
#include <cpp3ds/Graphics/Sprite.hpp>
#include <cpp3ds/Graphics/Text.hpp>
#include <TweenEngine/TweenManager.h>
#include <git2/oid.h>
#include <cpp3ds/System/Thread.hpp>
#include <cpp3ds/System/Clock.hpp>

#define REPO_DIR "sdmc:/3ds/BrewMan/repo"


namespace BrewMan {

class SyncState : public State
{
public:
	SyncState(StateStack& stack, Context& context);
	~SyncState();

	virtual void renderTopScreen(cpp3ds::Window& window);
	virtual void renderBottomScreen(cpp3ds::Window& window);
	virtual bool update(float delta);
	virtual bool processEvent(const cpp3ds::Event& event);
	void sync();

private:
	bool autoUpdate();
	void setStatus(const std::string& message);
	static int find_master(const char *ref_name, const char *remote_url, const git_oid *oid, unsigned int is_merge, void *payload);

	static git_oid m_git_oid;
	cpp3ds::Thread m_thread;
	cpp3ds::Clock m_timer;
	util3ds::TweenText m_textStatus;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace BrewMan

#endif // BREWMAN_SYNCSTATE_HPP
