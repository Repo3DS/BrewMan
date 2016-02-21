#include <cpp3ds/System/Err.hpp>
#include <iostream>
#include <cpp3ds/System/I18n.hpp>
#include "AssetManager.hpp"
#include "AppItem.hpp"

namespace {

	cpp3ds::Texture& getMissingIconTexture()
	{
		static cpp3ds::Texture texture;
		if (texture.getSize().x == 0)
			texture.loadFromFile("images/missing-icon.png");
		return texture;
	}

}


namespace BrewMan
{

// Static members
cpp3ds::Texture AppItem::m_backgroundTexture;
cpp3ds::Texture AppItem::m_backgroundSelectedTexture;

AppItem::AppItem()
: m_progress(0.f)
, m_installed(false)
{
	if (m_backgroundTexture.getSize().x == 0)
		m_backgroundTexture.loadFromFile("images/itembg.9.png");
	if (m_backgroundSelectedTexture.getSize().x == 0)
		m_backgroundSelectedTexture.loadFromFile("images/itembg-selected.9.png");

	deselect();

	m_icon.setSize(cpp3ds::Vector2f(48.f, 48.f));
	m_icon.setTexture(&getMissingIconTexture(), true);
	m_icon.setPosition(4.f, 4.f);
	m_icon.setOutlineThickness(1.f);
	m_icon.setFillColor(cpp3ds::Color(180,180,180));
	m_icon.setOutlineColor(cpp3ds::Color(0, 0, 0, 50));

	m_progressBar.setFillColor(cpp3ds::Color(0, 0, 0, 50));

	m_titleText.setCharacterSize(12);
	m_titleText.setPosition(57.f, 3.f);
	m_titleText.setFillColor(cpp3ds::Color(50,50,50));

	m_shortDescriptionText.setCharacterSize(10);
	m_shortDescriptionText.setPosition(56.f, 20.f);
	m_shortDescriptionText.setFillColor(cpp3ds::Color(100,100,100));

	m_filesizeText.setCharacterSize(10);
	m_filesizeText.setPosition(56.f, 37.f);
	m_filesizeText.setFillColor(cpp3ds::Color(150,150,150));

	m_authorText.setCharacterSize(10);
	m_authorText.setPosition(56.f, 37.f);
	m_authorText.setFillColor(cpp3ds::Color(120,120,120));

	setSize(194.f, 56.f);
}

AppItem::~AppItem()
{

}

void AppItem::draw(cpp3ds::RenderTarget &target, cpp3ds::RenderStates states) const
{
	states.transform *= getTransform();

	target.draw(m_background, states);
	target.draw(m_icon, states);
	target.draw(m_shortDescriptionText, states);
	target.draw(m_titleText, states);
	target.draw(m_filesizeText, states);
	target.draw(m_authorText, states);
	if (m_progress > 0.f)
		target.draw(m_progressBar, states);
}

void AppItem::setSize(float width, float height)
{
	m_size.x = width;
	m_size.y = height;

//	m_background.setPadding(3.f, 3.f, 3.f, 3.f);
	m_background.setContentSize(m_size.x + m_background.getPadding().width - m_background.getTexture()->getSize().x + 2.f,
								m_size.y + m_background.getPadding().height - m_background.getTexture()->getSize().y + 2.f);
	setProgress(m_progress); // Resize progress bar
}

const cpp3ds::Vector2f& AppItem::getSize() const
{
	return m_size;
}

void AppItem::setTitle(const std::string &title)
{
	m_title = title;
	m_titleText.setString(title);
}

const std::string &AppItem::getTitle() const
{
	return m_title;
}

void AppItem::setDirectory(const std::string &directory)
{
	m_directory = directory;
}

const std::string &AppItem::getDirectory() const
{
	return m_directory;
}

void AppItem::setAuthor(const std::string &author)
{
	m_author = author;
	m_authorText.setString(author);
}

const std::string &AppItem::getAuthor() const
{
	return m_author;
}

void AppItem::setVersion(const std::string &version)
{
	m_version = version;
}

const std::string &AppItem::getVersion() const
{
	return m_version;
}

void AppItem::setDescription(const std::string &description)
{
	m_description = description;
	m_shortDescriptionText.setString(description);

	// If text overflows, trim it and add ellipsis
	if (m_shortDescriptionText.getLocalBounds().width > 135)
	{
		// Create a temp copy to test sizes
		cpp3ds::Text text(m_shortDescriptionText);
		for (int i = description.size(); i > 0; --i)
		{
			text.setString(description.substr(0, i));
			if (text.getLocalBounds().width < 128)
			{
				m_shortDescriptionText.setString(description.substr(0, i) + "...");
				break;
			}
		}
	}
}

const std::string &AppItem::getDescription() const
{
	return m_description;
}

void AppItem::setLongDescription(const std::string &description)
{
	m_longDescription = description;
}

const std::string &AppItem::getLongDescription() const
{
	return m_longDescription.empty() ? m_description : m_longDescription;
}

void AppItem::setFilename(const std::string &filename)
{
	m_filename = filename;
}

const std::string &AppItem::getFilename() const
{
	return m_filename;
}

void AppItem::setFileMD5(const std::string &md5)
{
	m_fileMD5 = md5;
}

const std::string &AppItem::getFileMD5() const
{
	return m_fileMD5;
}

void AppItem::setIcon(const cpp3ds::Texture* texture)
{
	m_icon.setTexture(texture, true);
	if (texture)
		m_icon.setFillColor(cpp3ds::Color::White);
}

void AppItem::loadFromYAML(const std::string &filename)
{
	try
	{
		YAML::Node m_yaml;
		m_yaml = YAML::LoadFile(filename);

		setTitle(m_yaml["title"].as<std::string>());
		setAuthor(m_yaml["author"].as<std::string>());
		setVersion(m_yaml["version"].as<std::string>());
		setDescription(m_yaml["description"].as<std::string>());
		setFilename(m_yaml["file"].as<std::string>());
		setFileMD5(m_yaml["file-md5"].as<std::string>());
		setFilesize(m_yaml["filesize"].as<size_t>());
		if (m_yaml["long-description"])
			setLongDescription(m_yaml["long-description"].as<std::string>());
		if (m_yaml["screenshots"])
			setScreenshots(m_yaml["screenshots"].as<std::vector<std::string>>());
	}
	catch(YAML::Exception& e)
	{
		cpp3ds::err() << e.what() << std::endl;
	}
}

void AppItem::select()
{
	m_background.setTexture(&m_backgroundSelectedTexture);
	m_background.setColor(cpp3ds::Color(255, 255, 255, 200));
	m_icon.setOutlineThickness(2.f);
}

void AppItem::deselect()
{
	m_background.setTexture(&m_backgroundTexture);
	m_background.setColor(cpp3ds::Color(255, 255, 255, 80));
	m_icon.setOutlineThickness(1.f);
}

void AppItem::setProgress(float progress)
{
	m_progress = progress;
	m_progressBar.setSize(cpp3ds::Vector2f(m_progress * m_size.x, m_size.y));
}

float AppItem::getProgress() const
{
	return m_progress;
}

void AppItem::setInstalled(bool installed)
{
	m_installed = installed;
}

bool AppItem::isInstalled() const
{
	return m_installed;
}

void AppItem::setFilesize(size_t filesize)
{
	m_filesize = filesize;

	if (filesize > 1024 * 1024)
		m_filesizeText.setString(_("%.1f MB", static_cast<float>(filesize) / 1024.f / 1024.f));
	else
		m_filesizeText.setString(_("%d KB", filesize / 1024));

	m_filesizeText.setPosition(189.f - m_filesizeText.getLocalBounds().width, 37.f);
}

size_t AppItem::getFilesize() const
{
	return m_filesize;
}

const cpp3ds::Texture *AppItem::getIcon() const
{
	return m_icon.getTexture();
}

void AppItem::setScreenshots(const std::vector<std::string> &screenshots)
{
	m_screenshots = screenshots;
}

const std::vector<std::string> &AppItem::getScreenshots() const
{
	return m_screenshots;
}

} // namespace BrewMan
