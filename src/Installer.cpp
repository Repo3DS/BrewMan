#include <archive.h>
#include <iostream>
#include <cpp3ds/System/FileSystem.hpp>
#include <archive_entry.h>
#include <mbedtls/md5.h>
#include <cpp3ds/System/I18n.hpp>
#include <fstream>
#include <iomanip>
#include "Installer.hpp"
#include "Util.hpp"


namespace {

static int copy_data(struct archive *ar, struct archive *aw, BrewMan::AppItem* app, BrewMan::Download* download)
{
	int r;
	const void *buff;
	size_t size;
	int64_t offset;

	while (1)
	{
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r < ARCHIVE_OK)
			return (r);
		r = archive_write_data_block(aw, buff, size, offset);
		if (r < ARCHIVE_OK)
		{
			fprintf(stderr, "%s\n", archive_error_string(aw));
			return (r);
		}

		if (download)
		{
			float progress = 0.5 + 0.5f * archive_position_compressed(ar) / app->getFilesize();
			if (progress >= 1.f)
				progress = 0.999f;
			download->setProgress(progress);

			if (download->isCanceled())
				break;
		}
	}
}

} // namespace


namespace BrewMan {

Installer::Installer(AppItem *app, const std::string& filename)
: m_thread(&Installer::install, this)
, m_app(app)
, m_filename(filename)
, m_download(nullptr)
{
	m_filelistPath = cpp3ds::FileSystem::getFilePath(_("sdmc:/3ds/BrewMan/installed/%s.list", m_app->getDirectory().c_str()));
}

void Installer::install()
{
	if (!isValid())
		return;

	m_filelist.clear();

	struct archive *a;
	struct archive *ext;
	struct archive_entry *entry;
	int r;

	a = archive_read_new();
	archive_read_support_format_all(a);
//	archive_read_support_compression_all(a);
	archive_read_support_compression_compress(a);
	archive_read_support_compression_bzip2(a);
	archive_read_support_compression_xz(a);
	ext = archive_write_disk_new();
	if ((r = archive_read_open_filename(a, cpp3ds::FileSystem::getFilePath(m_filename).c_str(), 10240))) {
		std::cout << "failure! " << r << std::endl;
		return;
	}

	while(1)
	{
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r < ARCHIVE_OK)
			fprintf(stderr, "%s\n", archive_error_string(a));
		// TODO: handle these fatal error
		// Fatal error/failure encountered, must terminate
//		if (r < ARCHIVE_WARN)
//			exit(1);
		m_filelist.push_back(archive_entry_pathname(entry));

		std::string path = cpp3ds::FileSystem::getFilePath(std::string("sdmc:/") + archive_entry_pathname(entry));

		// BrewMan's auto-updating is the only one that overwrites files
		if (pathExists(path.c_str(), false))
		{
			if (m_app->getDirectory() == "BrewMan")
			{
				if (unlink(path.c_str()) && path == "sdmc:/3ds/BrewMan/BrewMan.3dsx")
					path = "sdmc:/3ds/BrewMan/tmp/update.3dsx";
			}
			else
				continue;

		}

		if (m_download)
			m_download->setProgressMessage(_("Extracting %s", archive_entry_pathname(entry)));

		archive_entry_set_pathname(entry, path.c_str());
		r = archive_write_header(ext, entry);
		if (r < ARCHIVE_OK)
			fprintf(stderr, "%s\n", archive_error_string(ext));
		else if (archive_entry_size(entry) > 0)
		{
			r = copy_data(a, ext, m_app, m_download);

			if (m_download && m_download->isCanceled())
				break;

			if (r < ARCHIVE_OK)
				fprintf(stderr, "%s\n", archive_error_string(ext));
//						if (r < ARCHIVE_WARN)
//							exit(1);
		}
		r = archive_write_finish_entry(ext);
		if (r < ARCHIVE_OK)
			fprintf(stderr, "%s\n", archive_error_string(ext));
//					if (r < ARCHIVE_WARN)
//						exit(1);
		std::cout << path << std::endl;
	}

	archive_read_close(a);
	archive_read_free(a);
	archive_write_close(ext);
	archive_write_free(ext);

	writeFilelist();

	if (r == ARCHIVE_EOF)
		m_app->setInstalled(true);
	else
		uninstall();
}

// Verify filesize and checksum supplied by app config file
bool Installer::isValid()
{
	unsigned char buf[1024];
	FILE *file;
	size_t len;

	std::stringstream md5sum;
	unsigned char md5bytes[16];
	mbedtls_md5_context md5;

	bool ret = true;

	mbedtls_md5_init(&md5);
	mbedtls_md5_starts(&md5);

	file = fopen(cpp3ds::FileSystem::getFilePath(m_filename).c_str(), "rb");
	while(len = fread(buf, 1, sizeof(buf), file))
		mbedtls_md5_update(&md5, buf, len);

	mbedtls_md5_finish(&md5, md5bytes);

	md5sum << std::hex;
	for (int i = 0; i < sizeof(md5bytes); ++i)
		md5sum << std::setw(2) << std::setfill('0') << static_cast<int>(md5bytes[i]);

	if (ftell(file) != m_app->getFilesize())
		ret = false;
	if (md5sum.str() != m_app->getFileMD5())
		ret = false;

	fclose(file);
	mbedtls_md5_free(&md5);

	return ret;
}

void Installer::uninstall()
{
	readFileList();

	// TODO: add error checks and handling
	for (auto& filename : m_filelist)
	{
		std::string path = cpp3ds::FileSystem::getFilePath("sdmc:/" + filename);
		std::cout << filename << std::endl;
		struct stat sb;
		if (!stat(path.c_str(), &sb))
		{
			if (!S_ISDIR(sb.st_mode))
				unlink(path.c_str());
		}
	}

	std::string appDirectory = _("sdmc:/3ds/%s", m_app->getDirectory().c_str());

	if (removeDirectory(cpp3ds::FileSystem::getFilePath(appDirectory).c_str(), true))
	{
		std::cout << "files still exist" << std::endl;
	}

	unlink(m_filelistPath.c_str());
	m_app->setInstalled(false);
}

void Installer::writeFilelist()
{
	std::ofstream file(m_filelistPath);
	for (auto& filename : m_filelist)
	{
		if (filename != "3ds/")
			file << filename << std::endl;
	}
	file.close();
}

void Installer::readFileList()
{
	m_filelist.clear();
	std::ifstream file(m_filelistPath);
	std::string line;
	while(std::getline(file, line))
	{
		m_filelist.push_back(line);
	}
}

void Installer::attachDownload(Download *download)
{
	m_download = download;
}

} // namespace BrewMan
