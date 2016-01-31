#include "Projectile.h"
#include "Application.h"


Projectile::Projectile()
	: GameObject()
	, damage(0)
	, type(PROJ_NONE)
	, target(nullptr)
	, speed(0.f)
{
}


Projectile::~Projectile()
{
}

void Projectile::Init(Ship* owner, PROJECTILE_TYPE type, float x, float y, float w, bool active)
{
	GameObject::Init(x, y, w, active);
	this->type = type;
	this->owner = owner;

	HGE* hge = hgeCreate(HGE_VERSION);

	switch (type)
	{
	case PROJ_BULLET:
		{
			tex = hge->Texture_Load("missile.png");
			sprite.reset(new hgeSprite(tex, 0, 0, 40, 20));
			sprite.get()->SetHotSpot(20, 10);
		}
		break;
	case PROJ_SEEKING_MISSLE:
		{
			tex = hge->Texture_Load("missile.png");
			sprite.reset(new hgeSprite(tex, 0, 0, 40, 20));
			sprite.get()->SetHotSpot(20, 10);
		}
		break;
	}

	hge->Release();
}

bool Projectile::Update(double dt)
{
	GameObject::Update(dt);

	if (CheckDespawn()) // Callback for despawn
	{
		return true;
	}
	else // Continue moving
	{
		Vector2 pos(x, y);
		switch (type)
		{
		case PROJ_BULLET:
			{
				// Straight movement
				Vector2 vel(velocity_x, velocity_y);
				pos = pos + (vel * dt);
				x = pos.x;
				y = pos.y;
				return false;
			}
			break;
		case PROJ_SEEKING_MISSLE:
			{
				if (target)
				{
					// Seeking
					Vector2 newPos = Vector2::MoveToPoint(pos, Vector2(target->GetX(), target->GetY()), speed * dt);
					x = newPos.x;
					y = newPos.y;
					w = -Application::CalcW((newPos - Vector2(target->GetX(), target->GetY())));
					return false;
				}
				else
				{
					// Straight movement
					Vector2 vel(velocity_x, velocity_y);
					pos = pos + (vel * dt);
					x = pos.x;
					y = pos.y;
					return false;
				}
			}
			break;
			
		}
	}
	return false;
}

void Projectile::Render()
{
	GameObject::Render();
}

void Projectile::Reset()
{
	//GameObject::Reset();
	// Do not use GameObject::Reset() as ID should not be reset
	x = y = w = velocity_x = velocity_y = 0.f;
	active = false;

	damage = 0;
	type = PROJ_NONE;
	target = nullptr;
	speed = 0.f;
}

bool Projectile::CheckDespawn()
{
	if (sprite.get())
	{
		Vector2 minBound(x - sprite.get()->GetWidth() * 0.5f, y - sprite.get()->GetHeight() * 0.5f);
		Vector2 maxBound(x + sprite.get()->GetWidth() * 0.5f, y + sprite.get()->GetHeight() * 0.5f);
		if (maxBound.x < 0.f ||
			minBound.x > Application::S_SCREEN_WIDTH ||
			maxBound.y < 0.f ||
			minBound.y > Application::S_SCREEN_HEIGHT)
		{
			return true;
		}
		return false;
	}
}

void Projectile::SetDamage(int damage)
{
	this->damage = damage;
}

int Projectile::GetDamage()
{
	return damage;
}

void Projectile::SetType(PROJECTILE_TYPE type)
{
	this->type = type;
}

Projectile::PROJECTILE_TYPE Projectile::GetType()
{
	return type;
}

void Projectile::SetTarget(GameObject * target)
{
	this->target = target;
}

GameObject * Projectile::GetTarget()
{
	return target;
}

void Projectile::SetSpeed(float speed)
{
	this->speed = speed;
	velocity_x = cosf(w) * speed;
	velocity_y = sinf(w) * speed;
}

float Projectile::GetSpeed()
{
	return speed;
}

void Projectile::SetOwner(Ship * owner)
{
	this->owner = owner;
}

Ship * Projectile::GetOwner()
{
	return owner;
}
