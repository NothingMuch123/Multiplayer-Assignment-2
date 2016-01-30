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
	: x_(0), y_(0), type_(1)
	{
		id = newid;
	}

	void Reset()
	{
		id = 0;
		x_ = y_ = 0.f;
		type_ = 0;
	}

	unsigned int id;
	float x_;
	float y_;
	int type_;
};

struct ServerEnemy : ServerGameObject
{
	enum ENEMY_TYPE
	{
		E_EASY,
		E_NORMAL,
		E_HARD,
		E_NUM_ENEMY,
	};

	ServerEnemy(ENEMY_TYPE type, unsigned int newid)
		: ServerGameObject(newid), active(false), vel_x(0.f), vel_y(0.f), hp(0), speed(0.f)
	{
	}

	void Reset()
	{
		x_ = y_ = 0.f;
		type_ = 0;
		active = false;
		vel_x = vel_y = 0.f;
		hp = 0;
		speed = 0.f;
	}

	bool active;
	float vel_x;
	float vel_y;
	int hp;
	float speed;
};

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
	ServerEnemy* FetchEnemy();
	ServerEnemy* FindEnemyByID(unsigned int id);

public:
	ServerApp();
	~ServerApp();
	void Loop();
};

#endif