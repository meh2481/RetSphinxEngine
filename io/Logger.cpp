#include "Logger.h"
#include "StringUtils.h"
#include <fstream>

static std::ofstream logfile;
static LogLevel curLogLevel;

static const char* levelToString(LogLevel l)
{
    switch(l)
    {
        case TRACE:
            return "TRACE";
        case DBG:
            return "DBG";
        case INFO:
            return "INFO";
        case WARN:
            return "WARN";
        case ERR:
            return "ERR";
    }
    return "UNK";
}

static std::string curTime()
{
    return "time";
}

static std::string curDate()
{
    return "date";
}

std::ostream& logg(LogLevel l, const char* file, int line)
{
    if(l >= curLogLevel)
    {
        logfile << std::endl << curTime() << ' ' << levelToString(l) << ' ' << StringUtils::getFilename(file) << ':' << line << " : ";
        return logfile;
    }
    //cerr is used as null output
    return std::cerr;
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
    logfile << curDate() << " " << curTime() << " Logger init";

    //Set cerr to not output anything, so we can use as null out
    std::cerr.setstate(std::ios_base::badbit);
}

void logger_quit()
{
    logfile << std::endl << curDate() << " " << curTime() << " Logger close" << std::endl;
    logfile.close();
}
