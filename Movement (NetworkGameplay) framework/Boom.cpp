#include "Boom.h"



Boom::Boom()
	: x(0.f), y(0.f), timer(0.f), active(false)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	if (hge)
	{
		tex = hge->Texture_Load("boom2.png");
		sprite = new hgeSprite(tex, 0, 0, 40, 32);
		sprite->SetHotSpot(20, 16);
	}
	hge->Release();
}


Boom::~Boom()
{
	if (sprite)
	{
		delete sprite;
		sprite = nullptr;
	}

	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex);
	hge->Release();
}

void Boom::Init(float x, float y)
{
	this->x = x;
	this->y = y;
	this->active = true;
}

bool Boom::Update(double dt)
{
	timer += dt;
	if (timer >= 0.2f) // Set animation as ended
	{
		Reset();
		return true;
	}
	return false;
}

void Boom::Render()
{
	sprite->RenderEx(x, y, 0);
}

void Boom::Reset()
{
	x = y = 0.f;
	timer = 0.f;
	active = false;
}

bool Boom::isActive()
{
	return active;
}
