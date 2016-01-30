#include "GameObject.h"
#include <hgesprite.h>


GameObject::GameObject()
	: id (INVALID_ID) // Invalid id at start
	, x(0.f)
	, y(0.f)
	, w(0.f)
	, velocity_x(0.f)
	, velocity_y(0.f)
	, sprite(nullptr)
	, active(false)
{
}

GameObject::~GameObject()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex);
	hge->Release();
}

void GameObject::Init(float x, float y, float w, bool active)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->active = active;
}

bool GameObject::Update(double dt)
{
	return false;
}

void GameObject::Render()
{
	if (active)
	{
		sprite->RenderEx(x, y, w);
	}
}

void GameObject::Reset()
{
	id = INVALID_ID;
	x = y = w = velocity_x = velocity_y = 0.f;
	active = false;
}

hgeRect * GameObject::GetBoundingBox()
{
	if (sprite.get()) // If sprite exists
	{
		sprite->GetBoundingBox(x, y, &collidebox);

		return &collidebox;
	}
	return nullptr;
}
