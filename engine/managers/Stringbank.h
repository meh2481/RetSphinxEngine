#pragma once
#include "ResourceTypes.h"

class Stringbank
{
    uint8_t* raw;

    uint32_t curLangOffset;

    StringBankHeader* header;
    LanguageOffset* offsets;
    uint64_t* ids;
    uint64_t* stringPointers;
    const char* stringData;

    const char* getStringAt(uint64_t offset);
    void load(uint8_t* fromData, uint64_t dataLen);

    Stringbank(){};

public:
    Stringbank(uint8_t* fromData, uint64_t dataLen);    //Load from data (MUST BE ALLOCATED WITH MALLOC. FREED ON OBJECT DELETION)
    ~Stringbank();

    const char* getString(const char* stringID);
    const char* getString(uint64_t stringHash);

    bool setLanguage(const char* langID);    //Set to a particular language. Returns true on success, false on failure
    const char* getLanguage();
};