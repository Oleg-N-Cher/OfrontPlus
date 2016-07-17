// windows.h wrapper
// Includes windows.h while avoiding conflicts with Oberon types.

#define BOOLEAN _BOOLEAN
#define BYTE _BYTE
#define CHAR _CHAR

#undef _WIN32_WINNT
// 0x0501 is for Windows XP (no service pack)
#define _WIN32_WINNT 0x0501
#include <windows.h>

#undef BOOLEAN
#undef BYTE
#undef CHAR
