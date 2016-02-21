#ifndef UTIL3DS_TWEENABLEOBJECTS_H
#define UTIL3DS_TWEENABLEOBJECTS_H

#include <TweenEngine/Tweenable.h>
#include <cpp3ds/Graphics/Transformable.hpp>
#include <cpp3ds/Graphics/Sprite.hpp>
#include <cpp3ds/Graphics/Text.hpp>
#include <cpp3ds/Graphics/Shape.hpp>
#include <cpp3ds/Graphics/RectangleShape.hpp>
#include <cpp3ds/Graphics/CircleShape.hpp>
#include <cpp3ds/Graphics/ConvexShape.hpp>

namespace util3ds {

template <class T>
class TweenTransformable: public T, public TweenEngine::Tweenable
{
public:
	static const int POSITION_X  = 1;
	static const int POSITION_Y  = 2;
	static const int POSITION_XY = 3;
	static const int ROTATION    = 4;
	static const int SCALE_X     = 5;
	static const int SCALE_Y     = 6;
	static const int SCALE_XY    = 7;
	static const int ORIGIN_X    = 8;
	static const int ORIGIN_Y    = 9;
	static const int ORIGIN_XY   = 10;

protected:
	virtual int getValues(int tweenType, float *returnValues)
	{
		switch (tweenType) {
			case POSITION_X: returnValues[0] = T::getPosition().x; return 1;
			case POSITION_Y: returnValues[0] = T::getPosition().y; return 1;
			case POSITION_XY:
				returnValues[0] = T::getPosition().x;
				returnValues[1] = T::getPosition().y;
				return 2;
			case ROTATION: returnValues[0] = T::getRotation(); return 1;
			case SCALE_X: returnValues[0] = T::getScale().x; return 1;
			case SCALE_Y: returnValues[0] = T::getScale().y; return 1;
			case SCALE_XY:
				returnValues[0] = T::getScale().x;
				returnValues[1] = T::getScale().y;
				return 2;
			case ORIGIN_X: returnValues[0] = T::getOrigin().x; return 1;
			case ORIGIN_Y: returnValues[0] = T::getOrigin().y; return 1;
			case ORIGIN_XY:
				returnValues[0] = T::getOrigin().x;
				returnValues[1] = T::getOrigin().y;
				return 2;
			default: return -1;
		}
	}

	virtual void setValues(int tweenType, float *newValues)
	{
		switch (tweenType) {
			case POSITION_X: T::setPosition(newValues[0], T::getPosition().y); break;
			case POSITION_Y: T::setPosition(T::getPosition().x, newValues[0]); break;
			case POSITION_XY: T::setPosition(newValues[0], newValues[1]); break;
			case ROTATION: T::setRotation(newValues[0]); break;
			case SCALE_X: T::setScale(newValues[0], T::getScale().y); break;
			case SCALE_Y: T::setScale(T::getScale().x, newValues[0]); break;
			case SCALE_XY: T::setScale(newValues[0], newValues[1]); break;
			case ORIGIN_X: T::setOrigin(newValues[0], T::getOrigin().y); break;
			case ORIGIN_Y: T::setOrigin(T::getOrigin().x, newValues[0]); break;
			case ORIGIN_XY: T::setOrigin(newValues[0], newValues[1]); break;
			default:
				break;
		}
	}
};


template <class T>
class TweenShape: public TweenTransformable<T>
{
public:
	static const int FILL_COLOR_RGB      = 11;
	static const int FILL_COLOR_ALPHA    = 12;
	static const int OUTLINE_COLOR_RGB   = 13;
	static const int OUTLINE_COLOR_ALPHA = 14;
	static const int OUTLINE_THICKNESS   = 15;

protected:
	virtual int getValues(int tweenType, float *returnValues)
	{
		switch (tweenType) {
			case FILL_COLOR_RGB: {
				cpp3ds::Color color = T::getFillColor();
				returnValues[0] = color.r;
				returnValues[1] = color.g;
				returnValues[2] = color.b;
				return 3;
			}
			case OUTLINE_COLOR_RGB: {
				cpp3ds::Color color = T::getOutlineColor();
				returnValues[0] = color.r;
				returnValues[1] = color.g;
				returnValues[2] = color.b;
				return 3;
			}
			case FILL_COLOR_ALPHA: returnValues[0] = T::getFillColor().a; return 1;
			case OUTLINE_COLOR_ALPHA: returnValues[0] = T::getOutlineColor().a; return 1;
			case OUTLINE_THICKNESS: returnValues[0] = T::getOutlineThickness(); return 1;
			default:
				return TweenTransformable<T>::getValues(tweenType, returnValues);
		}
	}

