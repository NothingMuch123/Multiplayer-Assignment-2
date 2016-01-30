#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <hge.h>
#include <hgerect.h>
#include <memory>
#include "Vector2.h"

class hgeSprite;

class GameObject
{
public:
	static const int INVALID_ID = -1;

	GameObject();
	virtual ~GameObject();

	virtual void Init(float x, float y, float w, bool active = true);
	virtual bool Update(double dt);
	virtual void Render();
	virtual void Reset();

	// Setters and Getters
	void SetID(int id) { this->id = id; }
	int GetID() { return id; }

	void SetActive(bool active) { this->active = active; }
	bool GetActive() { return active; }

	hgeRect* GetBoundingBox();

	float GetVelocityX() { return velocity_x; }
	float GetVelocityY() { return velocity_y; }

	void SetVelocityX(float velocity) { velocity_x = velocity; }
	void SetVelocityY(float velocity) { velocity_y = velocity; }

	void setLocation(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	void SetX(float x) { this->x = x; }
	void SetY(float y) { this->y = y; }
	void SetW(float w) { this->w = w; }

	float GetX() { return this->x; }
	float GetY() { return this->y; }
	float GetW() { return this->w; }

	void SetSprite(std::auto_ptr<hgeSprite> sprite) { this->sprite = sprite; }
	std::auto_ptr<hgeSprite> GetSprite() { return sprite; };

protected:
	bool active; // See if this object is active or not
	HTEXTURE tex; //!< Handle to the sprite's texture
	std::auto_ptr<hgeSprite> sprite; //!< The sprite used to display the ship

	hgeRect collidebox; // Collision

	int id;
	float x; //!< The x-ordinate of the ship
	float y; //!< The y-ordinate of the ship
	float w; //!< The angular position of the ship (Used for direction it is facing)
	float velocity_x; //!< The resolved velocity of the ship along the x-axis
	float velocity_y; //!< The resolved velocity of the ship along the y-axis
};

#endif