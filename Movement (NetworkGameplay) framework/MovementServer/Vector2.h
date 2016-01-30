#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>
#include "MyMath.h"

struct Vector2
{
	static const Vector2 ZERO;

	bool IsEqual(float a, float b) const;

	Vector2( float a = 0, float b = 0 ); //default constructor
	Vector2( const Vector2 &rhs ); //copy constructor
	void Set( float a, float b ); //Set all data
	Vector2 operator+( const Vector2& rhs ) const; //Vector addition
	Vector2 operator-( const Vector2& rhs ) const; //Vector subtraction
	Vector2 operator-( void ) const; //Unary negation
	Vector2 operator*( float scalar ) const; //Scalar multiplication
	float Length( void ) const; //Get magnitude
	float Dot( const Vector2& rhs ) const; //Dot product
	Vector2 Normalized( void ); //Return a copy of this vector, normalized
	bool operator==(const Vector2& rhs) const;
	Vector2& operator=(const Vector2& rhs);

	static Vector2 MoveToPoint(Vector2 pos, Vector2 destination, float movement);

	float x, y;
};
#endif
