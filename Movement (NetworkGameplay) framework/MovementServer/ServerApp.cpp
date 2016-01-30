#include "ServerApp.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "Bitstream.h"
#include "GetTime.h"
#include "../MyMsgIDs.h"
#include <iostream>

int ServerApp::s_screen_width = 800;
int ServerApp::s_screen_height = 600;
const float ServerApp::S_SPAWN_OFFSET = 200.f;

ServerApp::ServerApp() : 
	rakpeer_(RakNetworkFactory::GetRakPeerInterface()),
	newID(0)
{
	rakpeer_->Startup(100, 30, &SocketDescriptor(1691, 0), 1);
	rakpeer_->SetMaximumIncomingConnections(100);
	rakpeer_->SetOccasionalPing(true);
	std::cout << "Server Started" << std::endl;
	prevTime = RakNet::GetTime();

	srand(time(NULL));

	InitEnemyList();
}

ServerApp::~ServerApp()
{
	rakpeer_->Shutdown(100);
	RakNetworkFactory::DestroyRakPeerInterface(rakpeer_);

	// Destroy objects
	while (enemyList.size() > 0)
	{
		ServerEnemy* e = enemyList.back();
		if (e)
		{
			delete e;
			enemyList.pop_back();
		}
	}
}

void ServerApp::Loop()
{
	static bool done = false;
	if (!done)
	{
		SpawnEnemy();
		done = true;
	}

	double currentTime = RakNet::GetTime(); // Current time by raknet
	double dt = currentTime - prevTime; // dt is the time difference between last frame and this frame
	prevTime = currentTime; // After calculating dt, set current time to previous time for next calculation later on

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

		switch (msgid)
		{
		case ID_SET_SCREEN_TO_SERVER: // Set screen position
			{
				std::cout << "Received screen size" << std::endl;
				bs.Read(s_screen_width);
				bs.Read(s_screen_height);
			}
			break;
		case ID_NEW_INCOMING_CONNECTION:
			{
				if (clientList.size() >= 2)
				{
					RejectPlayer(packet->systemAddress);
				}
				else
				{
					SendWelcomePackage(packet->systemAddress);
				}
			}
			break;

		case ID_DISCONNECTION_NOTIFICATION:
		case ID_CONNECTION_LOST:
			SendDisconnectionNotification(packet->systemAddress);
			break;

		case ID_INITIALPOS:
			{
				float x_, y_;
				int type_;
				std::cout << "ProcessInitialPosition" << std::endl;
				bs.Read( x_ );
				bs.Read( y_ );
				bs.Read( type_ );
				ProcessInitialPosition( packet->systemAddress, x_, y_, type_);
			}
			break;

		case ID_MOVEMENT:
			{
				float x, y;
				unsigned int shipid;
				bs.Read(shipid);
				bs.Read(x);
				bs.Read(y);
				UpdatePosition( packet->systemAddress, x, y );

				bs.ResetReadPointer();
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
			}
			break;

		case ID_COLLIDE:
			{
				bs.ResetReadPointer();
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
			}
			break;

		// Lab 13 Task 14 : new cases on server side to handle missiles
		case ID_NEWMISSILE:
			{
				bs.ResetReadPointer();
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
			}
			break;
		case ID_UPDATEMISSILE:
			{
				bs.ResetReadPointer();
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
			}
			break;
		case ID_DESTROY_ENEMY:
			{
				int id;
				bs.Read(id);
				ResetEnemy(id);
				bs.ResetReadPointer();
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
			}
			break;


		default:
			std::cout << "Unhandled Message Identifier: " << (int)msgid << std::endl;
		}

		rakpeer_->DeallocatePacket(packet);
	}
}

void ServerApp::SendWelcomePackage(SystemAddress& addr)
{
	++newID;
	unsigned int shipcount = static_cast<unsigned int>(clients_.size());
	unsigned char msgid = ID_WELCOME;
	
	RakNet::BitStream bs;
	bs.Write(msgid);
	bs.Write(newID);
	bs.Write(shipcount);

	// Send all existing ships
	for (ClientMap::iterator itr = clients_.begin(); itr != clients_.end(); ++itr)
	{
		std::cout << "Ship " << itr->second.id << " pos" << itr->second.x_ << " " << itr->second.y_ << std::endl;
		bs.Write( itr->second.id );
		bs.Write( itr->second.x_ );
		bs.Write( itr->second.y_ );
		bs.Write( itr->second.type_ );
	}

	for (vector<ServerEnemy*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
	{
		ServerEnemy* e = *it;
		bs.Write(e->id);
		bs.Write(e->active);
		bs.Write(e->type_);
		bs.Write(e->x_);
		bs.Write(e->y_);
		bs.Write(e->vel_x);
		bs.Write(e->vel_y);
		bs.Write(e->speed);
		bs.Write(e->hp);
	}

	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED,0, addr, false);

	bs.Reset();

	ServerGameObject newobject(newID);
	newobject.active = true;

	clients_.insert(std::make_pair(addr, newobject));
	clientList.push_back(addr); // Add client to list

	std::cout << "New guy, assigned id " << newID << std::endl;
}

void ServerApp::SendDisconnectionNotification(SystemAddress& addr)
{
	ClientMap::iterator itr = clients_.find(addr);
	if (itr == clients_.end())
		return;

	unsigned char msgid = ID_LOSTSHIP;
	RakNet::BitStream bs;
	bs.Write(msgid);
	bs.Write(itr->second.id);

	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, addr, true);

	std::cout << itr->second.id << " has left the game" << std::endl;

	clients_.erase(itr);

	RemoveClientFromList(addr);
}

