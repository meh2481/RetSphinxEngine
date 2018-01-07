#pragma once

#include "Color.h"
#include <map>
#include <stdint.h>


class Gradient 
{
protected:
    
    
    std::map<float, Color> m_colorMap;


public:    
    Gradient(const std::string& sXMLFilename);
    Gradient();
    ~Gradient() {};
    
    void insert(float fVal, int32_t r, int32_t g, int32_t b, int32_t a);
    Color getVal(float fVal);
    bool load(const std::string& sXMLFilename);
    
};
