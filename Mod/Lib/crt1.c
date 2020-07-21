#define SYSTEM_Cfg_KERNEL32

int argc; char *argv; void *stkadr;

__declspec(dllimport) void __stdcall ExitProcess (int);
__declspec(dllimport) void exit (int);
__declspec(dllimport) int __getmainargs (int*, char**, char**, int, void*);
int __stdcall WinMain (void*, void*, char*, int);

/*
 * For now the GUI startup function is the same as the console one.
 * This simply gets rid of the annoying warning about not being able
 * to find WinMainCRTStartup when linking GUI applications.
 */
void _WinMain (void) {
  char* tmp; __getmainargs(&argc, &argv, &tmp, 0, &tmp);
  stkadr = __builtin_frame_address(0);
#ifdef SYSTEM_Cfg_KERNEL32
  ExitProcess(
#else //SYSTEM_Cfg_MSVCRT
  exit(
#endif
    WinMain((void*)0, (void*)0, (char*)0, 0)
  );
} //_WinMain
