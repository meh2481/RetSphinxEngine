/*
 GameEngine source - StringUtils.cpp
 Copyright (c) 2014 Mark Hutcheson
*/
#include "StringUtils.h"
#include "rapidjson/prettywriter.h"
#include <algorithm>

namespace StringUtils
{
	const std::string delimiters = " \n\r\t";
	std::string trim(std::string s)
	{
		s.erase(s.find_last_not_of(delimiters) + 1);
		return s.erase(0, s.find_first_not_of(delimiters));
	}

	std::string removeWhitespace(std::string s)
	{
		for(size_t i = 0; i < s.length(); i++)
		{
			if(s.at(i) == ' ' || 
				s.at(i) == '\n' || 
				s.at(i) == '\t' ||
				s.at(i) == '\r')
			{
				s.erase(i, 1);
				i--;
			}
		}
		return s;
	}

	std::string stripCommas(std::string s)
	{
		//Replace all ',' characters with ' '
		return replaceWith(s, ',', ' ');
	}

	std::string replaceWith(std::string s, char from, char to)
	{
		//Replace all from characters with to
		while(true)
		{
			size_t iPos = s.find(from);
			if(iPos == s.npos)
				break;  //Done

			s.replace(iPos, 1, 1, to);
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

	std::string normalize(std::string input)
	{
		input = replaceWith(input, ' ', '_');	//Replace spaces with underscores
		transform(input.begin(), input.end(), input.begin(), ::toupper);	//Convert to uppercase
		input.erase(remove_if(input.begin(), input.end(), [](char c) { return (!isalnum(c) && c != '_'); }), input.end()); //Remove non alphanumeric chars
		return input;
	}

	std::string stringify(const rapidjson::Document& doc)
	{
		rapidjson::StringBuffer sb;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
		doc.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
		return sb.GetString();
	}

}
