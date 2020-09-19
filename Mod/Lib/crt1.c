int argc; char **argv;

__declspec(dllimport) void __stdcall ExitProcess (int);
__declspec(dllimport) int __getmainargs (int*, char***, char**, int, void*);
extern int _WinMain (int argc, char **argv, void *stkptr);

void WinMain (void) {
  char* tmp;
  __getmainargs(&argc, &argv, &tmp, 0, &tmp);
  ExitProcess(
    _WinMain(argc, argv, __builtin_frame_address(0))
  );
} //WinMain
