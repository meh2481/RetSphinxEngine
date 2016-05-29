#pragma once

#include <iosfwd>

extern std::ofstream errlog;	//Defined in Engine.cpp

//TODO Why this no worky?
//#ifdef DEBUG
//#define errlog errlog << __FILE__ << '(' << __LINE__ << "): "
//#else
//#define errlog errlog
//#endif
