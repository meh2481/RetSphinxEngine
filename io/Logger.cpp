#include "Logger.h"
#include "StringUtils.h"
#include <fstream>
#include <ctime>

#define NULL_OUT std::cerr
#define BUF_SZ 32

static std::ofstream logfile;
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

std::ostream& logg(LogLevel l, const char* file, int line)
{
    if(l >= curLogLevel)
    {
        logfile << std::endl << curTime() << ' ' << levelToString(l) << ' ' << StringUtils::getFilename(file) << ':' << line << " : ";
        return logfile;
    }

    //cerr is used as null output (badbit-set on logger init).
    return NULL_OUT;
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
    logfile.open(filename, MODE);
    logfile << curTime() << " Logger init";

    //Set cerr to not output anything, so we can use it as a null out. Note that writing to cerr is disabled this way, but that would be a stupid idea anyway
    NULL_OUT.setstate(std::ios_base::badbit);
}

void logger_quit()
{
    logfile << std::endl << curTime() << " Logger close" << std::endl;
    logfile.close();
}
