#pragma once

#include <SDL_log.h>

typedef enum loglevel
{
    TRACE = 0,
    DBG = 1,
    INFO = 2,
    WARN = 3,
    ERR = 4
 } LogLevel;

void logger_init(const char* filename, SDL_LogPriority l);
void logger_quit();

//TODO Logging levels
#define LOG_trace SDL_Log
#define LOG_dbg SDL_Log
#define LOG_info SDL_Log
#define LOG_warn SDL_Log
#define LOG_err SDL_Log

//#ifdef _DEBUG
//#define LOG(level) logg(level, __FILE__, __LINE__)
//#else
//#define LOG(level) logg(level)
//#endif

//#ifdef _DEBUG
//logstream& logg(LogLevel l, const char* file, int line);
//#else
//logstream& logg(LogLevel l);
//#endif



