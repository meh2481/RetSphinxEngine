#include "Hash.h"

namespace Hash
{
    uint64_t hash(const char* str)
    {
        uint64_t hash = 5381;
        int c;

        while(c = *str++)
            hash = ((hash << 5) + hash) + c;

        return hash;
    }
}
