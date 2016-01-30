#include "Vector2.h"

const Vector2 Vector2::ZERO(0, 0);

bool Vector2::IsEqual(float a, float b) const
{
	return a - b <= Math::EPSILON && b - a <= Math::EPSILON;
}

Vector2::Vector2( float a, float b ) : x(a), y(b)
{
}

Vector2::Vector2( const Vector2 &rhs ) : x(rhs.x), y(rhs.y)
{
}

void Vector2::Set( float a, float b )
{
	x = a;
	y = b;
}

Vector2 Vector2::operator+( const Vector2& rhs ) const
{
	return Vector2(x + rhs.x, y + rhs.y);
}

Vector2 Vector2::operator-( const Vector2& rhs ) const
{
	return Vector2(x - rhs.x, y - rhs.y);
}

Vector2 Vector2::operator-( void ) const
{
	return Vector2(-x, -y);
}

Vector2 Vector2::operator*( float scalar ) const
{
	return Vector2(x * scalar, y * scalar);
}

float Vector2::Length( void ) const
{
	return sqrt(x * x + y * y);
}

float Vector2::Dot( const Vector2& rhs ) const
{
	return (x * rhs.x + y * rhs.y);
}

Vector2 Vector2::Normalized( void )
{
	float d = Length();
	if(d <= Math::EPSILON && -d <= Math::EPSILON)
	{
	  throw DivideByZero();
	}
	return Vector2(x / d, y / d);
}

bool Vector2::operator==(const Vector2 & rhs) const
{
	return IsEqual(x, rhs.x) && IsEqual(y, rhs.y);
}

Vector2 & Vector2::operator=(const Vector2 & rhs)
{
	x = rhs.x;
	y = rhs.y;
	return *this;
}

Vector2 Vector2::MoveToPoint(Vector2 pos, Vector2 destination, float movement)
{
	if (pos == destination)
	{
		return pos;
	}
	Vector2 relative = (destination - pos);
	float distance = relative.Length();
	if (distance < movement)
	{
		// Reach destination
		return destination;
	}
	else
	{
		return pos + relative.Normalized() * movement;
	}
}
