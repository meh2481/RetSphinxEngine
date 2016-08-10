#include "SystemUtils.h"
#ifdef _WIN32
#include <Windows.h>
#define BUFFER_SIZE 100
#else
#error TODO Get locale in UNIX/POSIX using those functions
#endif

//See OSFunctions.cpp in Aquaria for inspiration

namespace SystemUtils
{
	std::string getCurLocale()
	{
#ifdef _WIN32
		WCHAR wcBuffer[BUFFER_SIZE];

		//NOTE: Breaks pre-Vista compat
		int iResult = GetLocaleInfoEx(
			LOCALE_NAME_USER_DEFAULT,
			LOCALE_SNAME,
			wcBuffer,
			BUFFER_SIZE
		);

		if(iResult > 0)
		{
			char* cBuf = (char*)malloc(BUFFER_SIZE);
			//Wandows is stupid and wants wide strings, but locale strings should always be ASCII, so convert
			for(int i = 0; i <= lstrlenW(wcBuffer); i++)
			{
				cBuf[i] = (char)wcBuffer[i];
			}

			char *firstDash = strchr(cBuf, '-');
			if(firstDash != NULL)
				*firstDash = '\0';

			std::string s(cBuf);
			free(cBuf);
			return s;
		}
		
#endif
		return std::string("");
	}
}