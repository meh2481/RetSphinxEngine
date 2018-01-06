#pragma once

#include <iostream>
#include <fstream>

typedef enum loglevel
{
    TRACE = 0,
    DBG = 1,
    INFO = 2,
    WARN = 3,
    ERR = 4
} LogLevel;


class logstream
{
public:
    std::ofstream coss;
    logstream& operator<< (std::ostream& (*pfun)(std::ostream&));
    void _on();
    void _off();
};

#ifdef _DEBUG
logstream& logg(LogLevel l, const char* file, int line);
#else
logstream& logg(LogLevel l);
#endif

template<typename T>
inline logstream& operator<<(logstream & st, const T & val)
{
    st.coss << val;
#ifdef _DEBUG
    std::cout << val;
#endif
    return st;
}

void logger_init(const char* filename, LogLevel l);
void logger_quit();

#ifdef _DEBUG
#define LOG(level) logg(level, __FILE__, __LINE__)
#else
#define LOG(level) logg(level)
#endif
