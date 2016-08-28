#include "MutexLock.h"
#include "MutexLock.h"
#include <cassert>

MutexLock::MutexLock(SDL_mutex * m)
{
	mutex = m;
	assert(!SDL_LockMutex(m));
}

MutexLock::~MutexLock()
{
	assert(!SDL_UnlockMutex(mutex));
}
