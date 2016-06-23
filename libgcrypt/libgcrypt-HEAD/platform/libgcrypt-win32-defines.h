#define inline __inline

#define WINVER			0x0500
#define _WIN32_IE		0x0600
#define _WIN32_WINNT	WINVER
#define _WIN32_WINDOWS	WINVER
#define _STAT_DEFINED

#include <WinSock2.h>
#include <Windows.h>
#include <sys/types.h>
#include <sys/stat.h>

#define strcasecmp _stricmp
#define strncasecmp _strnicmp
//#define snprintf _snprintf

#ifdef _WIN64
#define __x86_64__
#else
#define __i386__
#endif

#pragma warning( disable: 4996 4244 4018 )
