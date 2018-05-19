#include "Logger.h"
#include "StringUtils.h"
#include <fstream>
#include <ctime>
#include <SDL_mutex.h>
#include <iostream>

#define TIME_BUF_SZ 32

static char timeBuf[TIME_BUF_SZ];
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

    logfile << message << std::endl;
#ifdef _DEBUG
    logfile.flush();
    std::cout << message << std::endl;
    std::cout.flush();
#endif

}

static const char* curTime()
{
    time_t t = time(0);
    struct tm* now = localtime(&t);
    strftime(timeBuf, TIME_BUF_SZ, "%D %T", now);
    return timeBuf;
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
    SDL_DestroyMutex(logMutex);
}

#define LOG_BUF_SZ 4096
static char logBuf[LOG_BUF_SZ];
#ifdef _DEBUG
void _logHelper(SDL_LogPriority l, const char * file, int line, const char * fmt, ...)
#else
void _logHelper(SDL_LogPriority l, const char * fmt, ...)
#endif
{
    SDL_LockMutex(logMutex);
    va_list args;
    va_start(args, fmt);
    vsnprintf(logBuf, LOG_BUF_SZ, fmt, args);
    va_end(args);
#ifdef _DEBUG
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, l, "%s %s %s:%d : %s", curTime(), levelToString(l), StringUtils::getFilename(file).c_str(), line, logBuf);
#else
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, l, "%s %s : %s", curTime(), levelToString(l), logBuf);
#endif
    SDL_UnlockMutex(logMutex);
}
