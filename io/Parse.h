/*
	GameEngine header - Parse.h
	Utility functions for parsing things
	Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include <string>

//--------------------------------------------------
// Parsing functions
//--------------------------------------------------
namespace Parse
{
	std::string stripCommas(std::string s);	   //Strip all the commas from s, leaving spaces in their place
	std::string getExtension(std::string filename);	//Get the file extension from a filename, or empty string if there is none
}

