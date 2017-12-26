#pragma once

#include <iostream>

typedef enum loglevel
{
    TRACE = 0,
    DBG = 1,
    INFO = 2,
    WARN = 3,
    ERR = 4
} LogLevel;

std::ostream& logg(LogLevel l, const char* file, int line);

void logger_init(const char* filename, LogLevel l);
void logger_quit();

#define LOG(level) logg(level, __FILE__, __LINE__)
