extern int _main (void *stktop);

int WinMain (void) {
  return _main(__builtin_frame_address(0));
} //WinMain
