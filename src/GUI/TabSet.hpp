#ifndef GUI3DS_TABSET_HPP
#define GUI3DS_TABSET_HPP

#include <cpp3ds/Graphics/RenderTarget.hpp>
#include <cpp3ds/Graphics/RectangleShape.hpp>
#include <cpp3ds/Graphics/Drawable.hpp>
#include <cpp3ds/Window/Event.hpp>
#include "Tab.hpp"

namespace gui3ds {

class TabSet : public cpp3ds::Drawable, public cpp3ds::Transformable {
public:
	TabSet(const cpp3ds::Vector2f& size);

	void addTab(const std::string& caption);

	Tab& getTab(size_t index);
	size_t getTabCount() const;

	void setSize(const cpp3ds::Vector2f& size);
	const cpp3ds::Vector2f& getSize() const;

	void setPadding(const cpp3ds::Vector2f& padding);
	const cpp3ds::Vector2f& getPadding() const;

	void setTabSpacing(float spacing);
	float getTabSpacing() const;
	void setTabHeight(float height);
	float getTabHeight() const;


	void setTabCharacterSize(unsigned size);
	unsigned getTabCharacterSize() const;

	void setBackgroundColor(const cpp3ds::Color& color);
	const cpp3ds::Color& getBackgroundColor() const;

	void setTabBackgroundColor(const cpp3ds::Color& color);
	const cpp3ds::Color& getTabBackgroundColor() const;
	void setTabBackgroundActiveColor(const cpp3ds::Color& color);
	const cpp3ds::Color& getTabBackgroundActiveColor() const;
	void setTabBackgroundOutlineColor(const cpp3ds::Color& color);
	const cpp3ds::Color& getTabBackgroundOutlineColor() const;
	void setTabBackgroundOutlineActiveColor(const cpp3ds::Color& color);
	const cpp3ds::Color& getTabBackgroundOutlineActiveColor() const;

	void setTabTextColor(const cpp3ds::Color& color);
	const cpp3ds::Color& getTabTextColor() const;
	void setTabTextActiveColor(const cpp3ds::Color& color);
	const cpp3ds::Color& getTabTextActiveColor() const;
	void setTabTextOutlineColor(const cpp3ds::Color& color);
	const cpp3ds::Color& getTabTextOutlineColor() const;
	void setTabTextOutlineActiveColor(const cpp3ds::Color& color);
	const cpp3ds::Color& getTabTextOutlineActiveColor() const;

	void setTabBackgroundOutlineThickness(float thickness);
	float getTabBackgroundOutlineThickness() const;
	void setTabBackgroundOutlineActiveThickness(float thickness);
	float getTabBackgroundOutlineActiveThickness() const;

	void setTabTextOutlineThickness(float thickness);
	float getTabTextOutlineThickness() const;
	void setTabTextOutlineActiveThickness(float thickness);
	float getTabTextOutlineActiveThickness() const;

	void processEvent(const cpp3ds::Event &event);
	const Tab& getSelected() const;

	void setSelectedIndex(int index);
	int getSelectedIndex() const;

protected:
	virtual void draw(cpp3ds::RenderTarget& target, cpp3ds::RenderStates states) const;

private:
	mutable bool m_needsUpdate;
	mutable std::vector<Tab> m_tabs;
	int m_selectedIndex;

	cpp3ds::RectangleShape m_background;
	cpp3ds::RectangleShape m_backgroundOutline;
	cpp3ds::View m_view;

	cpp3ds::Vector2f m_size;
	cpp3ds::Vector2f m_padding;
	float m_tabSpacing;
	float m_tabHeight;
	mutable float m_tabPosX;
	unsigned m_tabCharacterSize;
	float m_tabBackgroundOutlineThickness;
	float m_tabBackgroundOutlineActiveThickness;
	float m_tabTextOutlineThickness;
	float m_tabTextOutlineActiveThickness;
	cpp3ds::Color m_tabBackgroundColor;
	cpp3ds::Color m_tabBackgroundActiveColor;
	cpp3ds::Color m_tabBackgroundOutlineColor;
	cpp3ds::Color m_tabBackgroundOutlineActiveColor;
	cpp3ds::Color m_tabTextColor;
	cpp3ds::Color m_tabTextActiveColor;
	cpp3ds::Color m_tabTextOutlineColor;
	cpp3ds::Color m_tabTextOutlineActiveColor;
};

} // namespace gui3ds

#endif // GUI3DS_TABSET_HPP
