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
    std::string trim(const std::string& sTrim)
    {
        std::string s = sTrim;
        s.erase(s.find_last_not_of(delimiters) + 1);
        return s.erase(0, s.find_first_not_of(delimiters));
    }

    std::string removeWhitespace(const std::string& sTrim)
    {
        std::string s = sTrim;
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

    std::string stripCommas(const std::string& s)
    {
        //Replace all ',' characters with ' '
        return replaceWith(s, ',', ' ');
    }

    std::string replaceWith(const std::string& sRep, char from, char to)
    {
        std::string s = sRep;
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

    std::string getExtension(const std::string& filename)
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

        return filename.substr(idx + 1);    //Since substr() returns empty string if idx == len
    }

    std::string getFilename(const std::string& path)
    {
        size_t len = path.length();
        size_t idx = 0;
        for(size_t i = len-1; i > 0; i--)
        {
            if(path.at(i) == '\\' || path.at(i) == '/')
            {
                idx = i;
                break;
            }
        }

        return path.substr(idx + 1);    //Since substr() returns empty string if idx == len
    }

    std::string normalize(const std::string& s)
    {
        std::string input = s;
        input = replaceWith(input, ' ', '_');    //Replace spaces with underscores
        transform(input.begin(), input.end(), input.begin(), ::toupper);    //Convert to uppercase
        std::string output = "";
        for(size_t i = 0; i < input.length(); i++)
        {
            char c = input.at(i);
            if(isalnum(c) || c == '_')
                output.append(&c, 1);
        }
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
