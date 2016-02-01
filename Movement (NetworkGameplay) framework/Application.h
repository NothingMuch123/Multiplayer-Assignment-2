#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "ship.h"
#include "missile.h"
#include <vector>

// Enemies
#include "Enemy.h"

// Animation
#include "Explosion.h"

// Projectile
#include "Projectile.h"

using std::vector;

class HGE;
class RakPeerInterface;

//! The default angular velocity of the ship when it is in motion
static const float DEFAULT_ANGULAR_VELOCITY = 3.0f; 
//! The default acceleration of the ship when powered
static const float DEFAULT_ACCELERATION = 100.0f;

/**
* The application class is the main body of the program. It will
* create an instance of the graphics engine and execute the
* Update/Render cycle.
*
*/

class Application
{
public:
	static const int S_SCREEN_WIDTH = 800;
	static const int S_SCREEN_HEIGHT = 600;
	static const float S_BULLET_SHOOT_INTERVAL;
	static const float S_MISSILE_SHOOT_INTERVAL;

private:
	HGE* hge_; //!< Instance of the internal graphics engine
	typedef std::vector<Ship*> ShipList;  //!< A list of ships
	ShipList ships_; //!< List of all the ships in the universe
	RakPeerInterface* rakpeer_;
	unsigned int timer_;

	// Enemy
	vector<Enemy*> enemyList;
	Enemy* FindEnemyByID(int id);
	void InitEnemyList();
	void DestroyEnemy(Enemy* e);

	// Explosion
	vector<Explosion*> explosionList;
	void InitExplosionList();
	Explosion* FetchExplosion();

	// Projectile
	vector<Projectile*> projectileList;
	void InitProjectileList();
	Projectile* FetchProjectile();
	Projectile* FindProjectileByID(int id);
	void Shoot(Projectile::PROJECTILE_TYPE type);
	Enemy* FindNearest();
	float bulletShootTimer;
	float missileShootTimer;
	vector<Projectile*> projectileUpdateList;

	// Ship
	Ship* FindShipByID(int id);

	
	// Lab 13 Task 1 : add variables for local missle
	Missile* mymissile; // This player's missle
	bool have_missile;
	bool keydown_enter;

	// Lab 13 Task 8 : add variables to handle networked missiles
	typedef std::vector<Missile*> MissileList;
	MissileList missiles_; // List of missions for other players

	bool Init();
	static bool Loop();
	void Shutdown();
	bool checkCollisions(Ship* ship);
	void ProcessWelcomePackage();
	bool SendInitialPosition();
	void SendScreenSize();

	// Lab 13
	void CreateMissile( float x, float y, float w, int id );
	bool RemoveMissile( float x, float y, float w, int id );

	void SendCollision( Ship* ship );

public:
	Application();
	~Application() throw();

	void Start();
	bool Update();
	void Render();

	static float CalcW(Vector2 dir);
};

#endif