#include "Application.h"
#include "ship.h"
#include "Globals.h"
#include "MyMsgIDs.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "Bitstream.h"
#include "GetTime.h"
#include <hge.h>
#include <string>
#include <iostream>
#include <fstream>
#include "hgefont.h"

const float Application::S_BULLET_SHOOT_INTERVAL = 0.5f;
const float Application::S_MISSILE_SHOOT_INTERVAL = 1.f;;

// Lab 13 Task 9a : Uncomment the macro NETWORKMISSILE
#define NETWORKMISSLE


float GetAbsoluteMag( float num )
{
	if ( num < 0 )
	{
		return -num;
	}

	return num;
}

/** 
* Constuctor
*
* Creates an instance of the graphics engine and network engine
*/

Application::Application()
	: hge_(hgeCreate(HGE_VERSION))
	, rakpeer_(RakNetworkFactory::GetRakPeerInterface())
	, timer_(0)
	// Lab 13 Task 2 : add new initializations
	, mymissile(NULL)
	, keydown_enter(false)
	, bulletShootTimer(S_BULLET_SHOOT_INTERVAL)
	, missileShootTimer(S_MISSILE_SHOOT_INTERVAL)
	, background(nullptr)
	, base(nullptr)
	, base_hp(5)
	, f_base_hp(nullptr)
	, p1_score(nullptr)
	, p2_score(nullptr)
{
}

/**
* Destructor
*
* Does nothing in particular apart from calling Shutdown
*/

Application::~Application() throw()
{
	Shutdown();
	rakpeer_->Shutdown(100);
	RakNetworkFactory::DestroyRakPeerInterface(rakpeer_);
}

/**
* Initialises the graphics system
* It should also initialise the network system
*/

void Application::InitBackground()
{
	bgTex = hge_->Texture_Load("bg1.png");
	background = new hgeSprite(bgTex, 0, 0, S_SCREEN_WIDTH, S_SCREEN_HEIGHT);
}

void Application::InitBase()
{
	base_hp = 5;
	baseTex = hge_->Texture_Load("moon.png");
	base = new hgeSprite(baseTex, 0, 0, 100, 101);
	base->SetHotSpot(50, 50.5);

	f_base_hp = new hgeFont("font1.fnt");
	f_base_hp->SetScale(3);
}

void Application::InitScore()
{
	p1_score = new hgeFont("font1.fnt");
	p1_score->SetScale(1.5);

	p2_score = new hgeFont("font1.fnt");
	p2_score->SetScale(1.5);
}

