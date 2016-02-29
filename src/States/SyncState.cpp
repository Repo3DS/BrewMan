#include "SyncState.hpp"
#include "../Download.hpp"
#include "../Installer.hpp"
#include "../boot.h"
#include "../version.h"
#include "../Util.hpp"
#include <TweenEngine/Tween.h>
#include <cpp3ds/Window/Window.hpp>
#include <cpp3ds/System/I18n.hpp>
#include <git2/global.h>
#include <cpp3ds/System/FileSystem.hpp>
#include <git2/errors.h>
#include <git2/clone.h>
#include <git2/annotated_commit.h>
#include <git2/merge.h>
#include <git2/reset.h>
#include <cpp3ds/System/Sleep.hpp>
#include <sys/stat.h>
#include <fstream>


namespace BrewMan {

SyncState::SyncState(StateStack& stack, Context& context)
: State(stack, context)
, m_thread(&SyncState::sync, this)
{
	m_textStatus.setCharacterSize(14);
	m_textStatus.setFillColor(cpp3ds::Color::White);
	m_textStatus.setOutlineColor(cpp3ds::Color(0, 0, 0, 70));
	m_textStatus.setOutlineThickness(2.f);
	m_textStatus.setPosition(160.f, 155.f);
	TweenEngine::Tween::to(m_textStatus, util3ds::TweenText::OUTLINE_COLOR_ALPHA, 0.15f)
			.target(90)
			.repeatYoyo(-1, 0)
			.start(m_tweenManager);

	git_libgit2_init();
	m_thread.launch();
}


SyncState::~SyncState()
{
	m_thread.wait();
	git_libgit2_shutdown();
}


void SyncState::renderTopScreen(cpp3ds::Window& window)
{
	// Nothing
}

void SyncState::renderBottomScreen(cpp3ds::Window& window)
{
	window.draw(m_textStatus);
}

bool SyncState::update(float delta)
{
	m_tweenManager.update(delta);
	return true;
}

bool SyncState::processEvent(const cpp3ds::Event& event)
{
	return true;
}

git_oid SyncState::m_git_oid;

int SyncState::find_master(const char *ref_name, const char *remote_url, const git_oid *oid, unsigned int is_merge, void *payload)
{
	if (is_merge)
		m_git_oid = *oid;
	return 0;
}

void SyncState::sync()
{
	m_timer.restart();

	// Set up directory structure, if necessary
	std::string tmpPath = cpp3ds::FileSystem::getFilePath("sdmc:/3ds/BrewMan/tmp");
	std::string cachePath = cpp3ds::FileSystem::getFilePath("sdmc:/3ds/BrewMan/cache");
	std::string installedPath = cpp3ds::FileSystem::getFilePath("sdmc:/3ds/BrewMan/installed");
	if (pathExists(tmpPath.c_str(), false))
		removeDirectory(tmpPath.c_str());
	mkdir(tmpPath.c_str(), 0777);
	if (!pathExists(cachePath.c_str(), false))
		mkdir(cachePath.c_str(), 0777);
	if (!pathExists(installedPath.c_str(), false))
		mkdir(installedPath.c_str(), 0777);

	// If auto-dated, boot into newest BrewMan
	if (autoUpdate())
	{
		char buf[256];
		size_t len;
		FILE *src = fopen("sdmc:/3ds/BrewMan/tmp/update.3dsx", "rb");
		FILE *dst = fopen("sdmc:/3ds/BrewMan/tmp/update-copy.3dsx", "wb");
		while ((len = fread(buf, 1, sizeof(buf), src)) > 0)
			fwrite(buf, 1, sizeof(buf), dst);
		fclose(src);
		fclose(dst);

		bootApp("/3ds/BrewMan/tmp/update.3dsx", "");
		requestStackClear();
		return;
	}

	git_repository *repo = NULL;
	const char *repoUrl = "git://github.com/Repo3DS/ideal-enigma.git";
	const std::string path = cpp3ds::FileSystem::getFilePath(REPO_DIR);

	git_clone_options opts = GIT_CLONE_OPTIONS_INIT;

	setStatus("Fetching git repo...");

	int error = git_clone(&repo, repoUrl, path.c_str(), NULL);

	if (error < 0)
	{
		if (error == GIT_EEXISTS) {
			error = git_repository_open(&repo, path.c_str());
			if (error == 0) {
				git_remote *remote;
				error = git_remote_lookup(&remote, repo, "origin");
				if (error == 0) {
					error = git_remote_fetch(remote, NULL, NULL, "pull");
					if (error == 0)
					{
						git_annotated_commit *our_head, *their_heads[1];
						if (git_repository_fetchhead_foreach(repo, find_master, NULL) == 0)
						{
							git_annotated_commit_from_fetchhead(&their_heads[0], repo, "master", repoUrl, &m_git_oid);

							git_merge_analysis_t analysis;
							git_merge_preference_t prefs;
							git_merge_analysis(&analysis, &prefs, repo, (const git_annotated_commit**)their_heads, 1);

							if (analysis & GIT_MERGE_ANALYSIS_UP_TO_DATE)
								printf("up to date\n");
							else if (analysis & GIT_MERGE_ANALYSIS_FASTFORWARD)
							{
								printf("fast-forwarding\n");
//								if (git_merge(repo, (const git_annotated_commit **)their_heads, 1, NULL, NULL) == 0) {

								git_reference *ref;
								git_reference *newref;
								if (git_reference_lookup(&ref, repo, "refs/heads/master") == 0)
									git_reference_set_target(&newref, ref, &m_git_oid, "BrewMan pull: Fast-forward");

								git_reset_from_annotated(repo, their_heads[0], GIT_RESET_HARD, NULL);

								git_reference_free(ref);
								git_repository_state_cleanup(repo);
							}

							git_annotated_commit_free(their_heads[0]);
						}
					}
				}
			}
			git_repository_free(repo);
			const git_error *e = giterr_last();
			if (e) {
				setStatus(_("Error %d/%d\n%s\nCloning repo again...", error, e->klass, e->message));
				if (!removeDirectory(path.c_str())) {
					cpp3ds::sleep(cpp3ds::seconds(2.f));
					sync();
				} else {
					setStatus("Repo failure. Please report this error.\nAnd delete /3ds/BrewMan/repo/ and try again.");
				}
				return;
			}
		}
		const git_error *e = giterr_last();
		if (e) {
			setStatus(_("Error %d/%d: %s", error, e->klass, e->message));
			return;
		}
	}

	setStatus("Everything up-to-date!");

	// Give the Title animation time to finish if necessary
	while (m_timer.getElapsedTime() < cpp3ds::seconds(5.f))
		cpp3ds::sleep(cpp3ds::milliseconds(50));

	requestStackClear();
	requestStackPush(States::Browse);
}


bool SyncState::autoUpdate()
{
	const char* versionUrl = "https://raw.githubusercontent.com/Repo3DS/BrewMan/master/version.yml";
	const char* versionFilename = "sdmc:/3ds/BrewMan/tmp/version.yml";
	const char* updateFilename = "sdmc:/3ds/BrewMan/tmp/update.bin";
	setStatus("Checking for BrewMan update...");

	Download download(versionUrl, versionFilename);
	download.run();

	YAML::Node yaml = YAML::LoadFile(cpp3ds::FileSystem::getFilePath(versionFilename));
	std::string version = yaml["version"].as<std::string>();
	std::string fileUrl = yaml["file"].as<std::string>();

	if (version != BREWMAN_VERSION)
	{
		setStatus(_("Downloading BrewMan %s ...", version.c_str()));
		Download download(fileUrl, updateFilename);
		download.run();

		AppItem item;
		item.setFilesize(yaml["filesize"].as<size_t>());
		item.setFileMD5(yaml["file-md5"].as<std::string>());
		item.setDirectory("BrewMan");
		Installer installer(&item, updateFilename);
		if (installer.isValid())
		{
			setStatus(_("Installing BrewMan %s ...", version.c_str()));
			installer.install();
			return true;
		}
		else
			setStatus(_("Failed to install BrewMan %s!", version.c_str()));
	}

	return false;
}


void SyncState::setStatus(const std::string &message)
{
	m_textStatus.setString(message);
	cpp3ds::FloatRect rect = m_textStatus.getLocalBounds();
	m_textStatus.setOrigin(rect.width / 2.f, rect.height / 2.f);
}

} // namespace BrewMan
