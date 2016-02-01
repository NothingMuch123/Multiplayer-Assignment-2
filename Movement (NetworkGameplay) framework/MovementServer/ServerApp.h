#ifndef SERVERAPP_H_
#define SERVERAPP_H_

#include "RakNetTypes.h"
#include <map>
#include <vector>
#include "Vector2.h"

using std::vector;

class RakPeerInterface;

struct ServerGameObject 
{
	ServerGameObject(unsigned int newid)
	: x_(0), y_(0), type_(1), active(false)
	{
		id = newid;
	}

	void Reset()
	{
		//id = 0;
		x_ = y_ = 0.f;
		type_ = 0;
		active = false;
	}

	unsigned int id;
	float x_;
	float y_;
	int type_;
	bool active;
};

struct ServerVelocityObject : ServerGameObject
{
	ServerVelocityObject(unsigned int newid)
		: ServerGameObject(newid), vel_x(0.f), vel_y(0.f), speed(0.f)
	{
	}

	void Reset()
	{
		ServerGameObject::Reset();
		speed = 0.f;
		vel_x = vel_y = 0.f;
	}

	float speed;
	float vel_x;
	float vel_y;
};

struct ServerEnemy : ServerVelocityObject
{
	enum ENEMY_TYPE
	{
		E_EASY = 1,
		E_NORMAL,
		E_HARD,
		E_NUM_ENEMY,
	};

	ServerEnemy(ENEMY_TYPE type, unsigned int newid)
		: ServerVelocityObject(newid),  hp(0)
	{
	}

	void Reset()
	{
		ServerVelocityObject::Reset();
		x_ = y_ = 0.f;
		type_ = 0;
		hp = 0;
	}

	int hp;
};

/*struct ServerProjectile : ServerGameObject
{
	enum PROJECTILE_TYPE
	{
		PROJ_NORMAL,
		PROJ_SEEKING,
	};

	ServerProjectile(unsigned int newid)
		: ServerGameObject(newid)
	{
	}
};*/

class ServerApp
{
public:
	static int s_screen_width;
	static int s_screen_height;
	static const float S_SPAWN_OFFSET;

private:
	RakPeerInterface* rakpeer_;
	typedef std::map<SystemAddress, ServerGameObject> ClientMap;

	ClientMap clients_;
	vector<SystemAddress> clientList;

	unsigned int newID;
	double prevTime;
	
	void SendWelcomePackage(SystemAddress& addr);
	void SendDisconnectionNotification(SystemAddress& addr);
	void ProcessInitialPosition( SystemAddress& addr, float x_, float y_, int type_);
	void UpdatePosition( SystemAddress& addr, float x_, float y_ );
	void RejectPlayer(SystemAddress& addr);
	bool RemoveClientFromList(SystemAddress& addr);

	// Enemy
	vector<ServerEnemy*> enemyList;
	void InitEnemyList();
	void SpawnEnemy();
	void UpdateEnemy(double dt);
	void ResetEnemy(unsigned int id);
	void SendEnemy();
	ServerEnemy* FetchEnemy();
	ServerEnemy* FindEnemyByID(unsigned int id);

public:
	ServerApp();
	~ServerApp();
	bool Loop();
};

#endif