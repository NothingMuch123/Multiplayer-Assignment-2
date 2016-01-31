#include "Explosion.h"



Explosion::Explosion()
	: x(0.f), y(0.f), timer(0.f)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	if (hge)
	{
		tex = hge->Texture_Load("explosion.png");
		anim = new hgeAnimation(tex, 30, 30.f, 0, 0, 64, 206 / 3);
		anim->Stop();
	}
	hge->Release();
}


Explosion::~Explosion()
{
	if (anim)
	{
		delete anim;
		anim = nullptr;
	}

	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex);
	hge->Release();
}

void Explosion::Init(float x, float y)
{
	this->x = x;
	this->y = y;
	timer = 0.f;
	anim->Play();
}

bool Explosion::Update(double dt)
{
	timer += dt;
	anim->Update(dt);
	if (timer >= 1.f) // Set animation as ended
	{
		Reset();
		return true;
	}
	return false;
}

void Explosion::Render()
{
	anim->RenderEx(x, y, 0);
}

void Explosion::Reset()
{
	anim->Stop();
	x = y = 0.f;
	timer = 0.f;
}

bool Explosion::isActive()
{
	return anim->IsPlaying();
}
