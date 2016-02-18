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

	void setFilename(const std::string &filename);
	const std::string &getFilename() const;

	void setFilesize(size_t filesize);
	size_t getFilesize() const;

	void setIcon(const cpp3ds::Texture *texture);

	void setSize(float width, float height);

	const cpp3ds::Vector2f &getSize() const;
	const cpp3ds::Texture* getIcon() const;

	void setProgress(float progress);
	float getProgress() const;

	void setInstalled(bool installed);
	bool isInstalled() const;

	void select();

	void deselect();

protected:
	virtual void draw(cpp3ds::RenderTarget &target, cpp3ds::RenderStates states) const;

private:
	static cpp3ds::Texture m_backgroundTexture;
	static cpp3ds::Texture m_backgroundSelectedTexture;
	gui3ds::NinePatch m_background;

	cpp3ds::RectangleShape m_icon;
	cpp3ds::RectangleShape m_progressBar;

	cpp3ds::Text m_titleText;
	cpp3ds::Text m_shortDescriptionText;
	cpp3ds::Text m_authorText;
	cpp3ds::Text m_filesizeText;

	cpp3ds::Vector2f m_size;
	float m_progress;

	std::string m_title;
	std::string m_directory;
	std::string m_author;
	std::string m_version;
	std::string m_description;
	std::string m_filename;
	size_t m_filesize;

	bool m_installed;
};

} // namepace BrewMan


#endif // BREWMAN_APPITEM_HPP
