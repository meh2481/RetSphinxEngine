#pragma once

//Enum for different actions the player can perform
typedef enum
{
	JUMP,
	RUN,
	SHIP_THRUST,
	EXAMINE,
	ATTACK,
	NOTE_UP,
	NOTE_DOWN,
	NOTE_LEFT,
	NOTE_RIGHT,

	NUM_ACTIONS	//Not an action, an array size
} Action;
