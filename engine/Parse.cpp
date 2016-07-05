/*
 GameEngine source - Parse.cpp
 Copyright (c) 2014 Mark Hutcheson
*/
#include "Parse.h"

string stripCommas(string s)
{
	//Replace all ',' characters with ' '
	for(;;)
	{
		size_t iPos = s.find(',');
		if(iPos == s.npos)
			break;  //Done

		s.replace(iPos, 1, " ");
	}
	return s;
}
