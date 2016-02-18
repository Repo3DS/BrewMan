#ifndef BREWMAN_INSTALLER_HPP
#define BREWMAN_INSTALLER_HPP

#include <string>
#include <cpp3ds/System/Thread.hpp>
#include "AppItem.hpp"

namespace BrewMan {

class Installer {
public:
	Installer(AppItem* app, const std::string& filename);

	void run();

private:
	std::string m_filename;
	cpp3ds::Thread m_thread;
	AppItem* m_app;
};

} // namespace BrewMan

#endif // BREWMAN_INSTALLER_HPP
