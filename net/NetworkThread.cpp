#include "NetworkThread.h"
#include "ThreadsafeQueue.h"
#include "SDL_thread.h"
#include "SDL_timer.h"
#include "easylogging++.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "minihttp.h"

// Overloaded socket class that handles incoming data.
// This one just prints to standard output.
class HttpDumpSocket : public minihttp::HttpSocket
{
public:
	virtual ~HttpDumpSocket() {}

protected:
	virtual void _OnClose()
	{
		puts("_OnClose()");
		minihttp::HttpSocket::_OnClose();
	}
	virtual void _OnOpen()
	{
		puts("_OnOpen()");
		char buf[1024] = { 0 };
		minihttp::SSLResult sr = verifySSL(buf, sizeof(buf));
		printf("SSL status flags (0 is good): 0x%x. Info: %s\n", sr, buf);
		minihttp::HttpSocket::_OnOpen();
	}

	virtual void _OnRequestDone()
	{
		printf("_OnRequestDone(): %s\n", GetCurrentRequest().resource.c_str());
		// Do *NOT* call close() in here!
	}

	virtual void _OnRecv(void *buf, unsigned int size)
	{
		if(!size)
			return;
		printf("===START==[Status:%d, Size:%d]======\n", GetStatusCode(), size);
		fwrite(buf, 1, size, stdout);
		puts("\n===END====================");
	}
};

namespace NetworkThread
{
	//Outgoing queue of network messages to send
	ThreadsafeQueue<NetworkMessage> outgoing;

	//Thread to spin off
	SDL_Thread *thread;

	//These two go together
	SDL_mutex *stopMutex;	//Lock if you want to get/set stopFlag
	bool stopFlag;			//true if networking thread should quit

	void sendPost(const NetworkMessage& msg)
	{
		LOG(TRACE) << "Sending " << msg.data << " to " << msg.url;
		//Init socket
		HttpDumpSocket *ht = new HttpDumpSocket;
		ht->SetKeepAlive(3);
		ht->SetBufsizeIn(64 * 1024);
		ht->SetAlwaysHandle(true);
		ht->SetUserAgent("minihttp");

		//Test thing I guess?
		minihttp::POST post;
		post.setJsonData(msg.data.c_str());
		ht->Download(msg.url, NULL, NULL, &post);

		minihttp::SocketSet ss;
		ss.add(ht, false);

		//TODO: Figure out why this hangs here for so long...
		uint32_t startTicks = SDL_GetTicks();
		while(ss.size() && SDL_GetTicks() < startTicks + 1000 * 10)	//Just spin here (for a maximum of 10 seconds)
			ss.update();
		LOG(TRACE) << "\nDone sending to " << msg.url << "\n";
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
			assert(!SDL_LockMutex(stopMutex));
			if(stopFlag)
				shouldStop = true;
			assert(!SDL_UnlockMutex(stopMutex));

			//Poll for network changes
			NetworkMessage msg;
			if(outgoing.pop(msg))
				sendPost(msg);
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

		//Destroy mutex
		SDL_DestroyMutex(stopMutex);

		//Done
		return (threadReturnValue == 0);
	}

	bool send(NetworkMessage message)
	{
		outgoing.push(message);
		return true;
	}
}