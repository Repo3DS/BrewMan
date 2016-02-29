#include "TabSet.hpp"

namespace gui3ds {

TabSet::TabSet(const cpp3ds::Vector2f& size)
: m_selectedIndex(-1)
, m_needsUpdate(true)
, m_padding(4.f, 2.f)
, m_tabSpacing(2.f)
, m_tabCharacterSize(15)
, m_tabHeight(std::round(size.y / 1.5f))
, m_tabBackgroundOutlineThickness(1.f)
, m_tabTextOutlineThickness(0.f)
, m_tabTextOutlineActiveThickness(0.f)
, m_tabBackgroundColor(cpp3ds::Color::Transparent)
, m_tabBackgroundActiveColor(cpp3ds::Color(0, 0, 0, 100))
, m_tabBackgroundOutlineColor(cpp3ds::Color::Black)
, m_tabTextColor(cpp3ds::Color(150, 150, 150, 255))
, m_tabTextActiveColor(cpp3ds::Color::Black)
, m_tabTextOutlineColor(cpp3ds::Color::Black)
, m_tabTextOutlineActiveColor(cpp3ds::Color::Black)
{
	setSize(size);
	setTabBackgroundOutlineActiveThickness(1.f);
	setTabBackgroundOutlineActiveColor(cpp3ds::Color::Black);
	m_background.setFillColor(cpp3ds::Color::Transparent);
	m_backgroundOutline.setFillColor(cpp3ds::Color::Transparent);
}

void TabSet::draw(cpp3ds::RenderTarget &target, cpp3ds::RenderStates states) const
{
	cpp3ds::View oldView = target.getView();
	target.setView(m_view);

	target.draw(m_background, states);

	// Calculate posX at starting X position, to center tabs
	if (m_needsUpdate)
	{
		m_tabPosX = 0;
		for (auto& tab : m_tabs)
		{
			tab.setTextCharacterSize(m_tabCharacterSize);
			tab.setPadding(m_padding);
			tab.setHeight(m_tabHeight);
			m_tabPosX += tab.getBounds().width;
		}
		m_tabPosX += (m_tabs.size() - 1) * m_tabSpacing;
		m_tabPosX = (m_size.x - m_tabPosX) / 2.f;
	}

	float posX = m_tabPosX;
	for (int i = 0; i < m_tabs.size(); ++i)
	{
		Tab& tab = m_tabs[i];
		if (m_needsUpdate)
		{
			if (i == m_selectedIndex)
			{
				tab.setOutlineThickness(m_tabBackgroundOutlineActiveThickness);
				tab.setTextOutlineThickness(m_tabTextOutlineActiveThickness);
				tab.setFillColor(m_tabBackgroundActiveColor);
				tab.setOutlineColor(m_tabBackgroundOutlineActiveColor);
				tab.setTextFillColor(m_tabTextActiveColor);
				tab.setTextOutlineColor(m_tabTextOutlineActiveColor);
			}
			else
			{
				tab.setOutlineThickness(m_tabBackgroundOutlineThickness);
				tab.setTextOutlineThickness(m_tabTextOutlineThickness);
				tab.setFillColor(m_tabBackgroundColor);
				tab.setOutlineColor(m_tabBackgroundOutlineColor);
				tab.setTextFillColor(m_tabTextColor);
				tab.setTextOutlineColor(m_tabTextOutlineColor);
			}

			cpp3ds::FloatRect bounds = tab.getBounds();
			tab.setPosition(std::round(posX), m_size.y - m_tabHeight);
			posX += bounds.width + m_tabSpacing;
		}

		// Draw all except active tab
		if (i != m_selectedIndex)
			target.draw(tab, states);
	}

	target.draw(m_backgroundOutline, states);

	// Draw active tab after the background border of the tabset is drawn
	target.draw(m_tabs[m_selectedIndex], states);

	target.setView(oldView);

	m_needsUpdate = false;
}

void TabSet::processEvent(const cpp3ds::Event &event)
{
	if (event.type == cpp3ds::Event::TouchBegan)
	{
		cpp3ds::Vector2f pos = getPosition();
		for (int i = 0; i < m_tabs.size(); ++i)
		{
			if (m_tabs[i].getBounds().contains(pos.x + event.touch.x, pos.y + event.touch.y))
			{
				setSelectedIndex(i);
				break;
			}
		}
	}
}

void TabSet::setSelectedIndex(int index)
{
	m_selectedIndex = index;
	m_needsUpdate = true;
}

int TabSet::getSelectedIndex() const
{
	return m_selectedIndex;
}

const Tab &TabSet::getSelected() const
{
	return m_tabs[m_selectedIndex];
}

void TabSet::addTab(const std::string &caption)
{
	m_needsUpdate = true;
	m_tabs.emplace_back(caption);
}

size_t TabSet::getTabCount() const
{
	return m_tabs.size();
}

void TabSet::setSize(const cpp3ds::Vector2f &size)
{
	m_size = size;
	m_needsUpdate = true;

	m_background.setSize(size);
	m_backgroundOutline.setSize(size);
	m_view.reset(cpp3ds::FloatRect(getPosition().x, getPosition().y, m_size.x, m_size.y));
	m_view.setViewport(cpp3ds::FloatRect(getPosition().x / 320.f, getPosition().y / 240.f, m_size.x / 320.f, m_size.y / 240.f));
}

const cpp3ds::Vector2f &TabSet::getSize() const
{
	return m_size;
}

void TabSet::setPadding(const cpp3ds::Vector2f &padding)
{
	m_padding = padding;
	m_needsUpdate = true;
}

const cpp3ds::Vector2f &TabSet::getPadding() const
{
	return m_padding;
}

void TabSet::setBackgroundColor(const cpp3ds::Color &color)
{
	m_background.setFillColor(color);
	m_needsUpdate = true;
}

const cpp3ds::Color &TabSet::getBackgroundColor() const
{
	return m_background.getFillColor();
}

void TabSet::setTabBackgroundColor(const cpp3ds::Color &color)
{
	m_tabBackgroundColor = color;
	m_needsUpdate = true;
}

const cpp3ds::Color &TabSet::getTabBackgroundColor() const
{
	return m_tabBackgroundColor;
}

void TabSet::setTabBackgroundActiveColor(const cpp3ds::Color &color)
{
	m_tabBackgroundActiveColor = color;
	m_needsUpdate = true;
}

const cpp3ds::Color &TabSet::getTabBackgroundActiveColor() const
{
	return m_tabBackgroundActiveColor;
}

void TabSet::setTabBackgroundOutlineColor(const cpp3ds::Color &color)
{
	m_tabBackgroundOutlineColor = color;
	m_needsUpdate = true;
}

const cpp3ds::Color &TabSet::getTabBackgroundOutlineColor() const
{
	return m_tabBackgroundOutlineColor;
}

void TabSet::setTabBackgroundOutlineActiveColor(const cpp3ds::Color &color)
{
	m_tabBackgroundOutlineActiveColor = color;
	m_needsUpdate = true;

	m_backgroundOutline.setOutlineColor(color);
}

const cpp3ds::Color &TabSet::getTabBackgroundOutlineActiveColor() const
{
	return m_tabBackgroundOutlineActiveColor;
}

void TabSet::setTabTextColor(const cpp3ds::Color &color)
{
	m_tabTextColor = color;
	m_needsUpdate = true;
}

const cpp3ds::Color &TabSet::getTabTextColor() const
{
	return m_tabTextColor;
}

void TabSet::setTabTextActiveColor(const cpp3ds::Color &color)
{
	m_tabTextActiveColor = color;
	m_needsUpdate = true;
}

const cpp3ds::Color &TabSet::getTabTextActiveColor() const
{
	return m_tabTextActiveColor;
}

void TabSet::setTabTextOutlineColor(const cpp3ds::Color &color)
{
	m_tabTextOutlineColor = color;
	m_needsUpdate = true;
}

const cpp3ds::Color &TabSet::getTabTextOutlineColor() const
{
	return m_tabTextOutlineColor;
}

void TabSet::setTabTextOutlineActiveColor(const cpp3ds::Color &color)
{
	m_tabTextOutlineActiveColor = color;
	m_needsUpdate = true;
}

const cpp3ds::Color &TabSet::getTabTextOutlineActiveColor() const
{
	return m_tabTextOutlineActiveColor;
}

void TabSet::setTabBackgroundOutlineThickness(float thickness)
{
	m_tabBackgroundOutlineThickness = thickness;
	m_needsUpdate = true;
}

float TabSet::getTabBackgroundOutlineThickness() const
{
	return m_tabBackgroundOutlineThickness;
}

void TabSet::setTabBackgroundOutlineActiveThickness(float thickness)
{
	m_tabBackgroundOutlineActiveThickness = thickness;
	m_needsUpdate = true;

	m_backgroundOutline.setOutlineThickness(thickness);
	m_backgroundOutline.setPosition(getPosition() - cpp3ds::Vector2f(0.f, thickness));
}

float TabSet::getTabBackgroundOutlineActiveThickness() const
{
	return m_tabBackgroundOutlineActiveThickness;
}

void TabSet::setTabTextOutlineThickness(float thickness)
{
	m_tabTextOutlineThickness = thickness;
	m_needsUpdate = true;
}

float TabSet::getTabTextOutlineThickness() const
{
	return m_tabTextOutlineThickness;
}

void TabSet::setTabTextOutlineActiveThickness(float thickness)
{
	m_tabTextOutlineActiveThickness = thickness;
	m_needsUpdate = true;
}

float TabSet::getTabTextOutlineActiveThickness() const
{
	return m_tabTextOutlineActiveThickness;
}

void TabSet::setTabSpacing(float spacing)
{
	m_tabSpacing = spacing;
	m_needsUpdate = true;
}

float TabSet::getTabSpacing() const
{
	return m_tabSpacing;
}

void TabSet::setTabHeight(float height)
{
	m_tabHeight = height;
	m_needsUpdate = true;
}

float TabSet::getTabHeight() const
{
	return m_tabHeight;
}

void TabSet::setTabCharacterSize(unsigned size)
{
	m_tabCharacterSize = size;
	m_needsUpdate = true;
}

unsigned TabSet::getTabCharacterSize() const
{
	return m_tabCharacterSize;
}

Tab &TabSet::getTab(size_t index)
{
	// Assume the tab is modified when returned
	m_needsUpdate = true;
	return m_tabs[index];
}

} // namespace gui3ds
