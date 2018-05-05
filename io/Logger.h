#pragma once

#include <SDL_log.h>

void logger_init(const char* filename, SDL_LogPriority l);
void logger_quit();

#ifdef _DEBUG
void _logHelper(SDL_LogPriority l, const char* file, int line, const char* fmt, ...);
#define LOG_trace(fmt, ...) _logHelper(SDL_LOG_PRIORITY_VERBOSE, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_dbg(fmt, ...) _logHelper(SDL_LOG_PRIORITY_DEBUG, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_info(fmt, ...) _logHelper(SDL_LOG_PRIORITY_INFO, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_warn(fmt, ...) _logHelper(SDL_LOG_PRIORITY_WARN, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_err(fmt, ...) _logHelper(SDL_LOG_PRIORITY_ERROR, __FILE__, __LINE__, fmt, __VA_ARGS__)
#else
void _logHelper(SDL_LogPriority l, const char* fmt, ...);
#define LOG_trace(fmt, ...) _logHelper(SDL_LOG_PRIORITY_VERBOSE, fmt, __VA_ARGS__)
#define LOG_dbg(fmt, ...) _logHelper(SDL_LOG_PRIORITY_DEBUG, fmt, __VA_ARGS__)
#define LOG_info(fmt, ...) _logHelper(SDL_LOG_PRIORITY_INFO, fmt, __VA_ARGS__)
#define LOG_warn(fmt, ...) _logHelper(SDL_LOG_PRIORITY_WARN, fmt, __VA_ARGS__)
#define LOG_err(fmt, ...) _logHelper(SDL_LOG_PRIORITY_ERROR, fmt, __VA_ARGS__)
#endif