	virtual void setValues(int tweenType, float *newValues)
	{
		switch (tweenType) {
			case FILL_COLOR_RGB: {
				cpp3ds::Color color;
				color.r = std::max(std::min(newValues[0], 255.f), 0.f);
				color.g = std::max(std::min(newValues[1], 255.f), 0.f);
				color.b = std::max(std::min(newValues[2], 255.f), 0.f);
				color.a = T::getFillColor().a;
				T::setFillColor(color);
				break;
			}
			case FILL_COLOR_ALPHA: {
				cpp3ds::Color color = T::getFillColor();
				color.a = std::max(std::min(newValues[0], 255.f), 0.f);
				T::setFillColor(color);
				break;
			}
			case OUTLINE_COLOR_RGB: {
				cpp3ds::Color color;
				color.r = std::max(std::min(newValues[0], 255.f), 0.f);
				color.g = std::max(std::min(newValues[1], 255.f), 0.f);
				color.b = std::max(std::min(newValues[2], 255.f), 0.f);
				color.a = T::getOutlineColor().a;
				T::setOutlineColor(color);
				break;
			}
			case OUTLINE_COLOR_ALPHA: {
				cpp3ds::Color color = T::getOutlineColor();
				color.a = std::max(std::min(newValues[0], 255.f), 0.f);
				T::setOutlineColor(color);
				break;
			}
			case OUTLINE_THICKNESS: T::setOutlineThickness(newValues[0]); break;
			default:
				TweenTransformable<T>::setValues(tweenType, newValues);
		}
	}
};


template <class T>
class TweenColorTransformable : public TweenTransformable<T>
{
public:
	static const int COLOR_RGB   = 11;
	static const int COLOR_ALPHA = 12;

protected:
	virtual int getValues(int tweenType, float *returnValues)
	{
		switch (tweenType) {
			case COLOR_RGB: {
				cpp3ds::Color color = T::getColor();
				returnValues[0] = color.r;
				returnValues[1] = color.g;
				returnValues[2] = color.b;
				return 3;
			}
			case COLOR_ALPHA: returnValues[0] = T::getColor().a; return 1;
			default:
				return TweenTransformable<T>::getValues(tweenType, returnValues);
		}
	}

	virtual void setValues(int tweenType, float *newValues)
	{
		switch (tweenType) {
			case COLOR_RGB: {
				cpp3ds::Color color;
				color.r = std::max(std::min(newValues[0], 255.f), 0.f);
				color.g = std::max(std::min(newValues[1], 255.f), 0.f);
				color.b = std::max(std::min(newValues[2], 255.f), 0.f);
				color.a = T::getColor().a;
				T::setColor(color);
				break;
			}
			case COLOR_ALPHA: {
				cpp3ds::Color color = T::getColor();
				color.a = std::max(std::min(newValues[0], 255.f), 0.f);
				T::setColor(color);
				break;
			}
			default:
				TweenTransformable<T>::setValues(tweenType, newValues);
				break;
		}
	}
};


typedef TweenColorTransformable<cpp3ds::Sprite> TweenSprite;
typedef TweenShape<cpp3ds::Text> TweenText;

typedef TweenShape<cpp3ds::RectangleShape> TweenRectangleShape;
typedef TweenShape<cpp3ds::CircleShape> TweenCircleShape;
typedef TweenShape<cpp3ds::ConvexShape> TweenConvexShape;

} // namespace util3ds

#endif // UTIL3DS_TWEENABLEOBJECTS_H
