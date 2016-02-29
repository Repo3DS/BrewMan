#ifndef GUI3DS_TAB_HPP
#define GUI3DS_TAB_HPP

#include <cpp3ds/Graphics/Drawable.hpp>
#include <cpp3ds/Graphics/Text.hpp>
#include <cpp3ds/Graphics/RectangleShape.hpp>

namespace gui3ds {

class Tab : public cpp3ds::Drawable, public cpp3ds::Transformable {
public:
	Tab(const std::string& caption);

	void setCaption(const std::string& caption);
	const std::string& getCaption() const;

	void setPadding(const cpp3ds::Vector2f& padding);
	const cpp3ds::Vector2f& getPadding() const;

	void setHeight(float height);
	float getHeight() const;

	cpp3ds::FloatRect getBounds() const;

	void setFillColor(const cpp3ds::Color& color);
	void setOutlineColor(const cpp3ds::Color& color);
	void setTextFillColor(const cpp3ds::Color& color);
	void setTextOutlineColor(const cpp3ds::Color& color);
	void setTextCharacterSize(unsigned size);

	void setOutlineThickness(float thickness);
	void setTextOutlineThickness(float thickness);

	void setFont(const cpp3ds::Font& font);

protected:
	void resize();
	virtual void draw(cpp3ds::RenderTarget& target, cpp3ds::RenderStates states) const;

private:
	cpp3ds::RectangleShape m_background;
//	cpp3ds::RectangleShape m_backgroundOutlined;
	cpp3ds::Text m_caption;
//	cpp3ds::Text m_captionActive;

	float m_height;
	cpp3ds::Vector2f m_padding;
};

} // namespace gui3ds

#endif // GUI3DS_TAB_HPP
