#ifndef BREWMAN_APPITEM_HPP
#define BREWMAN_APPITEM_HPP

#include <cpp3ds/Graphics/Drawable.hpp>
#include <cpp3ds/Graphics/Texture.hpp>
#include <cpp3ds/Graphics/Sprite.hpp>
#include <yaml-cpp/yaml.h>
#include "GUI/NinePatch.hpp"
#include "TweenObjects.hpp"

namespace BrewMan {

class AppItem : public cpp3ds::Drawable, public util3ds::TweenTransformable<cpp3ds::Transformable> {
public:
	AppItem();

	~AppItem();

	void loadFromYAML(const std::string &filename);

	void setTitle(const std::string &title);
	const std::string &getTitle() const;

	void setDirectory(const std::string &directory);
	const std::string &getDirectory() const;

	void setAuthor(const std::string &author);
	const std::string &getAuthor() const;

	void setVersion(const std::string &version);
	const std::string &getVersion() const;

	void setDescription(const std::string &description);
	const std::string &getDescription() const;

	void setLongDescription(const std::string &description);
	const std::string &getLongDescription() const;

	void setFilename(const std::string &filename);
	const std::string &getFilename() const;

	void setFileMD5(const std::string &md5);
	const std::string &getFileMD5() const;

	void setFilesize(size_t filesize);
	size_t getFilesize() const;

	void setIcon(const cpp3ds::Texture *texture);

	void setSize(float width, float height);

	const cpp3ds::Vector2f &getSize() const;
	const cpp3ds::Texture* getIcon() const;

	void setScreenshots(const std::vector<std::string> &screenshots);
	const std::vector<std::string>& getScreenshots() const;

	void setInstalled(bool installed);
	bool isInstalled() const;

	void select();

	void deselect();

protected:
	virtual void draw(cpp3ds::RenderTarget &target, cpp3ds::RenderStates states) const;

private:
	gui3ds::NinePatch m_background;

	cpp3ds::RectangleShape m_icon;
	cpp3ds::RectangleShape m_progressBar;

	cpp3ds::Text m_titleText;
	cpp3ds::Text m_shortDescriptionText;
	cpp3ds::Text m_authorText;
	cpp3ds::Text m_filesizeText;

	cpp3ds::Vector2f m_size;

	std::string m_title;
	std::string m_directory;
	std::string m_author;
	std::string m_version;
	std::string m_description;
	std::string m_longDescription;
	std::string m_filename;
	std::string m_fileMD5;
	size_t m_filesize;
	std::vector<std::string> m_screenshots;

	bool m_installed;
};

} // namepace BrewMan


#endif // BREWMAN_APPITEM_HPP
