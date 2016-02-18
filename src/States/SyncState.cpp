#include "SyncState.hpp"
#include <TweenEngine/Tween.h>
#include <cpp3ds/Window/Window.hpp>
#include <cpp3ds/System/I18n.hpp>
#include <cpp3ds/System/Service.hpp>
#include <dirent.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <git2/global.h>
#include <cpp3ds/System/FileSystem.hpp>
#include <git2/errors.h>
#include <git2/clone.h>
#include <git2/annotated_commit.h>
#include <git2/merge.h>
#include <git2/reset.h>

namespace
{
	int remove_directory(const char *path)
	{
		DIR *d = opendir(path);
		size_t path_len = strlen(path);
		int r = -1;

		if (d) {
			struct dirent *p;

			r = 0;

			while (!r && (p = readdir(d))) {
				int r2 = -1;
				char *buf;
				size_t len;
printf("%s\n", p->d_name);
				/* Skip the names "." and ".." as we don't want to recurse on them. */
				if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
					continue;

				len = path_len + strlen(p->d_name) + 2;
				buf = (char*) malloc(len);

				if (buf) {
					struct stat sb;
					snprintf(buf, len, "%s/%s", path, p->d_name);

					if (!stat(buf, &sb)) {
						if (S_ISDIR(sb.st_mode))
							r2 = remove_directory(buf);
						else
							r2 = unlink(buf);
					}

					free(buf);
				}

				r = r2;
			}

			closedir(d);
		}

		if (!r)
			r = rmdir(path);

		return r;
	}

}


namespace BrewMan {

SyncState::SyncState(StateStack& stack, Context& context)
: State(stack, context)
, m_thread(&SyncState::sync, this)
{
	git_libgit2_init();
	m_thread.launch();
//	sync();
}


SyncState::~SyncState()
{
//	m_thread.wait();
	git_libgit2_shutdown();
}


void SyncState::renderTopScreen(cpp3ds::Window& window)
{
	//
}

void SyncState::renderBottomScreen(cpp3ds::Window& window)
{
	// Nothing
}

bool SyncState::update(float delta)
{
	return true;
}

bool SyncState::processEvent(const cpp3ds::Event& event)
{
	if (event.type == cpp3ds::Event::KeyPressed)
	{
		if (event.key.code == cpp3ds::Keyboard::A)
			m_thread.launch();
		if (event.key.code == cpp3ds::Keyboard::B)
			requestStackClear();
	}
	return true;
}

git_oid SyncState::m_git_oid;

int SyncState::find_master(const char *ref_name, const char *remote_url, const git_oid *oid, unsigned int is_merge, void *payload)
{
	printf("%s\n", ref_name);
	if (is_merge) {
		printf("is_merge\n");
		m_git_oid = *oid;
//		memcpy(&oid, oid, sizeof(git_oid));
	} else {
//		return -1;
	}
	return 0;
}

void SyncState::sync()
{
	git_repository *repo = NULL;

	const char *url = "git://github.com/Repo3DS/ideal-enigma.git";
	const std::string path = cpp3ds::FileSystem::getFilePath(REPO_DIR);


//	return;

	git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
	opts.checkout_branch = "master";

	int error = git_clone(&repo, url, path.c_str(), NULL);
//	int error = 1;
	if (error < 0) {
		if (error == GIT_EEXISTS) {
			error = git_repository_open(&repo, path.c_str());
			if (error == 0) {
				git_remote *remote;
				error = git_remote_lookup(&remote, repo, "origin");
				if (error == 0) {
					error = git_remote_fetch(remote,
											 NULL,  /* refspecs, NULL to use the configured ones */
											 NULL,  /* options, empty for defaults */
											 "pull"); /* reflog mesage, usually "fetch" or "pull", you can leave it NULL for "fetch" */
					if (error == 0) {
						git_annotated_commit *our_head, *their_heads[1];
//									git_merge_options merge_opts;
						if (git_repository_fetchhead_foreach(repo, find_master, NULL) == 0)
						{
							git_annotated_commit_from_fetchhead(&their_heads[0], repo, "master", url, &m_git_oid);

							git_merge_analysis_t analysis;
							git_merge_preference_t prefs;
							printf("analyzing\n");
							git_merge_analysis(&analysis, &prefs, repo, (const git_annotated_commit**)their_heads, 1);

							printf("%d", (int)analysis);

							if (analysis & GIT_MERGE_ANALYSIS_UP_TO_DATE)
								printf("up to date\n");
							if (analysis & GIT_MERGE_ANALYSIS_FASTFORWARD) {
								printf("fast-forwarding\n");
//								if (git_merge(repo, (const git_annotated_commit **)their_heads, 1, NULL, NULL) == 0) {
//									printf("merged\n");

									git_reference *ref;
									git_reference *newref;
									if (git_reference_lookup(&ref, repo, "refs/heads/master") == 0) {
										if (git_reference_set_target(&newref, ref, &m_git_oid, "BrewMan pull: Fast-forward") == 0) {
											printf("set target\n");
										}
									}


									git_reset_from_annotated(repo, their_heads[0], GIT_RESET_HARD, NULL);

									git_reference_free(ref);
									git_repository_state_cleanup(repo);
//								} else {
//									printf("merged failed");
//								}
							}

							git_annotated_commit_free(their_heads[0]);
						}





					}
				}
			}
			git_repository_free(repo);
			const git_error *e = giterr_last();
			if (e) {
				printf("Error bleh %d/%d: %s\n", error, e->klass, e->message);
//				if (!remove_directory(path.c_str())) {
//					std::cout << "deleted dir: " << path << std::endl;
//					sync();
//				} else {
//					std::cout << "failed to remove" << std::endl;
//				}
				return;
			}
		}
		const git_error *e = giterr_last();
		if (e) {
			printf("Error bleh %d/%d: %s\n", error, e->klass, e->message);
			return;
		}
	}
	else
	{
		std::cout << "cloned successfully" << std::endl;
//		requestStackClear();
//		requestStackPush(States::Browse);
	}
	std::cout << "browsing..." << std::endl;
	requestStackClear();
	requestStackPush(States::Browse);
}

} // namespace BrewMan
