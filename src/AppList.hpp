#ifndef BREWMAN_APPLIST_HPP
#define BREWMAN_APPLIST_HPP

#include <vector>
#include <string>
#include "AppItem.hpp"


namespace BrewMan
{

class AppList : public cpp3ds::Drawable, public util3ds::TweenTransformable<cpp3ds::Transformable> {
public:
	enum SortType {
		AlphaNumericAsc,
		AlphaNumericDesc,
	};

	AppList(std::string directory);
	~AppList();
	void refresh();
	void setSortType(SortType sortType);
	SortType getSortType() const;

	void setSelectedIndex(int index);
	int getSelectedIndex() const;

	size_t getCount() const;

	AppItem* getSelected();

protected:
	virtual void draw(cpp3ds::RenderTarget& target, cpp3ds::RenderStates states) const;
	void sort();
	void resize();

private:
	SortType m_sortType;
	std::string m_directory;
	int m_selectedIndex;
	std::vector<AppItem> m_list;
	std::vector<cpp3ds::Texture*> m_iconTextures;
};

} // namespace BrewMan

#endif // BREWMAN_APPLIST_HPP
