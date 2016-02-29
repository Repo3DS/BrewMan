#include <cpp3ds/System/FileSystem.hpp>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <bits/stl_algo.h>
#include <sys/stat.h>
#include <functional>
#include <iostream>
#include "AppList.hpp"
#include "Util.hpp"


namespace BrewMan {

AppList::AppList(std::string directory)
: m_sortType(AlphaNumericAsc)
, m_selectedIndex(0)
{
	m_directory = directory;
	refresh();
}

AppList::~AppList()
{
	for (auto& texture : m_iconTextures)
		delete texture;
}

void AppList::refresh()
{
	char configNameBuf[256];
	char iconNameBuf[256];
	DIR *dir = opendir(cpp3ds::FileSystem::getFilePath(m_directory).c_str());
	struct dirent *entry;

	m_list.clear();

	while (entry = readdir(dir))
	{
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;

		if (entry->d_type == DT_DIR)
		{
			snprintf(configNameBuf, sizeof(configNameBuf), "%s%s/config.yml", m_directory.c_str(), entry->d_name);
			snprintf(iconNameBuf, sizeof(iconNameBuf), "%s%s/icon.png", m_directory.c_str(), entry->d_name);

			if (pathExists(configNameBuf))
			{
				AppItem item;

				if (pathExists(iconNameBuf))
				{
					cpp3ds::Texture* icon = new cpp3ds::Texture();
					if (icon->loadFromFile(iconNameBuf))
					{
						m_iconTextures.push_back(icon);
						item.setIcon(icon);
					}
					else
						delete icon;
				}

				item.loadFromYAML(cpp3ds::FileSystem::getFilePath(configNameBuf));
				item.setDirectory(entry->d_name);

				snprintf(configNameBuf, sizeof(configNameBuf), "sdmc:/3ds/BrewMan/installed/%s.list", entry->d_name);
				item.setInstalled(pathExists(configNameBuf));

				m_list.push_back(item);
			}
		}

	}

	closedir(dir);

	sort();
	resize();

	setSelectedIndex(m_selectedIndex);
}

void AppList::setSortType(AppList::SortType sortType)
{
	m_sortType = sortType;
	sort();
}

AppList::SortType AppList::getSortType() const
{
	return m_sortType;
}

void AppList::sort()
{
	std::sort(m_list.begin(), m_list.end(), [&](const AppItem& a, const AppItem& b)
	{
		switch(m_sortType)
		{
			case AlphaNumericDesc:
				return a.getTitle() > b.getTitle();
			default:
				return a.getTitle() < b.getTitle();
		}
	});
}

void AppList::resize()
{
	float posY = 4.f;
	int i = 0;

	for (auto& app : m_list)
	{
		app.setPosition(3.f + (i/4) * 200.f, posY);

		if (++i % 4 == 0)
			posY = 4.f;
		else
			posY += 59.f;
	}
}

void AppList::setSelectedIndex(int index)
{
	m_list[m_selectedIndex].deselect();
	m_selectedIndex = index;
	m_list[m_selectedIndex].select();
}

int AppList::getSelectedIndex() const
{
	return m_selectedIndex;
}

AppItem *AppList::getSelected()
{
	return &m_list[m_selectedIndex];
}

void AppList::draw(cpp3ds::RenderTarget &target, cpp3ds::RenderStates states) const
{
	states.transform *= getTransform();

	for (auto& app : m_list)
	{
		target.draw(app, states);
	}
}

size_t AppList::getCount() const
{
	return m_list.size();
}

} // namespace BrewMan