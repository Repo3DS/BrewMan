#include "Button.hpp"


namespace gui3ds {

Button::Button()
: m_autoSize(true)
, m_needsUpdate(true)
, m_active(false)
, m_backgroundColor(cpp3ds::Color::White)
{

}


void Button::draw(cpp3ds::RenderTarget &target, cpp3ds::RenderStates states) const
{
	ensureUpdate();

	states.transform *= getTransform();

	NinePatch::draw(target, states);
	if (!m_text.getString().isEmpty())
		target.draw(m_text, states);
}


void Button::setTextOffset(const cpp3ds::Vector2f& offset)
{
	m_textOffset = offset;
	m_needsUpdate = true;
}


const cpp3ds::Vector2f& Button::getTextOffset() const
{
	return m_textOffset;
}


void Button::setContentSize(const cpp3ds::Vector2f& size)
{
	NinePatch::setContentSize(size);
	m_autoSize = false;
}


void Button::setContentSize(float width, float height)
{
	NinePatch::setContentSize(width, height);
	m_autoSize = false;
}


const cpp3ds::Vector2f& Button::getContentSize() const
{
	ensureUpdate();
	return NinePatch::getContentSize();
}


const cpp3ds::Vector2f& Button::getSize() const
{
	ensureUpdate();
	return m_size;
}


void Button::setString(const cpp3ds::String& string)
{
	m_text.setString(string);
	m_needsUpdate = true;
}


const cpp3ds::String& Button::getString() const
{
	return m_text.getString();
}


void Button::setTextColor(const cpp3ds::Color& color)
{
	m_textColor = color;
	m_needsUpdate = true;
}


const cpp3ds::Color& Button::getTextColor() const
{
	return m_textColor;
}


void Button::setTextActiveColor(const cpp3ds::Color& color)
{
	m_textActiveColor = color;
	m_needsUpdate = true;
}


const cpp3ds::Color& Button::getTextActiveColor() const
{
	return m_textActiveColor;
}


void Button::setColor(const cpp3ds::Color& color)
{
	m_backgroundColor = color;
	m_needsUpdate = true;
}


const cpp3ds::Color& Button::getColor() const
{
	return m_backgroundColor;
}


void Button::setActiveColor(const cpp3ds::Color& color)
{
	m_backgroundActiveColor = color;
	m_needsUpdate = true;
}


const cpp3ds::Color& Button::getActiveColor() const
{
	return m_backgroundActiveColor;
}


void Button::autoSize()
{
	m_autoSize = true;
	m_needsUpdate = true;
}


bool Button::processEvent(const cpp3ds::Event &event)
{
	if (event.type == cpp3ds::Event::TouchBegan)
	{
		if (m_rect.contains(event.touch.x, event.touch.y))
		{
			m_active = true;
			m_needsUpdate = true;
			return false;
		}
	}

	if (event.type == cpp3ds::Event::TouchEnded)
	{
		if (m_active) {
			m_active = false;
			m_needsUpdate = true;
			if (m_clickFunction)
				m_clickFunction();
			return false;
		}
	}

	if (event.type == cpp3ds::Event::TouchMoved)
	{
		if (m_active) {
			if (!m_rect.contains(event.touch.x, event.touch.y)) {
				m_active = false;
				m_needsUpdate = true;
			}
			return false;
		}
	}

	return true;
}


void Button::ensureUpdate() const
{
	if (m_needsUpdate)
	{
		if (m_autoSize)
			NinePatch::setContentSize(cpp3ds::Vector2f(m_text.getLocalBounds().width, m_text.getLocalBounds().height));

		cpp3ds::FloatRect textBounds = m_text.getLocalBounds();
		cpp3ds::FloatRect padding = getPadding();
		cpp3ds::Vector2f contentSize = NinePatch::getContentSize();

		if (m_autoSize && getTexture()) {
			m_size.x = contentSize.x + getTexture()->getSize().x - padding.width;
			m_size.y = contentSize.y + getTexture()->getSize().y - padding.height;
		} else {
			m_size.x = contentSize.x - padding.width;
			m_size.y = contentSize.y - padding.height;
		}

		m_text.setOrigin(std::round(textBounds.left + textBounds.width/2),
		                 std::round(textBounds.top + textBounds.height/2));
		m_text.setPosition(std::round(padding.left + contentSize.x/2 + m_textOffset.x),
		                   std::round(padding.top + contentSize.y/2 + m_textOffset.y));

		if (m_active) {
			NinePatch::setColor(m_backgroundActiveColor);
			m_text.setFillColor(m_textActiveColor);
		} else {
			NinePatch::setColor(m_backgroundColor);
			m_text.setFillColor(m_textColor);
		}

		m_needsUpdate = false;
	}

	m_rect.left = getPosition().x;
	m_rect.top = getPosition().y;
	m_rect.width = m_size.x;
	m_rect.height = m_size.y;
}


void Button::onClick(const std::function<void()>& callback)
{
	m_clickFunction = callback;
}


int Button::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
		case COLOR_RGB: {
			cpp3ds::Color color = getColor();
			returnValues[0] = color.r;
			returnValues[1] = color.g;
			returnValues[2] = color.b;
			return 3;
		}
		case COLOR_ALPHA: returnValues[0] = getColor().a; return 1;
		case CONTENT_X: returnValues[0] = getContentSize().x; return 1;
		case TEXTCOLOR_ALPHA: returnValues[0] = getTextColor().a; return 1;
		default:
			return TweenTransformable::getValues(tweenType, returnValues);
	}
}


void Button::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case COLOR_RGB: {
			cpp3ds::Color color;
			color.r = std::max(std::min(newValues[0], 255.f), 0.f);
			color.g = std::max(std::min(newValues[1], 255.f), 0.f);
			color.b = std::max(std::min(newValues[2], 255.f), 0.f);
			color.a = getColor().a;
			setColor(color);
			break;
		}
		case COLOR_ALPHA: {
			cpp3ds::Color color = getColor();
			color.a = std::max(std::min(newValues[0], 255.f), 0.f);
			setColor(color);
			break;
		}
		case CONTENT_X: setContentSize(cpp3ds::Vector2f(newValues[0], getContentSize().y)); break;
		case TEXTCOLOR_ALPHA: {
			cpp3ds::Color color = getTextColor();
			color.a = std::max(std::min(newValues[0], 255.f), 0.f);
			setTextColor(color);
			break;
		}
		default:
			TweenTransformable::setValues(tweenType, newValues);
			break;
	}
}


cpp3ds::Text &Button::getText() const
{
	return m_text;
}


} // namespace gui3ds
