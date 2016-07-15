// windows.h wrapper
// Includes windows.h while avoiding conflicts with Oberon types.

#define BOOLEAN _BOOLEAN
#define BYTE _BYTE
#define CHAR _CHAR
#include <windows.h>
#undef BOOLEAN
#undef BYTE
#undef CHAR
