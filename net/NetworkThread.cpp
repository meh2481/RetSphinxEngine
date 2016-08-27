#include "NetworkThread.h"
#include "ThreadsafeQueue.h"
#include "SDL_thread.h"
#include "SDL_timer.h"
#include "easylogging++.h"

namespace NetworkThread
{
	//Outgoing queue of network messages to send
	ThreadsafeQueue<NetworkMessage> outgoing;

	//Thread to spin off
	SDL_Thread *thread;

	//These two go together
	SDL_mutex *stopMutex;	//Lock if you want to get/set stopFlag
	bool stopFlag;			//true if networking thread should quit

	static int NetworkingThread(void *ptr)
	{
		LOG(INFO) << "Starting networking thread";
		
		bool shouldStop = false;
		while(!shouldStop)
		{
			assert(!SDL_LockMutex(stopMutex));
			if(stopFlag)
				shouldStop = true;
			assert(!SDL_UnlockMutex(stopMutex));
			SDL_Delay(30);
		}

		LOG(INFO) << "Networking thread finished";
		return 0;
	}

	bool start()
	{
		stopFlag = false;
		stopMutex = SDL_CreateMutex();
		assert(stopMutex);
		thread = SDL_CreateThread(NetworkingThread, "NetworkingThread", (void *)NULL);
		return (thread != NULL);
	}

	bool stop()
	{
		//Stop mutex
		assert(!SDL_LockMutex(stopMutex));
		stopFlag = true;
		assert(!SDL_UnlockMutex(stopMutex));

		//Wait for thread to finish
		int threadReturnValue;
		SDL_WaitThread(thread, &threadReturnValue);
		return (threadReturnValue == 0);
	}

	bool send(NetworkMessage message)
	{
		outgoing.push(message);
		return true;
	}
}