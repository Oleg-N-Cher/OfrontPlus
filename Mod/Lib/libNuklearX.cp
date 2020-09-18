MODULE [foreign] libNuklearX;
IMPORT SYSTEM, nk := libNuklear;

TYPE
  char_ptr = POINTER [notag] TO ARRAY [notag] OF SHORTCHAR;
  void_ptr = SYSTEM.ADRINT;
  int = INTEGER;

(** 
 * @brief Main structure. Implementation depends on frontend
 * There always will be fields:
 *     struct nk_context *ctx;
 *     int nkcInited;
*)
TYPE
  nkc* = RECORD [notag] END;

  MainLoopFunc = PROCEDURE;

(** 
 * @brief Window mode constants, passed to initialization 
 *)
  window_mode* = INTEGER; CONST
    WIN_NORMAL* = ORD("w"); (**< normal window mode, user can resize/maximize *)
    WIN_MAXIMIZED* = ORD("m"); (**< like normal, but starts maximized *)
    WIN_FIXED* = ORD("x"); (**< fixed size mode. User can't resize/maximize *)
    WIN_FULLSCREEN* = ORD("f"); (**< real fullscreen. In almost all frontends changes the screen's resolution *)
    WIN_FULLSCREEN_DESKTOP* = ORD("d"); (**< "virtual" fullscreen. Removes windows decorations, 
     * expands window to full size, changes window size *)

PROCEDURE- AdefineImplementation "#define NKC_IMPLEMENTATION";
PROCEDURE- AdefineReal "#define REAL _REAL";
PROCEDURE- AincludeNuklearCross* '#include "nuklear_cross.h"';
PROCEDURE- AundefReal "#undef REAL";

PROCEDURE- defStructNkc "#define libNuklearX_nkc struct nkc";

(**
 * @brief Initialize the library, create the operation system window
 * @param nkcHandle - pointer to struct nkc
 * @param title - window title
 * @param width - window width in pixels
 * @param height - window height in pixels
 * @param windowMode - one value from enum nkc_window_mode
 * @see enum nkc_window_mode
 * @return if error, than returns NULL
 *)
PROCEDURE- Init* (
  VAR nkcHandle: nkc; title: char_ptr; width, height: int; windowMode: window_mode)
  : POINTER TO nk.context
    "nkc_init(nkcHandle, title, width, height, windowMode)";

(** @defgroup mainloop Main loop
 *  @brief Required for Emscripten
 *  @{
 *)
 
(**
 * @brief Starts the 'infinite' main loop. Required for Emscipten only.
 * @param mainLoopFunc - pointer to the function, that will be called each loop iteration
 * @param loopArg - pointer to your data structer, that will be passed to mainLoopFunc function
 *)
PROCEDURE- SetMainLoop* (
  VAR nkcHandle: nkc; mainLoopFunc: MainLoopFunc; loopArg: void_ptr)
    "nkc_set_main_loop(nkcHandle, (void(*)(void*))mainLoopFunc, (void*)loopArg)";
(** Exit from main loop *)

PROCEDURE- StopMainLoop* (VAR nkcHandle: nkc) "nkc_stop_main_loop(nkcHandle)";
(** @} *) (* end of mainloop group *)

END libNuklearX.
