#pragma once

//Enum for different actions the player can perform
typedef enum
{
	JUMP,
	RUN,
	SHIP_THRUST,
	EXAMINE,
	ATTACK,

	NUM_ACTIONS	//Not an action, an array size
} Action;