Enemy * Application::FindEnemyByID(int id)
{
	for (vector<Enemy*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
	{
		Enemy* e = *it;
		if (e->GetID() == id)
		{
			return e;
		}
	}
	return nullptr;
}

void Application::InitEnemyList()
{
	for (int i = 0; i < 100; ++i)
	{
		Enemy* e = new Enemy();
		e->SetID(i);
		enemyList.push_back(e);
	}
}

void Application::DestroyEnemy(Enemy * e)
{
	if (e->GetActive())
	{
		// Spawn explosion
		Explosion* ex = FetchExplosion();
		if (ex)
		{
			ex->Init(e->GetX(), e->GetY());

			// Reset enemy
			e->Reset();
		}
	}
}

void Application::InitExplosionList()
{
	for (int i = 0; i < 100; ++i)
	{
		Explosion* explosion = new Explosion();
		explosionList.push_back(explosion);
	}
}

Explosion * Application::FetchExplosion()
{
	for (vector<Explosion*>::iterator it = explosionList.begin(); it != explosionList.end(); ++it)
	{
		Explosion* e = *it;
		if (!e->isActive())
		{
			return e;
		}
	}
	return nullptr;
}

void Application::InitProjectileList()
{
	for (int i = 0; i < 100; ++i)
	{
		Projectile* p = new Projectile();
		p->SetID(i);
		projectileList.push_back(p);
	}
}

Projectile * Application::FetchProjectile()
{
	for (vector<Projectile*>::iterator it = projectileList.begin(); it != projectileList.end(); ++it)
	{
		Projectile* p = *it;
		if (!p->GetActive())
		{
			return p;
		}
	}
	return nullptr;
	/*const int BATCH_PRODUCE = 20;
	for (int i = 0; i < BATCH_PRODUCE; ++i)
	{
		Projectile* p = new Projectile();
		projectileList.push_back(p);
	}
	return projectileList.back();*/
}

Projectile * Application::FindProjectileByID(int id)
{
	for (vector<Projectile*>::iterator it = projectileList.begin(); it != projectileList.end(); ++it)
	{
		Projectile* p = *it;
		if (p->GetID() == id)
		{
			return p;
		}
	}
	return nullptr;
}

void Application::Shoot(Projectile::PROJECTILE_TYPE type)
{
	Projectile* p = FetchProjectile();
	if (p)
	{
		float posX = ships_.at(0)->GetX();
		float posY = ships_.at(0)->GetY();
		switch (type)
		{
		case Projectile::PROJ_BULLET:
			{
				// TODO: Find pos
				p->Init(ships_.at(0), type, posX, posY, ships_.at(0)->GetW());
				p->SetDamage(1);
				p->SetSpeed(400.f);
				bulletShootTimer = 0.f;
			}
			break;
		case Projectile::PROJ_SEEKING_MISSLE:
			{
				// TODO: Find pos
				p->Init(ships_.at(0), type, posX, posY, ships_.at(0)->GetW());
				p->SetDamage(5);
				p->SetSpeed(250.f);
				p->SetTarget(FindNearest());
				missileShootTimer = 0.f;
			}
			break;
		}

		RakNet::BitStream bs;
		bs.Write((unsigned char)ID_SHOOT);
		bs.Write(p->GetID());
		bs.Write(p->GetType());
		bs.Write(p->GetX());
		bs.Write(p->GetY());
		bs.Write(p->GetW());
		bs.Write(p->GetVelocityX());
		bs.Write(p->GetVelocityY());
		bs.Write(p->GetSpeed());
		bs.Write(p->GetDamage());
		bs.Write(p->GetOwner()->GetID());
		// Send target if available
		if (p->GetTarget())
		{
			bs.Write(p->GetTarget()->GetID());
		}
		else
		{
			bs.Write(-1);
		}
		rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
}

Enemy * Application::FindNearest()
{
	float shortestLength = -1;
	Enemy* shortestEnemy = nullptr;
	for (vector<Enemy*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
	{
		Enemy* e = *it;
		if (e->GetActive())
		{
			float dist = (Vector2(ships_.at(0)->GetX(), ships_.at(0)->GetY()) - Vector2(e->GetX(), e->GetY())).Length();
			if (shortestLength == -1 || dist < shortestLength)
			{
				shortestLength = dist;
				shortestEnemy = e;
			}
		}
	}
	return shortestEnemy;
}

Ship * Application::FindShipByID(int id)
{
	for (ShipList::iterator it = ships_.begin(); it != ships_.end(); ++it)
	{
		Ship* s = *it;
		if (s->GetID() == id)
		{
			return s;
		}
	}
	return nullptr;
}

bool Application::Init()
{
	std::ifstream inData;	
	std::string serverip;

	inData.open("serverip.txt");

	inData >> serverip;

	srand( RakNet::GetTime() );

	hge_->System_SetState(HGE_FRAMEFUNC, Application::Loop);
	hge_->System_SetState(HGE_WINDOWED, true);
	hge_->System_SetState(HGE_USESOUND, false);
	hge_->System_SetState(HGE_TITLE, "Movement");
	hge_->System_SetState(HGE_LOGFILE, "movement.log");
	hge_->System_SetState(HGE_DONTSUSPEND, true);
	hge_->System_SetState(HGE_SCREENWIDTH, S_SCREEN_WIDTH);
	hge_->System_SetState(HGE_SCREENHEIGHT, S_SCREEN_HEIGHT);

	if(hge_->System_Initiate()) 
	{
		ships_.push_back(new Ship(rand() % 3 + 1, S_SCREEN_WIDTH * 0.5f, S_SCREEN_HEIGHT * 0.5f));
		//ships_.push_back(new Ship(rand() % 3 + 1, rand() % 500 + 100, rand() % 400 + 100));
		ships_.at(0)->SetName("My Ship");
		if (rakpeer_->Startup(1,30,&SocketDescriptor(), 1))
		{
			rakpeer_->SetOccasionalPing(true);
			return rakpeer_->Connect(serverip.c_str(), 1691, 0, 0);
		}
	}
	return false;
}

/**
* Update cycle
*
* Checks for keypresses:
*   - Esc - Quits the game
*   - Left - Rotates ship left
*   - Right - Rotates ship right
*   - Up - Accelerates the ship
*   - Down - Deccelerates the ship
*
* Also calls Update() on all the ships in the universe
*/
bool Application::Update()
{
	if (hge_->Input_GetKeyState(HGEK_ESCAPE))
		return true;

	float timedelta = hge_->Timer_GetDelta();
	if (bulletShootTimer < S_BULLET_SHOOT_INTERVAL)
	{
		bulletShootTimer += timedelta;
	}
	if (missileShootTimer < S_MISSILE_SHOOT_INTERVAL)
	{
		missileShootTimer += timedelta;
	}

	ships_.at(0)->SetAngularVelocity(0.0f);

	if (hge_->Input_GetKeyState(HGEK_LEFT))
	{
		ships_.at(0)->SetAngularVelocity(ships_.at(0)->GetAngularVelocity() - DEFAULT_ANGULAR_VELOCITY);
	}

	if (hge_->Input_GetKeyState(HGEK_RIGHT))
	{
		ships_.at(0)->SetAngularVelocity(ships_.at(0)->GetAngularVelocity() + DEFAULT_ANGULAR_VELOCITY);
	}

	if (hge_->Input_GetKeyState(HGEK_UP))
	{
		ships_.at(0)->Accelerate(DEFAULT_ACCELERATION, timedelta);
	}

	if (hge_->Input_GetKeyState(HGEK_DOWN))
	{
		ships_.at(0)->Accelerate(-DEFAULT_ACCELERATION, timedelta);
	}

	// Lab 13 Task 4 : Add a key to shoot missiles
	/*if (hge_->Input_GetKeyState(HGEK_ENTER))
	{
		if (!keydown_enter)
		{
			CreateMissile(ships_.at(0)->GetX(), ships_.at(0)->GetY(), ships_.at(0)->GetW(), ships_.at(0)->GetID());
			keydown_enter = true;
		}
	}
	else
	{
		if (keydown_enter)
		{
			keydown_enter = false;
		}
	}*/

	if (hge_->Input_GetKeyState(HGEK_ENTER))
	{
		if (missileShootTimer >= S_MISSILE_SHOOT_INTERVAL)
		{
			Shoot(Projectile::PROJ_SEEKING_MISSLE);
		}
	}
	if (hge_->Input_GetKeyState(HGEK_SPACE))
	{
		if (bulletShootTimer >= S_BULLET_SHOOT_INTERVAL)
		{
			Shoot(Projectile::PROJ_BULLET);
		}
	}

	// Update all ships
	for (ShipList::iterator ship = ships_.begin();
	ship != ships_.end(); ship++)
	{
		(*ship)->Update(timedelta);

		//collisions
		/*if ((*ship) == ships_.at(0))
			checkCollisions((*ship));*/
	}

	// Lab 13 Task 5 : Updating the missile
	/*if (mymissile)
	{
		if (mymissile->Update(ships_, timedelta))
		{
			//havecollision
			delete mymissile;
			mymissile = 0;
		}
	}*/

	// Lab 13 Task 13 : Update network missiles
	/*for (MissileList::iterator missile = missiles_.begin(); missile != missiles_.end(); missile++)
	{
		if ((*missile)->Update(ships_, timedelta))
		{
			//havecollision
			delete*missile;
			missiles_.erase(missile);
			break;
		}
	}*/

	// Update enemies
	/*for (vector<Enemy*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
	{
		Enemy* e = *it;
		if (e->GetActive())
		{
			bool reset = e->Update(timedelta);
			if (reset)
			{
				RakNet::BitStream bs;
				bs.Write((unsigned char)ID_DESTROY_ENEMY);
				bs.Write(e->GetID());
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
			}
		}
	}*/

	// Update projectile
	for (vector<Projectile*>::iterator it = projectileList.begin(); it != projectileList.end(); ++it)
	{
		Projectile* p = *it;
		if (/*p->GetOwner() == ships_.at(0) && */p->GetActive()) // Only update your own projectile
		{
			bool reset = p->Update(timedelta);
			RakNet::BitStream bs;
			if (reset)
			{
				p->Reset();
				bs.Write((unsigned char)ID_DESTROY_PROJECTILE);
				bs.Write(p->GetID());
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
			}
			else
			{
				for (vector<Enemy*>::iterator it2 = enemyList.begin(); it2 != enemyList.end(); ++it2)
				{
					Enemy* e = *it2;
					if (e->GetActive())
					{
						bool collision = p->CollideWith(e);
						if (collision)
						{
							int newHP = e->Injure(p->GetDamage());
							if (newHP <= 0)
							{
								RakNet::BitStream bs;
								// Add score to player
								p->GetOwner()->AddScore(100);

								bs.Write((unsigned char)ID_UPDATE_SCORE);
								bs.Write(p->GetOwner()->GetID());
								bs.Write(p->GetOwner()->GetScore());
								rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

								// Destroy enemy
								DestroyEnemy(e);

								// Send to server to destroy enemy
								bs.ResetWritePointer();
								bs.Write((unsigned char)ID_DESTROY_ENEMY);
								bs.Write(e->GetID());
								rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
							}
							else
							{
								// Send to server to injure enemy
								RakNet::BitStream bs;
								bs.ResetWritePointer();
								bs.Write((unsigned char)ID_INJURE_ENEMY);
								bs.Write(e->GetID());
								bs.Write(e->GetHP());
								rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
							}

							// Reset projectile
							p->Reset();
							bs.ResetWritePointer();
							bs.Write((unsigned char)ID_DESTROY_PROJECTILE);
							bs.Write(p->GetID());
							rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
						}
					}
				}
				/*int max = projectileUpdateList.size();
				int count = 0;
				for (vector<Projectile*>::iterator it2 = projectileUpdateList.begin(); it2 != projectileUpdateList.end(); ++it2)
				{
					Projectile* p2 = *it2;
					if (p == p2)
					{
						break;
					}
					else
					{
						++count;
					}
				}
				if (count == max)
				{
					projectileUpdateList.push_back(p);
				}*/
				/*bs.Write((unsigned char)ID_UPDATE_PROJECTILE);
				bs.Write(p->GetID());
				bs.Write(p->GetActive());
				bs.Write(p->GetType());
				bs.Write(p->GetX());
				bs.Write(p->GetY());
				bs.Write(p->GetVelocityX());
				bs.Write(p->GetVelocityY());
				bs.Write(p->GetSpeed());
				bs.Write(p->GetDamage());
				bs.Write(p->GetOwner()->GetID());
				// Send target id if available
				if (p->GetTarget())
				{
					bs.Write(p->GetTarget()->GetID());
				}
				else
				{
					bs.Write(-1);
				}
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);*/
			}
		}
	}

	// Update explosions
	for (vector<Explosion*>::iterator it = explosionList.begin(); it != explosionList.end(); ++it)
	{
		Explosion* e = *it;
		if ( e->isActive())
		{
			e->Update(timedelta);
		}
	}

	// Packet receive
	if (Packet* packet = rakpeer_->Receive())
	{
		RakNet::BitStream bs(packet->data, packet->length, false);
		
		unsigned char msgid = 0;
		RakNetTime timestamp = 0;

		bs.Read(msgid);

		if (msgid == ID_TIMESTAMP)
		{
			bs.Read(timestamp);
			bs.Read(msgid);
		}

		switch(msgid)
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				std::cout << "Connected to Server" << std::endl;
				SendScreenSize();
				InitEnemyList();
				InitExplosionList();
				InitProjectileList();
				InitBackground();
				InitBase();
				InitScore();
			}
			break;

		case ID_NO_FREE_INCOMING_CONNECTIONS:
		case ID_CONNECTION_LOST:
		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "Lost Connection to Server" << std::endl;
			rakpeer_->DeallocatePacket(packet);
			return true;

		case ID_WELCOME:
			{
				unsigned int shipcount, id;
				float x_, y_;
				int type_;
				std::string temp;
				char chartemp[5];

				bs.Read(id);
				ships_.at(0)->setID( id );
				bs.Read(shipcount);

				for (unsigned int i = 0; i < shipcount; ++ i)
				{
					bs.Read(id);
					bs.Read(x_);
					bs.Read(y_);
					bs.Read(type_);
					std::cout << "New Ship pos" << x_ << " " << y_ << std::endl;
					Ship* ship = new Ship(type_, x_, y_ ); 
					temp = "Ship ";
					temp += _itoa(id, chartemp, 10);
					ship->SetName(temp.c_str());
					ship->setID( id );
					ships_.push_back(ship);
				}

				for (vector<Enemy*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
				{
					int id, hp;
					bool active;
					Enemy::ENEMY_TYPE type;
					float x, y, vel_x, vel_y, speed;
					bs.Read(id);
					bs.Read(active);
					bs.Read(type);
					bs.Read(x);
					bs.Read(y);
					bs.Read(vel_x);
					bs.Read(vel_y);
					bs.Read(speed);
					bs.Read(hp);
					Enemy* e = FindEnemyByID(id);
					if (e)
					{
						float w = CalcW(Vector2(vel_x, vel_y));
						/*if (speed != 0)
						{
							w = acosf(vel_x / speed);
						}
						else
						{
							w = 0.f;
						}*/
						e->Init(type, x, y, w, active);
						e->SetVelocityX(vel_x);
						e->SetVelocityY(vel_y);
						e->SetSpeed(speed);
						e->SetHP(hp);
					}
				}
				
				// Base hp
				bs.Read(base_hp);

				SendInitialPosition();
			}
			break;
		case ID_REJECT_PLAYER:
			{
				std::cout << "Rejected player" << std::endl;
				return true;
			}
			break;

		case ID_NEWSHIP:
			{
				unsigned int id;
				bs.Read(id);

				if( id == ships_.at(0)->GetID() )
				{
					// if it is me
					break;
				}
				else
				{
					float x_, y_;
					int type_;
					std::string temp;
					char chartemp[5];

					bs.Read( x_ );
					bs.Read( y_ );
					bs.Read( type_ );
					std::cout << "New Ship pos" << x_ << " " << y_ << std::endl;
					Ship* ship = new Ship(type_, x_, y_);
					temp = "Ship "; 
					temp += _itoa(id, chartemp, 10);
					ship->SetName(temp.c_str());
					ship->setID( id );
					ships_.push_back(ship);
				}

			}
			break;

		case ID_LOSTSHIP:
			{
				unsigned int shipid;
				bs.Read(shipid);
				for (ShipList::iterator itr = ships_.begin(); itr != ships_.end(); ++itr)
				{
					if ((*itr)->GetID() == shipid)
					{
						delete *itr;
						ships_.erase(itr);
						break;
					}
				}
			}
			break;

		case ID_INITIALPOS:
			break;

		case ID_MOVEMENT:
			{
				unsigned int shipid;
				float temp;
				float x,y,w;
				bs.Read(shipid);
				for (ShipList::iterator itr = ships_.begin(); itr != ships_.end(); ++itr)
				{
					if ((*itr)->GetID() == shipid)
					{
						// this portion needs to be changed for it to work
#ifdef INTERPOLATEMOVEMENT
						bs.Read(x);
						bs.Read(y);
						bs.Read(w);

						(*itr)->SetServerLocation( x, y, w ); 

						bs.Read(temp);
						(*itr)->SetServerVelocityX( temp );
						bs.Read(temp);
						(*itr)->SetServerVelocityY( temp );
						bs.Read(temp);
						(*itr)->SetAngularVelocity( temp );

						(*itr)->DoInterpolateUpdate();
#else
						bs.Read(x);
						bs.Read(y);
						bs.Read(w);
						(*itr)->setLocation( x, y, w ); 

						// Lab 7 Task 1 : Read Extrapolation Data velocity x, velocity y & angular velocity
						bs.Read(temp);
						(*itr)->SetVelocityX( temp );
						bs.Read(temp);
						(*itr)->SetVelocityY( temp );
						bs.Read(temp);
						(*itr)->SetAngularVelocity( temp );
#endif

						break;
					}
				}
			}
			break;

		case ID_COLLIDE:
			{
				unsigned int shipid;
				float x, y;
				bs.Read(shipid);
				
				if( shipid == ships_.at(0)->GetID() )
				{
					std::cout << "collided with someone!" << std::endl;
					bs.Read(x);
					bs.Read(y);
					ships_.at(0)->SetX( x );
					ships_.at(0)->SetY( y );
					bs.Read(x);
					bs.Read(y);
					ships_.at(0)->SetVelocityX( x );
					ships_.at(0)->SetVelocityY( y );
#ifdef INTERPOLATEMOVEMENT
					bs.Read(x);
					bs.Read(y);
					ships_.at(0)->SetServerVelocityX( x );
					ships_.at(0)->SetServerVelocityY( y );
#endif	
				}
			}
			break;


		// Lab 13 Task 10 : new cases to handle missile on application side
		case ID_NEWMISSILE:
			{
				float x, y, w;
				int id;
				bs.Read(id);
				bs.Read(x);
				bs.Read(y);
				bs.Read(w);
				missiles_.push_back(new Missile("missile.png", x, y, w, id));
			}
			break;
		case ID_UPDATEMISSILE:
			{
				float x, y, w;
				int id;
				char deleted;
				bs.Read(id);
				bs.Read(deleted);
				for (MissileList::iterator itr = missiles_.begin(); itr !=
					missiles_.end(); ++itr)
				{
					if ((*itr)->GetOwnerID() == id)
					{
						if (deleted == 1)
						{
							delete*itr;
							missiles_.erase(itr);
						}
						else
						{
							bs.Read(x);
							bs.Read(y);
							bs.Read(w);
							(*itr)->UpdateLoc(x, y, w);
							bs.Read(x);
							(*itr)->SetVelocityX(x);
							bs.Read(y);
							(*itr)->SetVelocityY(y);
						}
						break;
					}
				}
			}
			break;
		case ID_NEW_ENEMY:
			{
				int id, hp;
				Enemy::ENEMY_TYPE type;
				float x, y, vel_x, vel_y, speed;
				bs.Read(id);
				bs.Read(type);
				bs.Read(x);
				bs.Read(y);
				bs.Read(vel_x);
				bs.Read(vel_y);
				bs.Read(speed);
				bs.Read(hp);
				Enemy* e = FindEnemyByID(id);
				if (e)
				{
					float w = CalcW(Vector2(vel_x, vel_y));
					e->Init(type, x, y, w);
					e->SetVelocityX(vel_x);
					e->SetVelocityY(vel_y);
					e->SetSpeed(speed);
					e->SetHP(hp);
				}
			}
			break;
		case ID_UPDATE_ENEMY:
			{
				int id;
				bs.Read(id);
				Enemy* e = FindEnemyByID(id);
				if (e)
				{
					float x, y;
					bs.Read(x);
					bs.Read(y);
					e->SetX(x);
					e->SetY(y);
				}
			}
			break;
		case ID_INJURE_ENEMY:
			{
				int id, hp;
				bs.Read(id);
				Enemy* e = FindEnemyByID(id);
				if (e && e->GetActive())
				{
					bs.Read(hp);
					e->SetHP(hp);
				}
			}
			break;
		case ID_DESTROY_ENEMY:
			{
				int id;
				bs.Read(id);
				Enemy* e = FindEnemyByID(id);

				DestroyEnemy(e);
			}
			break;
		case ID_SHOOT:
			{
				int id, damage, owner, target;
				Projectile::PROJECTILE_TYPE type;
				float x, y, w, vel_x, vel_y, speed;
				bs.Read(id);

				Projectile* p = FindProjectileByID(id);
				if (p)
				{
					bs.Read(type);
					bs.Read(x);
					bs.Read(y);
					bs.Read(w);
					bs.Read(vel_x);
					bs.Read(vel_y);
					bs.Read(speed);
					bs.Read(damage);
					bs.Read(owner);
					bs.Read(target);
					
					Ship* sOwner = FindShipByID(owner);
					if (sOwner)
					{
						p->Init(sOwner, type, x, y, w);
						p->SetVelocityX(vel_x);
						p->SetVelocityY(vel_y);
						p->SetSpeed(speed);
						p->SetDamage(damage);
						if (target != -1)
						{
							Enemy* sTarget = FindEnemyByID(target);
							if (sTarget)
							{
								p->SetTarget(sTarget);
							}
						}
					}
				}
			}
			break;
		case ID_UPDATE_PROJECTILE:
			{
				int id, damage, owner, target;
				Projectile::PROJECTILE_TYPE type;
				float x, y, vel_x, vel_y, speed;
				bool active;
				bs.Read(id);

				Projectile* p = FindProjectileByID(id);
				if (p)
				{
					bs.Read(active);
					bs.Read(type);
					bs.Read(x);
					bs.Read(y);
					bs.Read(vel_x);
					bs.Read(vel_y);
					bs.Read(speed);
					bs.Read(damage);
					bs.Read(owner);
					bs.Read(target);

					Ship* sOwner = FindShipByID(owner);
					if (sOwner)
					{
						p->SetOwner(sOwner);
						p->SetType(type);
						p->SetX(x);
						p->SetY(y);
						p->SetActive(active);
						p->SetVelocityX(vel_x);
						p->SetVelocityY(vel_y);
						p->SetSpeed(speed);
						p->SetDamage(damage);
						if (target != -1)
						{
							Enemy* sTarget = FindEnemyByID(target);
							if (sTarget)
							{
								p->SetTarget(sTarget);
							}
						}
					}
				}
			}
			break;
		case ID_DESTROY_PROJECTILE:
			{
				int id;
				bs.Read(id);
				Projectile* p = FindProjectileByID(id);
				if (p && p->GetActive())
				{
					// Reset projectile
					p->Reset();
				}
			}
			break;
		case ID_UPDATE_BASE:
			{
				bs.Read(base_hp);
			}
			break;
		case ID_UPDATE_SCORE:
			{
				int id, score;
				bs.Read(id);
				Ship* s = FindShipByID(id);
				if (s)
				{
					bs.Read(score);
					s->SetScore(score);
				}
			}
			break;

		default:
			std::cout << "Unhandled Message Identifier: " << (int)msgid << std::endl;

		}
		rakpeer_->DeallocatePacket(packet);
	}

	if (base_hp <= 0)
	{
		return true;
	}

	// Send projectile updates
	/*static const int SYNCS_PER_SEC = 24;
	static const float TIME_PER_SYNC = 1 / SYNCS_PER_SEC;
	static float projSendTimer = TIME_PER_SYNC;
	if (projSendTimer < TIME_PER_SYNC)
	{
		projSendTimer += timedelta;
	}
	else
	{
		for (vector<Projectile*>::iterator it = projectileList.begin(); it != projectileList.end(); ++it)
		{
			Projectile* p = *it;
			if (p->GetActive() && p->GetOwner() == ships_.at(0))
			{
				RakNet::BitStream sendProj;
				sendProj.Write((unsigned char)ID_UPDATE_PROJECTILE);
				sendProj.Write(p->GetID());
				sendProj.Write(p->GetActive());
				sendProj.Write(p->GetType());
				sendProj.Write(p->GetX());
				sendProj.Write(p->GetY());
				sendProj.Write(p->GetVelocityX());
				sendProj.Write(p->GetVelocityY());
				sendProj.Write(p->GetSpeed());
				sendProj.Write(p->GetDamage());
				sendProj.Write(p->GetOwner()->GetID());
				// Send target id if available
				if (p->GetTarget())
				{
					sendProj.Write(p->GetTarget()->GetID());
				}
				else
				{
					sendProj.Write(-1);
				}
				rakpeer_->Send(&sendProj, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
				std::cout << p->GetX() << " | " << p->GetY() << std::endl;
			}
		}
		projSendTimer = 0.f;
	}*/

	// Send data
	float timeToSync = 1000 / 24; // Sync 24 times in a second (Millisecond)
	if (RakNet::GetTime() - timer_ > timeToSync)
	{
		timer_ = RakNet::GetTime(); // Store previous time
		RakNet::BitStream bs2;
		unsigned char msgid = ID_MOVEMENT;
		bs2.Write(msgid);

#ifdef INTERPOLATEMOVEMENT
		bs2.Write(ships_.at(0)->GetID());
		bs2.Write(ships_.at(0)->GetServerX());
		bs2.Write(ships_.at(0)->GetServerY());
		bs2.Write(ships_.at(0)->GetServerW());
		bs2.Write(ships_.at(0)->GetServerVelocityX());
		bs2.Write(ships_.at(0)->GetServerVelocityY());
		bs2.Write(ships_.at(0)->GetAngularVelocity());

#else
		bs2.Write(ships_.at(0)->GetID());
		bs2.Write(ships_.at(0)->GetX());
		bs2.Write(ships_.at(0)->GetY());
		bs2.Write(ships_.at(0)->GetW());
		// Lab 7 Task 1 : Add Extrapolation Data velocity x, velocity y & angular velocity
		bs2.Write(ships_.at(0)->GetVelocityX());
		bs2.Write(ships_.at(0)->GetVelocityY());
		bs2.Write(ships_.at(0)->GetAngularVelocity());
#endif

		rakpeer_->Send(&bs2, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);


		// Lab 13 Task 11 : send missile update 
		/*if (mymissile)
		{
			RakNet::BitStream bs3;
			unsigned char msgid2 = ID_UPDATEMISSILE;
			unsigned char deleted = 0;
			bs3.Write(msgid2);
			bs3.Write(mymissile->GetOwnerID());
			bs3.Write(deleted);
			bs3.Write(mymissile->GetX());
			bs3.Write(mymissile->GetY());
			bs3.Write(mymissile->GetW());
			bs3.Write(mymissile->GetVelocityX());
			bs3.Write(mymissile->GetVelocityY());
			rakpeer_->Send(&bs3, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
		}*/

		// Send update projectile
		/*for (int i = 0; i < projectileUpdateList.size(); ++i)
		{
			Projectile* p = projectileList[i];
			if (p && p->GetActive())
			{
				RakNet::BitStream sendProj;
				sendProj.Write((unsigned char)ID_UPDATE_PROJECTILE);
				sendProj.Write(p->GetID());
				sendProj.Write(p->GetActive());
				sendProj.Write(p->GetType());
				sendProj.Write(p->GetX());
				sendProj.Write(p->GetY());
				sendProj.Write(p->GetVelocityX());
				sendProj.Write(p->GetVelocityY());
				sendProj.Write(p->GetSpeed());
				sendProj.Write(p->GetDamage());
				sendProj.Write(p->GetOwner()->GetID());
				// Send target id if available
				if (p->GetTarget())
				{
					sendProj.Write(p->GetTarget()->GetID());
				}
				else
				{
					sendProj.Write(-1);
				}
				rakpeer_->Send(&sendProj, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
			}
		}
		projectileUpdateList.clear();*/
	}

	return false;
}


/**
* Render Cycle
*
* Clear the screen and render all the ships
*/
void Application::Render()
{
	hge_->Gfx_BeginScene();  
	hge_->Gfx_Clear(0);

	// Background
	if (background)
	{
		background->Render(0, 0);
	}

	// Base
	if (base)
	{
		base->Render(S_SCREEN_WIDTH * 0.5f, S_SCREEN_HEIGHT * 0.5f);
	}

	ShipList::iterator itr;
	for (itr = ships_.begin(); itr != ships_.end(); itr++)
	{
		(*itr)->Render();
	}

	// Lab 13 Task 6 : Render the missile
	if (mymissile)
	{
		mymissile->Render();
	}

	// Lab 13 Task 12 : Render network missiles
	MissileList::iterator itr2;
	for (itr2 = missiles_.begin(); itr2 != missiles_.end(); itr2++)
	{
		(*itr2)->Render();
	}

	// Render enemies
	for (vector<Enemy*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
	{
		Enemy* e = *it;
		if (e->GetActive())
		{
			e->Render();
		}
	}

	// Render projectile
	for (vector<Projectile*>::iterator it = projectileList.begin(); it != projectileList.end(); ++it)
	{
		Projectile* p = *it;
		if (p->GetActive())
		{
			p->Render();
		}
	}

	// Render explosion
	for (vector<Explosion*>::iterator it = explosionList.begin(); it != explosionList.end(); ++it)
	{
		Explosion* e = *it;
		if (e->isActive())
		{
			e->Render();
		}
	}

	// Base hp
	if (f_base_hp)
	{
		f_base_hp->Render(S_SCREEN_WIDTH * 0.5f, S_SCREEN_HEIGHT * 0.1f, HGETEXT_CENTER, std::to_string(base_hp).c_str());
	}

	// Score
	if (p1_score)
	{
		string score = "Your score: " + std::to_string(ships_.at(0)->GetScore());
		p1_score->Render(S_SCREEN_WIDTH * 0.25f, S_SCREEN_HEIGHT * 0.1f, HGETEXT_CENTER, score.c_str());
	}
	if (p2_score && ships_.size() > 1)
	{
		string score = "Other score: " + std::to_string(ships_.at(1)->GetScore());
		p1_score->Render(S_SCREEN_WIDTH * 0.75f, S_SCREEN_HEIGHT * 0.1f, HGETEXT_CENTER, score.c_str());
	}

	hge_->Gfx_EndScene();
}


/** 
* Main game loop
*
* Processes user input events
* Supposed to process network events
* Renders the ships
*
* This is a static function that is called by the graphics
* engine every frame, hence the need to loop through the
* global namespace to find itself.
*/
bool Application::Loop()
{
	Global::application->Render();
	return Global::application->Update();
}

/**
* Shuts down the graphics and network system
*/

void Application::Shutdown()
{
	while (enemyList.size() > 0)
	{
		Enemy* e = enemyList.back();
		if (e)
		{
			delete e;
			enemyList.pop_back();
		}
	}

	while (explosionList.size() > 0)
	{
		Explosion* e = explosionList.back();
		if (e)
		{
			delete e;
			explosionList.pop_back();
		}
	}

	while (projectileList.size() > 0)
	{
		Projectile* p = projectileList.back();
		if (p)
		{
			delete p;
			projectileList.pop_back();
		}
	}

	if (background)
	{
		delete background;
		background = nullptr;
	}

	hge_->System_Shutdown();
	hge_->Release();
}

/** 
* Kick starts the everything, called from main.
*/
void Application::Start()
{
	if (Init())
	{
		hge_->System_Start();
		Math::InitRNG();
	}
}

void Application::SendScreenSize()
{
	// Send screen size to server
	RakNet::BitStream BS_ScreenRes;
	BS_ScreenRes.Write((unsigned char)ID_SET_SCREEN_TO_SERVER);
	BS_ScreenRes.Write(S_SCREEN_WIDTH);
	BS_ScreenRes.Write(S_SCREEN_HEIGHT);
	rakpeer_->Send(&BS_ScreenRes, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	std::cout << "Sent screen size" << std::endl;
}

float Application::CalcW(Vector2 dir)
{
	float result = atan2(dir.y, dir.x);
	return result;
}

bool Application::SendInitialPosition()
{
	RakNet::BitStream bs;
	unsigned char msgid = ID_INITIALPOS;
	bs.Write(msgid);
	bs.Write(ships_.at(0)->GetX());
	bs.Write(ships_.at(0)->GetY());
	bs.Write(ships_.at(0)->GetType());

	std::cout << "Sending pos" << ships_.at(0)->GetX() << " " << ships_.at(0)->GetY() << std::endl;

	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

	return true;
}

bool Application::checkCollisions(Ship* ship)
{
	for (std::vector<Ship*>::iterator thisship = ships_.begin();
		thisship != ships_.end(); thisship++)
	{
		if( (*thisship) == ship ) continue;	//skip if it is the same ship

		if( ship->HasCollided( (*thisship) ) )
		{
			if( (*thisship)->CanCollide( RakNet::GetTime() ) &&  ship->CanCollide( RakNet::GetTime() ) )
			{
				std::cout << "collide!" << std::endl;

#ifdef INTERPOLATEMOVEMENT
			if( GetAbsoluteMag( ship->GetVelocityY() ) > GetAbsoluteMag( (*thisship)->GetVelocityY() ) )
			{
				// this transfers vel to thisship
				(*thisship)->SetVelocityY( (*thisship)->GetVelocityY() + ship->GetVelocityY()/3 );
				ship->SetVelocityY( - ship->GetVelocityY() );

				(*thisship)->SetServerVelocityY( (*thisship)->GetServerVelocityY() + ship->GetServerVelocityY()/3 );
				ship->SetServerVelocityY( - ship->GetServerVelocityY() );
			}
			else
			{
				ship->SetVelocityY( ship->GetVelocityY() + (*thisship)->GetVelocityY()/3 ); 
				(*thisship)->SetVelocityY( -(*thisship)->GetVelocityY()/2 );

				ship->SetServerVelocityY( ship->GetServerVelocityY() + (*thisship)->GetServerVelocityY()/3 ); 
				(*thisship)->SetServerVelocityY( -(*thisship)->GetServerVelocityY()/2 );
			}
			
			if( GetAbsoluteMag( ship->GetVelocityX() ) > GetAbsoluteMag( (*thisship)->GetVelocityX() ) )
			{
				// this transfers vel to thisship
				(*thisship)->SetVelocityX( (*thisship)->GetVelocityX() + ship->GetVelocityX()/3 );
				ship->SetVelocityX( - ship->GetVelocityX() );

				(*thisship)->SetServerVelocityX( (*thisship)->GetServerVelocityX() + ship->GetServerVelocityX()/3 );
				ship->SetServerVelocityX( - ship->GetServerVelocityX() );
			}
			else
			{
				// ship transfers vel to asteroid
				ship->SetVelocityX( ship->GetVelocityX() + (*thisship)->GetVelocityX()/3 ); 
				(*thisship)->SetVelocityX( -(*thisship)->GetVelocityX()/2 );

				ship->SetServerVelocityX( ship->GetServerVelocityX() + (*thisship)->GetServerVelocityX()/3 ); 
				(*thisship)->SetServerVelocityX( -(*thisship)->GetServerVelocityX()/2 );
			}

				ship->SetPreviousLocation();
#else
			if( GetAbsoluteMag( ship->GetVelocityY() ) > GetAbsoluteMag( (*thisship)->GetVelocityY() ) )
			{
				// this transfers vel to thisship
				(*thisship)->SetVelocityY( (*thisship)->GetVelocityY() + ship->GetVelocityY()/3 );
				ship->SetVelocityY( - ship->GetVelocityY() );
			}
			else
			{
				ship->SetVelocityY( ship->GetVelocityY() + (*thisship)->GetVelocityY()/3 ); 
				(*thisship)->SetVelocityY( -(*thisship)->GetVelocityY()/2 );
			}
			
			if( GetAbsoluteMag( ship->GetVelocityX() ) > GetAbsoluteMag( (*thisship)->GetVelocityX() ) )
			{
				// this transfers vel to thisship
				(*thisship)->SetVelocityX( (*thisship)->GetVelocityX() + ship->GetVelocityX()/3 );
				ship->SetVelocityX( - ship->GetVelocityX() );
			}
			else
			{
				// ship transfers vel to asteroid
				ship->SetVelocityX( ship->GetVelocityX() + (*thisship)->GetVelocityX()/3 ); 
				(*thisship)->SetVelocityX( -(*thisship)->GetVelocityX()/2 );
			}


//				ship->SetVelocityY( -ship->GetVelocityY() );
//				ship->SetVelocityX( -ship->GetVelocityX() );
			
				ship->SetPreviousLocation();
#endif
				SendCollision( (*thisship) );

				return true;
			}
				
		}

	}
	
	return false;
}

void Application::SendCollision( Ship* ship )
{
	RakNet::BitStream bs;
	unsigned char msgid = ID_COLLIDE;
	bs.Write( msgid );
	bs.Write( ship->GetID() );
	bs.Write( ship->GetX() );
	bs.Write( ship->GetY() );
	bs.Write( ship->GetVelocityX() );
	bs.Write( ship->GetVelocityY() );
#ifdef INTERPOLATEMOVEMENT
	bs.Write( ship->GetServerVelocityX() );
	bs.Write( ship->GetServerVelocityY() );
#endif

	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

}

void Application::CreateMissile(float x, float y, float w, int id)
{
#ifdef NETWORKMISSLE
	// Lab 13 Task 9b : Implement networked version of createmissile
	RakNet::BitStream bs;
	unsigned char msgid;
	unsigned char deleted = 0;
	if (mymissile)
	{
		//sendupdatethroughnetworktodeletethismissile
		deleted = 1;
		msgid = ID_UPDATEMISSILE;
		bs.Write(msgid);
		bs.Write(id);
		bs.Write(deleted);
		bs.Write(x);
		bs.Write(y);
		bs.Write(w);
		rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
			UNASSIGNED_SYSTEM_ADDRESS, true);
		//deleteexistingmissile
		delete mymissile;
		mymissile = 0;
	}
	//addanewmissilebasedonthefollowingparametercoordinates
	mymissile = new Missile("missile.png", x, y, w, id);
	//sendnetworkcommandtoaddnewmissile
	bs.Reset();
	msgid = ID_NEWMISSILE;
	bs.Write(msgid);
	bs.Write(id);
	bs.Write(x);
	bs.Write(y);
	bs.Write(w);
	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
		UNASSIGNED_SYSTEM_ADDRESS, true);

#else
	// Lab 13 Task 3 : Implement local version missile creation
	if (mymissile) // Delete existing missle
	{
		delete mymissile;
		mymissile = NULL;
	}

	// Add missle
	mymissile = new Missile("missile.png", x, y, w, id);
#endif
}
