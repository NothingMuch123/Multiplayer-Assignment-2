#ifndef BOOM_H
#define BOOM_H

#include <hgeSprite.h>
#include <hge.h>

class Boom
{
public:
	Boom();
	~Boom();

	void Init(float x, float y);
	bool Update(double dt);
	void Render();
	void Reset();

	bool isActive();

private:
	HTEXTURE tex;
	hgeSprite* sprite;
	float x, y;
	float timer;
	bool active;
};

#endif