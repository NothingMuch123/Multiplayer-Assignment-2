#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <hgeanim.h>
#include <hge.h>

class Explosion
{
public:
	Explosion();
	~Explosion();

	void Init(float x, float y);
	bool Update(double dt);
	void Render();
	void Reset();

	bool isActive();

private:
	HTEXTURE tex;
	hgeAnimation* anim;
	float x, y;
	float timer;
};

#endif