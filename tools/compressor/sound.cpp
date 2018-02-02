#include "main.h"
#include "tinyxml2.h"
#include "fmod.hpp"
#include "Logger.h"

void extractSoundGeometry(const std::string& xmlFilename)
{
    tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
    int iErr = doc->LoadFile(xmlFilename.c_str());
    if(iErr != tinyxml2::XML_NO_ERROR)
    {
        LOG(ERR) << "Error parsing object XML file " << xmlFilename << ": Error " << iErr;
        delete doc;
        return;
    }

    //Grab root element
    tinyxml2::XMLElement* root = doc->RootElement();
    if(root == NULL)
    {
        LOG(ERR) << "Error: Root element NULL in XML file " << xmlFilename;
        delete doc;
        return;
    }


    //TODO
    delete doc;
}
