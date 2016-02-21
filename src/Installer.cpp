#include <archive.h>
#include <iostream>
#include <cpp3ds/System/FileSystem.hpp>
#include <archive_entry.h>
#include <mbedtls/md5.h>
#include "Installer.hpp"


namespace {

static int copy_data(struct archive *ar, struct archive *aw)
{
	int r;
	const void *buff;
	size_t size;
	int64_t offset;

	for (;;) {
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r < ARCHIVE_OK)
			return (r);
		r = archive_write_data_block(aw, buff, size, offset);
		if (r < ARCHIVE_OK) {
			fprintf(stderr, "%s\n", archive_error_string(aw));
			return (r);
		}
	}
}

static int path_exists(const char *name)
{
	struct stat buffer;
	return stat(name, &buffer) == 0;
}

} // namespace


namespace BrewMan {

Installer::Installer(AppItem *app, const std::string& filename)
: m_thread(&Installer::run, this)
, m_app(app)
, m_filename(filename)
{

}

void Installer::run()
{
	if (!isValid())
		return;

	struct archive *a;
	struct archive *ext;
	struct archive_entry *entry;
	int flags;
	int r;

	/* Select which attributes we want to restore. */
	flags = ARCHIVE_EXTRACT_TIME;
	flags |= ARCHIVE_EXTRACT_PERM;
	flags |= ARCHIVE_EXTRACT_ACL;
	flags |= ARCHIVE_EXTRACT_FFLAGS;

	a = archive_read_new();
	archive_read_support_format_all(a);
//	archive_read_support_compression_all(a);
	archive_read_support_compression_compress(a);
	archive_read_support_compression_bzip2(a);
	archive_read_support_compression_xz(a);
	ext = archive_write_disk_new();
	archive_write_disk_set_options(ext, flags);
//	archive_write_disk_set_standard_lookup(ext);
	if ((r = archive_read_open_filename(a, cpp3ds::FileSystem::getFilePath(m_filename).c_str(), 10240))) {
		std::cout << "failure! " << r << std::endl;
		return;
	}
	for (;;) {
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r < ARCHIVE_OK)
			fprintf(stderr, "%s\n", archive_error_string(a));
//					if (r < ARCHIVE_WARN)
//						exit(1);
		std::string path = cpp3ds::FileSystem::getFilePath(std::string("sdmc:/") + archive_entry_pathname(entry));
		if (path_exists(path.c_str()))
		{
			std::cout << "file already exists: " << path << std::endl;
			continue;
		}
		archive_entry_set_pathname(entry, path.c_str());
		r = archive_write_header(ext, entry);
		if (r < ARCHIVE_OK)
			fprintf(stderr, "%s\n", archive_error_string(ext));
		else if (archive_entry_size(entry) > 0) {
			r = copy_data(a, ext);
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
		md5sum << static_cast<int>(md5bytes[i]);

	if (ftell(file) != m_app->getFilesize())
		ret = false;
	if (md5sum.str() != m_app->getFileMD5())
		ret = false;

	fclose(file);
	mbedtls_md5_free(&md5);

	return ret;
}

} // namespace BrewMan
