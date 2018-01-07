#pragma once
#include <string>

namespace NetworkThread
{
    typedef struct
    {
        std::string data;
        std::string url;
    } NetworkMessage;

    bool start();    //Start the networking thread
    bool stop();    //Stop the networking thread
    bool send(NetworkMessage message);    //Send the specified data to the specified URL
}