void ServerApp::ProcessInitialPosition( SystemAddress& addr, float x_, float y_, int type_)
{
	unsigned char msgid;
	RakNet::BitStream bs;
	ClientMap::iterator itr = clients_.find(addr);
	if (itr == clients_.end())
		return;

	itr->second.x_ = x_;
	itr->second.y_ = y_;
	itr->second.type_ = type_;
	
	std::cout << "Received pos" << itr->second.x_ << " " << itr->second.y_ << std::endl;
	std::cout << "Received type" << itr->second.type_ << std::endl;

	msgid = ID_NEWSHIP;
	bs.Write(msgid);
	bs.Write(itr->second.id);
	bs.Write(itr->second.x_);
	bs.Write(itr->second.y_);
	bs.Write(itr->second.type_);

	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, addr, true);
}

void ServerApp::UpdatePosition( SystemAddress& addr, float x_, float y_ )
{
	ClientMap::iterator itr = clients_.find(addr);
	if (itr == clients_.end())
		return;

	itr->second.x_ = x_;
	itr->second.y_ = y_;
}

void ServerApp::RejectPlayer(SystemAddress& addr)
{
	std::cout << "Rejected player" << std::endl;
	RakNet::BitStream bs;
	bs.Write((unsigned char)ID_REJECT_PLAYER);

	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, addr, false);
}

bool ServerApp::RemoveClientFromList(SystemAddress& addr)
{
	for (vector<SystemAddress>::iterator addrItr = clientList.begin(); addrItr != clientList.end(); ++addrItr)
	{
		SystemAddress sysAddr = *addrItr;
		if (sysAddr == addr)
		{
			clientList.erase(addrItr);
			return true;
		}
	}
	return false;
}

void ServerApp::InitEnemyList()
{
	for (int i = 0; i < 100; ++i)
	{
		ServerEnemy* e = new ServerEnemy(ServerEnemy::E_EASY, i);
		enemyList.push_back(e);
	}
}

void ServerApp::SpawnEnemy()
{
	ServerEnemy::ENEMY_TYPE type = (ServerEnemy::ENEMY_TYPE)(rand() % ServerEnemy::E_NUM_ENEMY);
	ServerEnemy* e = FetchEnemy();
	if (e)
	{
		// Universal settings
		e->active = true;
		bool left_right = rand() % 2;
		e->x_ = rand() % (int)S_SPAWN_OFFSET;
		if (left_right) // Left
		{
			e->x_ += -S_SPAWN_OFFSET;
		}
		else // Right
		{
			e->x_ += s_screen_width;
		}

		bool up_down = rand() % 2;
		e->y_ = rand() % (int)S_SPAWN_OFFSET;
		if (up_down) // Up
		{
			e->y_ += -S_SPAWN_OFFSET;
		}
		else // Down
		{
			e->y_ += s_screen_height;
		}
		
		Vector2 dir = (Vector2(s_screen_width, s_screen_height) - Vector2(e->x_, e->y_)).Normalized();

		switch (type)
		{
		case ServerEnemy::E_EASY: // Spawn easy enemy
			{
				e->hp = 1;
				e->speed = 100.f;
				e->type_ = ServerEnemy::E_EASY;
			}
			break;
		case ServerEnemy::E_NORMAL: // Spawn normal enemy
			{
				e->hp = 2;
				e->speed = 150.f;
				e->type_ = ServerEnemy::E_NORMAL;
			}
			break;
		case ServerEnemy::E_HARD: // Spawn hard enemy
			{
				e->hp = 3;
				e->speed = 200.f;
				e->type_ = ServerEnemy::E_HARD;
			}
			break;
		}

		// Universal settings
		e->vel_x = dir.x * e->speed;
		e->vel_y = dir.y * e->speed;

		RakNet::BitStream bs;
		bs.Write((unsigned char)ID_NEW_ENEMY);
		bs.Write(e->id);
		bs.Write(e->type_);
		bs.Write(e->x_);
		bs.Write(e->y_);
		bs.Write(e->vel_x);
		bs.Write(e->vel_y);
		bs.Write(e->speed);
		bs.Write(e->hp);

		rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
}

void ServerApp::UpdateEnemy(double dt)
{
	for (vector<ServerEnemy*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
	{
		ServerEnemy* e = *it;
		if (e->active)
		{
			// Update active enemy
			e->x_ += e->vel_x * dt;
			e->y_ += e->vel_y * dt;

			// Send data to everyone
			RakNet::BitStream bs;
			bs.Write((unsigned char)ID_UPDATE_ENEMY);
			bs.Write(e->id);
			bs.Write(e->x_);
			bs.Write(e->y_);

			rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
		}
	}
}

void ServerApp::ResetEnemy(unsigned int id)
{
	ServerEnemy* e = FindEnemyByID(id);
	if (e)
	{
		e->Reset();
	}
}

ServerEnemy* ServerApp::FetchEnemy()
{
	for (vector<ServerEnemy*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
	{
		ServerEnemy* e = *it;
		if (!e->active)
		{
			return e;
		}
	}
	return nullptr;
}

ServerEnemy * ServerApp::FindEnemyByID(unsigned int id)
{
	for (vector<ServerEnemy*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
	{
		ServerEnemy* e = *it;
		if (e->id == id)
		{
			return e;
		}
	}
	return nullptr;
}
