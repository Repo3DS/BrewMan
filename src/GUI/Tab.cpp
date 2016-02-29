#include <cpp3ds/Graphics/RenderTarget.hpp>
#include "Tab.hpp"

namespace gui3ds {

Tab::Tab(const std::string& caption)
: m_height(20.f)
{
	setCaption(caption);
}

void Tab::draw(cpp3ds::RenderTarget &target, cpp3ds::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_background, states);
	target.draw(m_caption, states);
}

void Tab::setPadding(const cpp3ds::Vector2f& padding)
{
	m_padding = padding;
	resize();
}

const cpp3ds::Vector2f& Tab::getPadding() const
{
	return m_padding;
}

cpp3ds::FloatRect Tab::getBounds() const
{
	cpp3ds::FloatRect bounds = m_background.getGlobalBounds();
	bounds.left = getPosition().x;
	bounds.top = getPosition().y;
	return bounds;
}

void Tab::setCaption(const std::string &caption)
{
	m_caption.setString(caption);
	resize();
}

const std::string &Tab::getCaption() const
{
	return m_caption.getString().toAnsiString();
}

void Tab::setFillColor(const cpp3ds::Color &color)
{
	m_background.setFillColor(color);
}

void Tab::setOutlineColor(const cpp3ds::Color &color)
{
	m_background.setOutlineColor(color);
}

void Tab::setTextFillColor(const cpp3ds::Color &color)
{
	m_caption.setFillColor(color);
}

void Tab::setTextOutlineColor(const cpp3ds::Color &color)
{
	m_caption.setOutlineColor(color);
}

void Tab::setTextCharacterSize(unsigned size)
{
	m_caption.setCharacterSize(size);
	resize();
}

void Tab::setOutlineThickness(float thickness)
{
	m_background.setOutlineThickness(thickness);
}

void Tab::setTextOutlineThickness(float thickness)
{
	m_caption.setOutlineThickness(thickness);
}

void Tab::setFont(const cpp3ds::Font &font)
{
	m_caption.setFont(font);
	resize();
}

void Tab::resize()
{
	cpp3ds::FloatRect bounds = m_caption.getLocalBounds();
	m_background.setSize(cpp3ds::Vector2f(bounds.width + m_padding.x * 2, m_height));
	m_caption.setPosition(m_padding.x - bounds.left, m_padding.y - bounds.top);
}

void Tab::setHeight(float height)
{
	m_height = height;
	resize();
}

float Tab::getHeight() const
{
	return m_height;
}

} // namespace gui3ds
