#include "Gradient.h"
#include "tinyxml2.h"
#include "Logger.h"

Gradient::Gradient(const std::string& sXMLFilename)
{
    load(sXMLFilename);
}

Gradient::Gradient()
{
}

void Gradient::insert(float fVal, int32_t r, int32_t g, int32_t b, int32_t a)
{
    Color c;
    c.from256(r, g, b, a);
    m_colorMap[fVal] = c;
}

Color Gradient::getVal(float fVal)
{
    for(std::map<float, Color>::iterator i = m_colorMap.begin(); i != m_colorMap.end(); i++)
    {
        if(fVal <= i->first)
            return i->second;    //Before beginning, or exactly on one point - return this color

        //Get next color point
        std::map<float, Color>::iterator next = i;
        next++;
        if(next == m_colorMap.end())
            return i->second;    //Past end - return last color in list

        if(fVal >= i->first && fVal <= next->first)    //Between these two points
        {
            float rDiff = next->second.r - i->second.r;
            float gDiff = next->second.g - i->second.g;
            float bDiff = next->second.b - i->second.b;
            float aDiff = next->second.a - i->second.a;
            float diff = next->first - i->first;
            float diffFac = (fVal - i->first) / diff;

            Color c = i->second;
            c.r += rDiff * diffFac;
            c.g += gDiff * diffFac;
            c.b += bDiff * diffFac;
            c.a += aDiff * diffFac;
            return c;
        }
    }

    Color c;
    return c;    //No points or something; return rgba(1,1,1,1)
}

bool Gradient::load(const std::string& sXMLFilename)
{
    m_colorMap.clear();

    tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
    int iErr = doc->LoadFile(sXMLFilename.c_str());
    if(iErr != tinyxml2::XML_NO_ERROR)
    {
        LOG_err("Error opening gradient XML file: %s - Error %d", sXMLFilename.c_str(), iErr);
        delete doc;
        return false;
    }

    tinyxml2::XMLElement* root = doc->RootElement();
    if(root == NULL)
    {
        LOG_err("Error: Root element NULL in XML file %s. Ignoring...", sXMLFilename.c_str());
        delete doc;
        return false;
    }

    for(tinyxml2::XMLElement* val = root->FirstChildElement("val"); val != NULL; val = val->NextSiblingElement("val"))
    {
        float pos = 0.0f;

        val->QueryFloatAttribute("pos", &pos);
        const char* cCol = val->Attribute("col");
        if(cCol != NULL)
            m_colorMap[pos].fromString(cCol);
    }

    delete doc;
    return true;
}
