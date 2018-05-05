#include "Logger.h"
#include "StringUtils.h"
#include <fstream>
#include <ctime>
#include <SDL_mutex.h>
#include <iostream>

#define BUF_SZ 32

static char buffer[BUF_SZ];
static std::ofstream logfile;
static SDL_mutex *logMutex = NULL;

static const char* levelToString(SDL_LogPriority l)
{
    switch(l)
    {
    case SDL_LOG_PRIORITY_VERBOSE:
        return "TRACE";
    case SDL_LOG_PRIORITY_DEBUG:
        return "DEBUG";
    case SDL_LOG_PRIORITY_INFO:
        return "INFO";
    case SDL_LOG_PRIORITY_WARN:
        return "WARN";
    case SDL_LOG_PRIORITY_ERROR:
        return "ERROR";
    }
    return "UNK";
}

static void logWithLock(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    SDL_LockMutex(logMutex);

    logfile << levelToString(priority) << ": " << message << std::endl;
#ifdef _DEBUG
    std::cout << levelToString(priority) << ": " << message << std::endl;
#endif

    SDL_UnlockMutex(logMutex);
}

static const char* curTime()
{
    time_t t = time(0);
    struct tm* now = localtime(&t);
    strftime(buffer, BUF_SZ, "%D %T", now);
    return buffer;
}

void logger_init(const char* filename, SDL_LogPriority l)
{
#ifdef _DEBUG
    //If in debug mode, overwrite existing log file
    #define MODE std::ofstream::out
#else
    //If in release mode, append to existing file
    #define MODE std::ofstream::out | std::ofstream::app
#endif
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, l);
    logfile.open(filename, MODE);
    logMutex = SDL_CreateMutex();
    SDL_LogSetOutputFunction(logWithLock, NULL);
    SDL_Log("%s Logger init", curTime());
}

void logger_quit()
{
    SDL_Log("%s Logger close", curTime());
    logfile.close();
}
