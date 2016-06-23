#define WINVER			0x0500
#define _WIN32_IE		0x0600
#define _WIN32_WINNT	WINVER
#define _WIN32_WINDOWS	WINVER
#define WIN32_LEAN_AND_MEAN
#define GPGRT_ENABLE_ES_MACROS
#define _STAT_DEFINED
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#define	S_IRWXU	0000700			/* RWX mask for owner */
#define	S_IRUSR	0000400			/* R for owner */
#define	S_IWUSR	0000200			/* W for owner */
#define	S_IXUSR	0000100			/* X for owner */

#define strcasecmp _stricmp
#define strncasecmp _strnicmp
//#define snprintf _snprintf
