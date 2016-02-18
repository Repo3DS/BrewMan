#include "NinePatch.hpp"

namespace gui3ds {

NinePatch::NinePatch()
: m_color(cpp3ds::Color::White)
, m_padding(0,0,0,0)
, m_needsUpdate(true)
{
	setTexture(nullptr);
}


NinePatch::~NinePatch()
{

}


void NinePatch::setTexture(const cpp3ds::Texture *texture)
{
	m_texture = texture;
	updateRegions();
	m_needsUpdate = true;
}


const cpp3ds::Texture* NinePatch::getTexture() const
{
	return m_texture;
}


void NinePatch::setContentSize(const cpp3ds::Vector2f &size) const
{
	m_contentSize = size;
	m_needsUpdate = true;
}


void NinePatch::setContentSize(float width, float height) const
{
	setContentSize(cpp3ds::Vector2f(width, height));
}


const cpp3ds::Vector2f &NinePatch::getContentSize() const
{
	ensureUpdate();
	return m_contentSize;
}


void NinePatch::setPadding(const cpp3ds::FloatRect &padding)
{
	m_needsUpdate = true;
	m_padding = padding;
}


void NinePatch::setPadding(float left, float top, float width, float height)
{
	setPadding(cpp3ds::FloatRect(left, top, width, height));
}


const cpp3ds::FloatRect &NinePatch::getPadding() const
{
	ensureUpdate();
	return m_padding;
}


void NinePatch::setColor(const cpp3ds::Color &color) const
{
	m_color = color;
	m_needsUpdate = true;
}


const cpp3ds::Color &NinePatch::getColor() const
{
	return m_color;
}


void NinePatch::draw(cpp3ds::RenderTarget &target, cpp3ds::RenderStates states) const
{
	ensureUpdate();
	states.texture = m_texture;
	target.draw(m_vertices, states);
}


void NinePatch::ensureUpdate() const
{
	if (m_needsUpdate) {
		updateVertices();
		m_needsUpdate = false;
	}
}


void NinePatch::updateRegions() const
{
	m_regions.clear();

	if (!m_texture)
	{
		Region region;
		region.type = DynamicBoth;
		region.rect = cpp3ds::FloatRect(0.f, 0.f, 1.f, 1.f);
		m_dynamicHeight = m_dynamicWidth = 1.f;
		m_staticHeight = m_staticWidth = 0.f;
		m_regions.push_back(region);
		return;
	}

	cpp3ds::Image image = m_texture->copyToImage();

	unsigned int x, y, last_x, last_y;
	bool is_static_y = true;
	bool is_static_x = true;

	last_x = 1;
	last_y = 1;
	m_staticHeight = 0;
	m_dynamicHeight = 0;

	for (y = 1; y < image.getSize().y - 1; y++)
	{
		cpp3ds::Color color_y = image.getPixel(0, y);
		bool is_region_y = is_static_y ? (color_y == cpp3ds::Color::Black) : (color_y.a == 0);
		if (y == image.getSize().y - 2) {
			is_region_y = true;
			y++;
		}
		if (is_region_y)
		{
			m_staticWidth = 0;
			m_dynamicWidth = 0;
			is_static_x = true;
			for (x = 1; x < image.getSize().x - 1; x++)
			{
				cpp3ds::Color color_x = image.getPixel(x, 0);
				bool is_region_x = is_static_x ? (color_x == cpp3ds::Color::Black) : (color_x.a == 0);
				if (x == image.getSize().x - 2) {
					is_region_x = true;
					x++;
				}
				if (is_region_x)
				{
					// add region
					Region region;
					if (is_static_x) {
						region.type = is_static_y ? Static : DynamicHeight;
					} else {
						region.type = is_static_y ? DynamicWidth : DynamicBoth;
					}
					region.rect.left = last_x;
					region.rect.top  = last_y;
					region.rect.width  = x - last_x;
					region.rect.height = y - last_y;

					m_regions.push_back(region);

					if (is_static_x)
						m_staticWidth += x - last_x;
					else
						m_dynamicWidth += x - last_x;
					last_x = x;
					is_static_x = !is_static_x;
				}
			}

			if (is_static_y)
				m_staticHeight += y - last_y;
			else
				m_dynamicHeight += y - last_y;
			last_x = 1;
			last_y = y;
			is_static_y = !is_static_y;
		}
	}

	// Get padding
	bool is_padding = false;
	for (y = 1; y < image.getSize().y - 1; y++)
	{
		cpp3ds::Color color = image.getPixel(image.getSize().x-1, y);
		if (is_padding) {
			if (color.a == 0) {
				m_padding.height = y - m_padding.top;
				break;
			}
		} else {
			if (color == cpp3ds::Color::Black) {
				is_padding = true;
				m_padding.top = y;
				continue;
			}
		}
	}
	is_padding = false;
	for (x = 1; x < image.getSize().x - 1; x++)
	{
		cpp3ds::Color color = image.getPixel(x, image.getSize().y-1);
		if (is_padding) {
			if (color.a == 0) {
				m_padding.width = x - m_padding.left;
				break;
			}
		} else {
			if (color == cpp3ds::Color::Black) {
				is_padding = true;
				m_padding.left = x;
				continue;
			}
		}
	}
}


void NinePatch::updateVertices() const
{
	m_vertices.clear();
	cpp3ds::Vertex vertices[4];

	m_contentSize.x = std::max(m_contentSize.x, m_padding.width);
	m_contentSize.y = std::max(m_contentSize.y, m_padding.height);

	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = m_color;

	float dynamicWidthSum  = 0;
	float dynamicHeightSum = 0;
	float dynamicWidth;
	float dynamicHeight;
	bool addedHeight = false;
	bool addedWidth  = false;

	for (const auto &region: m_regions)
	{
		if (region.rect.left <= 1) {
			dynamicWidthSum = 0;
		}

		vertices[0].texCoords.x = region.rect.left;
		vertices[0].texCoords.y = region.rect.top;
		vertices[1].texCoords.x = region.rect.left + region.rect.width;
		vertices[1].texCoords.y = region.rect.top;
		vertices[2].texCoords.x = region.rect.left;
		vertices[2].texCoords.y = region.rect.top + region.rect.height;
		vertices[3].texCoords.x = region.rect.left + region.rect.width;
		vertices[3].texCoords.y = region.rect.top + region.rect.height;

		if (region.type == DynamicWidth || region.type == DynamicBoth) {
			dynamicWidth = std::floor((region.rect.width / m_dynamicWidth) * (m_contentSize.x - m_padding.width));
			if (!addedWidth) {
				addedWidth = true;
				dynamicWidthSum += dynamicWidth;
			}
		}

		if (region.type == DynamicHeight || region.type == DynamicBoth) {
			dynamicHeight = std::floor((region.rect.height / m_dynamicHeight) * (m_contentSize.y - m_padding.height));
			if (!addedHeight) {
				addedHeight = true;
				dynamicHeightSum += dynamicHeight;
			}
		}

		switch (region.type) {
			case DynamicWidth:
				addedHeight = false;
				vertices[0].position.x = region.rect.left + dynamicWidthSum - dynamicWidth - 1;
				vertices[0].position.y = region.rect.top  + dynamicHeightSum - 1;
				vertices[1].position.x = region.rect.left + region.rect.width + dynamicWidthSum - 1;
				vertices[1].position.y = region.rect.top  + dynamicHeightSum - 1;
				vertices[2].position.x = region.rect.left + dynamicWidthSum - dynamicWidth - 1;
				vertices[2].position.y = region.rect.top  + region.rect.height + dynamicHeightSum - 1;
				break;
			case DynamicHeight:
				addedWidth = false;
				vertices[0].position.x = region.rect.left + dynamicWidthSum - 1;
				vertices[0].position.y = region.rect.top  + dynamicHeightSum - dynamicHeight - 1;
				vertices[1].position.x = region.rect.left + region.rect.width + dynamicWidthSum - 1;
				vertices[1].position.y = region.rect.top  + dynamicHeightSum - dynamicHeight - 1;
				vertices[2].position.x = region.rect.left + dynamicWidthSum - 1;
				vertices[2].position.y = region.rect.top  + region.rect.height + dynamicHeightSum - 1;
				break;
			case DynamicBoth:
				vertices[0].position.x = region.rect.left + dynamicWidthSum  - dynamicWidth  - 1;
				vertices[0].position.y = region.rect.top  + dynamicHeightSum - dynamicHeight - 1;
				vertices[1].position.x = region.rect.left + region.rect.width + dynamicWidthSum - 1;
				vertices[1].position.y = region.rect.top  + dynamicHeightSum - dynamicHeight - 1;
				vertices[2].position.x = region.rect.left + dynamicWidthSum  - dynamicWidth  - 1;
				vertices[2].position.y = region.rect.top  + region.rect.height + dynamicHeightSum - 1;
				break;
			case Static:
			default:
				addedWidth = false;
				addedHeight = false;
				vertices[0].position = cpp3ds::Vector2f(region.rect.left+dynamicWidthSum-1, region.rect.top+dynamicHeightSum-1);
				vertices[1].position = cpp3ds::Vector2f(region.rect.left+dynamicWidthSum-1 + region.rect.width, region.rect.top+dynamicHeightSum-1);
				vertices[2].position = cpp3ds::Vector2f(region.rect.left+dynamicWidthSum-1, region.rect.top+dynamicHeightSum-1 + region.rect.height);
				break;
		}

		// Bottom-right position is the same for all region rectangles
		vertices[3].position.x = region.rect.left + region.rect.width  + dynamicWidthSum  - 1;
		vertices[3].position.y = region.rect.top  + region.rect.height + dynamicHeightSum - 1;

		// Add vertices counter-clockwise as two triangles
		m_vertices.append(vertices[0]);
		m_vertices.append(vertices[2]);
		m_vertices.append(vertices[3]);

		m_vertices.append(vertices[1]);
		m_vertices.append(vertices[0]);
		m_vertices.append(vertices[3]);
	}
}

} // namespace gui3ds
