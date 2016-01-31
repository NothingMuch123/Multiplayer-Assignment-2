#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"
#include "ship.h"

class Projectile : public GameObject
{
public:
	enum PROJECTILE_TYPE
	{
		PROJ_NONE,
		PROJ_BULLET,
		PROJ_SEEKING_MISSLE,
	};

	Projectile();
	~Projectile();

	virtual void Init(Ship* owner, PROJECTILE_TYPE type, float x, float y, float w, bool active = true);
	virtual bool Update(double dt);
	virtual void Render();
	virtual void Reset();

	bool CheckDespawn();

	// Setters and Getters
	void SetDamage(int damage);
	int GetDamage();

	void SetType(PROJECTILE_TYPE type);
	PROJECTILE_TYPE GetType();

	void SetTarget(GameObject* target);
	GameObject* GetTarget();

	void SetSpeed(float speed);
	float GetSpeed();

	void SetOwner(Ship* owner);
	Ship* GetOwner();

protected:
	int damage;
	PROJECTILE_TYPE type;
	GameObject* target; // Nullptr if no target
	Ship* owner;
	float speed;
};

#endif