#ifndef GUI3DS_NINEPATCH_HPP
#define GUI3DS_NINEPATCH_HPP

#include <cpp3ds/Graphics/Drawable.hpp>
#include <cpp3ds/Graphics/Texture.hpp>
#include <cpp3ds/Graphics/RenderTarget.hpp>
#include <cpp3ds/Graphics/VertexArray.hpp>
#include <cpp3ds/Graphics/RectangleShape.hpp>

namespace gui3ds {

class NinePatch : public cpp3ds::Drawable {
public:
	NinePatch();
	~NinePatch();

	void setTexture(const cpp3ds::Texture *texture);
	const cpp3ds::Texture* getTexture() const;

	void setContentSize(const cpp3ds::Vector2f& size) const;
	void setContentSize(float width, float height) const;
	const cpp3ds::Vector2f& getContentSize() const;

	void setPadding(const cpp3ds::FloatRect& padding);
	void setPadding(float left, float top, float width, float height);
	const cpp3ds::FloatRect& getPadding() const;

	void setColor(const cpp3ds::Color& color) const;
	const cpp3ds::Color& getColor() const;

protected:
	virtual void draw(cpp3ds::RenderTarget& target, cpp3ds::RenderStates states) const;
	void ensureUpdate() const;
	void updateRegions() const;
	void updateVertices() const;

private:
	enum ExpandType {
		Stretch,
		Repeat,
	};
	enum RegionType {
		Static,
		DynamicWidth,
		DynamicHeight,
		DynamicBoth,
	};
	struct Region {
		RegionType type;
		cpp3ds::FloatRect rect;

	};

	const cpp3ds::Texture *m_texture;
	mutable cpp3ds::Color m_color;
	mutable cpp3ds::Vector2f m_contentSize;
	mutable std::vector<Region> m_regions;
	mutable cpp3ds::VertexArray m_vertices;
	mutable cpp3ds::FloatRect m_padding;

	// Used for internal calculation
	mutable float m_staticWidth;
	mutable float m_staticHeight;
	mutable float m_dynamicWidth;
	mutable float m_dynamicHeight;

	mutable bool m_needsUpdate;
};

} // namespace gui3ds

#endif // GUI3DS_NINEPATCH_HPP
