#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "GameObject.h"

class Enemy : public GameObject
{
public:
	enum ENEMY_TYPE
	{
		E_NONE,
		E_WEAK,
		E_NORMAL,
		E_HARD,
	};

	Enemy();
	virtual ~Enemy();

	virtual void Init(ENEMY_TYPE type, float x, float y, float w, bool active = true);
	virtual void Update(double dt);
	virtual void Reset();

	// Setters and Getters
	void SetHP(int hp);
	int GetHP();
	
	void SetType(ENEMY_TYPE type);
	ENEMY_TYPE GetType();

	void SetVisibility(int visibility);
	int GetVisibility();

	void SetDestination(Vector2 destination);
	Vector2& GetDestination();

protected:
	int hp;
	ENEMY_TYPE type;
	int visibility; // Which player can kill it (-1 = All | 0 = Player 1 | 1 = Player 2)
	Vector2 destination;


};

#endif