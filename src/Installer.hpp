#ifndef BREWMAN_INSTALLER_HPP
#define BREWMAN_INSTALLER_HPP

#include <string>
#include <cpp3ds/System/Thread.hpp>
#include "AppItem.hpp"
#include "Download.hpp"

namespace BrewMan {

class Installer {
public:
	Installer(AppItem* app, const std::string& filename);
	bool isValid();
	void install();
	void uninstall();
	void attachDownload(Download* download);

private:
	void writeFilelist();
	void readFileList();

private:
	Download* m_download;
	std::string m_filename;
	cpp3ds::Thread m_thread;
	AppItem* m_app;
	std::string m_filelistPath;
	std::vector<std::string> m_filelist;
};

} // namespace BrewMan

#endif // BREWMAN_INSTALLER_HPP
