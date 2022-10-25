extern int _main (void *stktop);
__attribute__((dllimport)) void __stdcall ExitProcess (int uExitCode);

void *WinApi_hInstance, *WinApi_hPrevInstance;

void __stdcall WinMain (
  void *hInstance, void *hPrevInstance, char *lpCmdLine, int nCmdShow)
{
  WinApi_hInstance = hInstance; WinApi_hPrevInstance = hPrevInstance;
  ExitProcess(_main(__builtin_frame_address(0)));
} //WinMain
