#pragma once
#include <queue>
#include <cassert>
#include "MutexLock.h"
#include "SDL_mutex.h"

template <class T>
class ThreadsafeQueue
{
	std::queue<T> q;
	SDL_mutex *mutex;

public:
	ThreadsafeQueue();
	~ThreadsafeQueue();


	bool pop(T& s);			//returns true on success, false on failure
	void push(const T& x);
};


template <class T>
ThreadsafeQueue<T>::ThreadsafeQueue()
{
	mutex = SDL_CreateMutex();
	assert(mutex);
}

template <class T>
ThreadsafeQueue<T>::~ThreadsafeQueue()
{
	SDL_DestroyMutex(mutex);
}

template <class T>
bool ThreadsafeQueue<T>::pop(T& s)
{
	MutexLock lock(mutex);
	if(q.empty())
		return false;
	s = q.front();
	q.pop();
	return true;
}

template <class T>
void ThreadsafeQueue<T>::push(const T& x)
{
	MutexLock lock(mutex);
	q.push(x);
}
