#include "Stringbank.h"
#include "Hash.h"
#include <cstring>
#include <assert.h>

Stringbank::Stringbank(uint8_t* fromData, uint64_t dataLen)
{
    curLangOffset = 0;
    raw = fromData;
    load(fromData, dataLen);
}

Stringbank::~Stringbank()
{
    free(raw);
}

const char* Stringbank::getString(const char* stringID)
{
    return getString(Hash::hash(stringID));
}

const char* Stringbank::getString(uint64_t stringHash)
{
    //Binary search
    uint64_t first = 0;
    uint64_t last = header->numStrings - 1;
    uint64_t middle = (first + last) / 2;

    while(first <= last)
    {
        uint64_t mid = ids[middle];
        if(mid < stringHash)
            first = middle + 1;
        else if(mid == stringHash)
            return getStringAt(middle);
        else
            last = middle - 1;

        middle = (first + last) / 2;
    }

    return NULL;
}

bool Stringbank::setLanguage(const char* langID)
{
    LanguageOffset* offset = offsets;
    for(unsigned int i = 0; i < header->numLanguages; i++)
    {
        if(strcmp(offset->languageID, langID) == 0)
        {
            curLangOffset = offset->offset;
            return true;
        }
        offset++;
    }
    return false;
}

const char* Stringbank::getLanguage()
{
    return offsets[curLangOffset].languageID;
}

const char* Stringbank::getStringAt(uint64_t offset)
{
    return &stringData[stringPointers[(offset*header->numLanguages)+curLangOffset]];
}

void Stringbank::load(uint8_t* fromData, uint64_t dataLen)
{
    assert(dataLen > sizeof(StringBankHeader));

    header = (StringBankHeader*)fromData;
    
    assert(dataLen > sizeof(StringBankHeader) + header->numLanguages * sizeof(LanguageOffset) + header->numStrings * sizeof(StringID) + header->numStrings*header->numLanguages * sizeof(StringDataPointer));

    uint64_t offset = sizeof(StringBankHeader);

    offsets = (LanguageOffset*) &fromData[offset];

    offset += header->numLanguages * sizeof(LanguageOffset);

    ids = (uint64_t*)&fromData[offset];

    offset += header->numStrings * sizeof(StringID);

    stringPointers = (uint64_t*)&fromData[offset];

    offset += header->numStrings*header->numLanguages * sizeof(StringDataPointer);

    stringData = (const char*)&fromData[offset];
}
