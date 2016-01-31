#ifndef MYSMSGIDS_H_
#define MYSMSGIDS_H_

#include "MessageIdentifiers.h"

enum MyMsgIDs
{
	ID_WELCOME = ID_USER_PACKET_ENUM,
	ID_NEWSHIP,
	ID_LOSTSHIP,
	ID_INITIALPOS,
	ID_MOVEMENT,
	ID_COLLIDE,
	ID_SET_SCREEN_TO_SERVER, // Send screen size to server
	ID_REJECT_PLAYER, // Reject anyone else when there are 2 players in the server
	// Lab 13 Task 7 : Add new messages
	ID_NEWMISSILE,
	ID_UPDATEMISSILE,

	// Enemy
	ID_NEW_ENEMY,
	ID_UPDATE_ENEMY,
	ID_DESTROY_ENEMY,

	// Projectile
	ID_SHOOT,
	ID_UPDATE_PROJECTILE,
	ID_DESTROY_PROJECTILE,

	// Explosion
	ID_NEW_EXPLOSION,
	ID_END_EXPLOSION,
};

#endif