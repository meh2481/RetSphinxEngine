#pragma once
#include "SDL_mutex.h"

class MutexLock
{
	SDL_mutex *mutex;

	MutexLock() {};
public:
	MutexLock(SDL_mutex *m);
	~MutexLock();
};