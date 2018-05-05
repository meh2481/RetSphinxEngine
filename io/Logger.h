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

//TODO Release
void _logHelper(SDL_LogPriority l, const char* file, int line, const char* fmt, ...);
#define LOG_trace(fmt, ...) _logHelper(SDL_LOG_PRIORITY_VERBOSE, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_dbg(fmt, ...) _logHelper(SDL_LOG_PRIORITY_DEBUG, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_info(fmt, ...) _logHelper(SDL_LOG_PRIORITY_INFO, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_warn(fmt, ...) _logHelper(SDL_LOG_PRIORITY_WARN, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_err(fmt, ...) _logHelper(SDL_LOG_PRIORITY_ERROR, __FILE__, __LINE__, fmt, __VA_ARGS__)

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



