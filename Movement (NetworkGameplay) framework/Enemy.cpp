#include "Enemy.h"
#include "Application.h"
#include <hgesprite.h>

Enemy::Enemy()
	: hp(-1)
	, type(E_NONE)
	, visibility(-1)
	, destination(Vector2::ZERO)
	, speed(0.f)
{
}

Enemy::~Enemy()
{
}

void Enemy::Init(ENEMY_TYPE type, float x, float y, float w, bool active)
{
	GameObject::Init(x, y, w, active);

	// Set up default values
	this->type = type;
	destination.Set(Application::S_SCREEN_WIDTH * 0.5f, Application::S_SCREEN_HEIGHT * 0.5f);
	visibility = -1;

	// Load sprite
	HGE* hge = hgeCreate(HGE_VERSION);
	switch (type)
	{
	case E_EASY:
		{
			tex = hge->Texture_Load("ship4.png");
		}
		break;
	case E_NORMAL:
		{
			tex = hge->Texture_Load("ship4.png");
		}
		break;
	case E_HARD:
		{
			tex = hge->Texture_Load("ship4.png");
		}
		break;
	}

	hge->Release();
	sprite.reset(new hgeSprite(tex, 0, 0, 64, 64));
}

bool Enemy::Update(double dt)
{
	GameObject::Update(dt);
	if (x == destination.x && y == destination.y)
	{
		// Reached destination
		return true;
	}
	else
	{
		Vector2 pos(x, y);
		Vector2 newPos = Vector2::MoveToPoint(pos, destination, speed * dt);
		x = newPos.x;
		y = newPos.y;
	}
	return false;
}

void Enemy::Render()
{
	GameObject::Render();
}

void Enemy::Reset()
{
	type = E_NONE;
	hp = -1;
	visibility = -1;
	destination = Vector2::ZERO;
	// Do not use GameObject::Reset() as ID should not be reset
	x = y = w = velocity_x = velocity_y = 0.f;
	active = false;
	//GameObject::Reset();
}

void Enemy::SetHP(int hp)
{
	this->hp = hp;
}

int Enemy::GetHP()
{
	return hp;
}

void Enemy::SetType(ENEMY_TYPE type)
{
	this->type = type;
}

Enemy::ENEMY_TYPE Enemy::GetType()
{
	return type;
}

void Enemy::SetVisibility(int visibility)
{
	this->visibility = visibility;
}

int Enemy::GetVisibility()
{
	return visibility;
}

void Enemy::SetDestination(Vector2 destination)
{
	this->destination = destination;
}

Vector2 & Enemy::GetDestination()
{
	return destination;
}

void Enemy::SetSpeed(float speed)
{
	this->speed = speed;
}

float Enemy::GetSpeed()
{
	return speed;
}
