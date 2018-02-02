#include "Logger.h"
#include "StringUtils.h"
#include <fstream>
#include <ctime>

#define BUF_SZ 32

static logstream logfile;
static LogLevel curLogLevel;
static char buffer[BUF_SZ];

static const char* levelToString(LogLevel l)
{
    switch(l)
    {
        case TRACE:
            return "TRACE";
        case DBG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARN:
            return "WARN";
        case ERR:
            return "ERROR";
    }
    return "UNK";
}

static const char* curTime()
{
    time_t t = time(0);
    struct tm* now = localtime(&t);
    strftime(buffer, BUF_SZ, "%D %T", now);
    return buffer;
}

#ifdef _DEBUG
logstream& logg(LogLevel l, const char* file, int line)
#else
logstream& logg(LogLevel l)
#endif
{
    if(l >= curLogLevel)
    {
        logfile._on();
        logfile << std::endl << curTime() << ' ' << levelToString(l);
#ifdef _DEBUG
        logfile << ' ' << StringUtils::getFilename(file) << ':' << line;
#endif
        logfile << " : ";
    }
    else
        logfile._off();
    return logfile;
}

void logger_init(const char* filename, LogLevel l)
{
#ifdef _DEBUG
    //If in debug mode, overwrite existing log file
    #define MODE std::ofstream::out
#else
    //If in release mode, append to existing file
    #define MODE std::ofstream::out | std::ofstream::app
#endif
    curLogLevel = l;
    logfile.coss.open(filename, MODE);
    logfile << curTime() << " Logger init";
}

void logger_quit()
{
    logfile << std::endl << curTime() << " Logger close" << std::endl;
    logfile.coss.close();
}

void logstream::_on()
{
    coss.clear();
#ifdef _DEBUG
    std::cerr.clear();
#endif
}

void logstream::_off()
{
    coss.setstate(std::ios_base::badbit);
#ifdef _DEBUG
    std::cerr.setstate(std::ios_base::badbit);
#endif
}

logstream& logstream::operator<< (std::ostream& (*pfun)(std::ostream&))
{
    pfun(coss);
#ifdef _DEBUG
    pfun(std::cerr);
#endif
    return *this;
}
