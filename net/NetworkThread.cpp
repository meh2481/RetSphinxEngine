#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <cassert>
#include "NetworkThread.h"
#include "ThreadsafeQueue.h"
#include "MutexLock.h"
#include "SDL_thread.h"
#include "SDL_timer.h"
#include "easylogging++.h"
#include "minihttp.h"

// Overloaded socket class that handles incoming data.
// This one just prints to standard output.
class HttpDumpSocket : public minihttp::HttpSocket
{
public:
	virtual ~HttpDumpSocket() {}

protected:
	virtual void _OnRecv(void *buf, unsigned int size)
	{
#ifdef _DEBUG
		if(size)
		{
			printf("===START==[Status:%d, Size:%d]======\n", GetStatusCode(), size);
			fwrite(buf, 1, size, stdout);
			puts("\n===END====================");
		}
#endif
	}
};

namespace NetworkThread
{
	//Outgoing queue of network messages to send
	ThreadsafeQueue<NetworkMessage> outgoing;

	//Thread to spin off
	SDL_Thread *thread = NULL;

	//These two go together
	SDL_mutex *stopMutex = NULL;	//Lock if you want to get/set stopFlag
	bool stopFlag;					//true if networking thread should quit

	void sendPost(const NetworkMessage& msg)
	{
		//Init socket
		HttpDumpSocket *ht = new HttpDumpSocket;
		ht->SetKeepAlive(3);
		ht->SetBufsizeIn(64 * 1024);
		ht->SetAlwaysHandle(true);
		ht->SetUserAgent("minihttp");

		//Init a POST message with our JSON data
		minihttp::POST post;
		post.setJsonData(msg.data.c_str());
		ht->Download(msg.url, NULL, NULL, &post);

		minihttp::SocketSet ss;
		ss.add(ht, true);		//Set ss to delete ht when done (so no memory leak here)

		uint32_t startTicks = SDL_GetTicks();
		while(ss.size() && SDL_GetTicks() < startTicks + 1000)	//Just spin here (for a maximum of 1 second)
			ss.update();
	}

	static int NetworkingThread(void *ptr)
	{
		LOG(INFO) << "Starting networking thread";

		//Init networking
		minihttp::InitNetwork();
		atexit(minihttp::StopNetwork);
				
		//Start main loop
		bool shouldStop = false;
		while(!shouldStop)
		{
			//See if we should stop first
			{
				MutexLock lock(stopMutex);
				if(stopFlag)
					shouldStop = true;
			}

			//Poll for network messages
			NetworkMessage msg;
			if(outgoing.pop(msg))
				sendPost(msg);

			//Wait 10 ms so we don't hog all this CPU, since net is low-priority
			SDL_Delay(10);
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
		if(thread == NULL)
			return false;

		//Stop mutex
		{
			MutexLock lock(stopMutex);
			stopFlag = true;
		}
		
		//Wait for thread to finish
		int threadReturnValue;
		SDL_WaitThread(thread, &threadReturnValue);

		//Destroy mutex
		SDL_DestroyMutex(stopMutex);

		//Done
		return (threadReturnValue == 0);
	}

	bool send(NetworkMessage message)
	{
		assert(thread != NULL);
		outgoing.push(message);
		return true;
	}
}