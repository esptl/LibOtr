#define WINVER			0x0500
#define _WIN32_IE		0x0600
#define _WIN32_WINNT	WINVER
#define _WIN32_WINDOWS	WINVER
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#define strcasecmp _stricmp
#define strncasecmp _strnicmp
//#define snprintf _snprintf
