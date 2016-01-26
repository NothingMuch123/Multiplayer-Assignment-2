#include "Enemy.h"

Enemy::Enemy()
	: hp(-1)
	, type(E_NONE)
	, visibility(-1)
	, destination(Vector2::ZERO)
{
}

Enemy::~Enemy()
{
}

void Enemy::Init(ENEMY_TYPE type, float x, float y, float w, bool active)
{
	GameObject::Init(x, y, w, active);
}

void Enemy::Update(double dt)
{
	if (x == destination.x && y == destination.y)
	{
		// Reached destination
		Reset();
	}
	else
	{
		Vector2 pos(x, y);
		Vector2 pos = Vector2::MoveToPoint(pos, destination, dt);
		x = pos.x;
		y = pos.y;
	}
}

void Enemy::Reset()
{
	type = E_NONE;
	hp = -1;
	visibility = -1;
	destination = Vector2::ZERO;
	GameObject::Reset();
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
