extern int _main (void *stktop);
__attribute__((dllimport)) void __stdcall ExitProcess (int uExitCode);

void __stdcall WinMain (
  void *hInstance, void *hPrevInstance, char *lpCmdLine, int nCmdShow)
{
  ExitProcess(_main(__builtin_frame_address(0)));
} //WinMain
