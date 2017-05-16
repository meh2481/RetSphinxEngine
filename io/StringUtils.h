/*
	GameEngine header - StringUtils.h
	Utility functions for parsing strings
	Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "rapidjson/document.h"
#include <string>

//--------------------------------------------------
// Parsing functions
//--------------------------------------------------
namespace StringUtils
{
	//Trim whitespace from beginning & end of string
	std::string trim(const std::string& s);

	//Strip all whitespace from s
	std::string removeWhitespace(const std::string& s);

	//Strip all of "from" from s, leaving "to" in their place
	std::string replaceWith(const std::string& s, char from, char to);

	//Shorthand for replaceWith(s, ',', ' ')
	std::string stripCommas(const std::string& s);

	//Get the file extension (not including the dot) from a filename, or empty string if there is none
	// For example: if filename is "image.png", then getExtension(filename) will be "png"
	std::string getExtension(const std::string& filename);

	std::string normalize(const std::string& input); //Normalizes this string to all uppercase, no punctuation

	std::string stringify(const rapidjson::Document& doc);	//Stringifies the passed-in JSON doc
}

