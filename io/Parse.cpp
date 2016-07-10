/*
 GameEngine source - Parse.cpp
 Copyright (c) 2014 Mark Hutcheson
*/
#include "Parse.h"

namespace Parse
{

	std::string stripCommas(std::string s)
	{
		//Replace all ',' characters with ' '
		while(true)
		{
			size_t iPos = s.find(',');
			if(iPos == s.npos)
				break;  //Done

			s.replace(iPos, 1, " ");
		}
		return s;
	}

	std::string getExtension(std::string filename)
	{
		size_t len = filename.length();
		size_t idx = len - 1;
		for(size_t i = 0; i < len; i++)
		{
			if(filename.at(i) == '.')
				idx = i;

			else if(filename.at(i) == '/' || filename.at(i) == '\\')
				idx = len - 1;
		}

		return filename.substr(idx+1);	//Since substr() returns empty string if idx == len
	}

}
