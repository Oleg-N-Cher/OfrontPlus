MODULE [foreign] libNuklearX;
IMPORT SYSTEM, nk := libNuklear;

TYPE
  char_ptr = POINTER [notag] TO ARRAY [notag] OF SHORTCHAR;
  void_ptr = SYSTEM.PTR;
  int = INTEGER;
  enum = INTEGER;

(** 
 * @brief Main structure. Implementation depends on frontend
 * There always will be fields:
 *     struct nk_context *ctx;
 *     int nkcInited;
*)
TYPE
  Nkc* = RECORD [notag] END;

  MainLoopProc = PROCEDURE (loopArg: void_ptr);

(** 
 * @brief Window mode constants, passed to initialization 
 *)
  window_mode* = enum; CONST
    WIN_NORMAL* = ORD("w"); (**< normal window mode, user can resize/maximize *)
    WIN_MAXIMIZED* = ORD("m"); (**< like normal, but starts maximized *)
    WIN_FIXED* = ORD("x"); (**< fixed size mode. User can't resize/maximize *)
    WIN_FULLSCREEN* = ORD("f"); (**< real fullscreen. In almost all frontends changes the screen's resolution *)
    WIN_FULLSCREEN_DESKTOP* = ORD("d"); (**< "virtual" fullscreen. Removes windows decorations, 
     * expands window to full size, changes window size *)

(* Events system *)
TYPE
  EventType* = enum; CONST
    ENONE* = 0 ; (**< No event *)
    EWINDOW* = 1; (**< Window event *)
    EKEY* = 2; (**< Keyboard events *)
    EQUIT* = 3; (**< Quit from app event *)

TYPE
  WindowEvent* = RECORD [notag]
    type*: EventType;
    param*: int;
  END;

  KeyEvent* = RECORD [notag]
    type*: EventType;
    code*: int;
    mod*: int;
  END;

  Event* = RECORD [notag]
    type*: EventType;
    window*: WindowEvent;
    key*: KeyEvent;
  END;

PROCEDURE- defWindowEvent "#define libNuklearX_WindowEvent struct nkc_window_event";
PROCEDURE- defKeyEvent "#define libNuklearX_KeyEvent struct nkc_key_event";
PROCEDURE- defUnionEvent "#define libNuklearX_Event union nkc_event";
(* end Events *)

PROCEDURE- AdefineImplementation "#define NKC_IMPLEMENTATION";
PROCEDURE- AdefineReal "#define REAL _REAL";
PROCEDURE- AincludeNuklearCross* '#include "nuklear_cross.h"';
PROCEDURE- AundefReal "#undef REAL";

PROCEDURE- defStructNkc "#define libNuklearX_Nkc struct nkc";




(** @defgroup init Initialization/finalizing, event loop
 *  @brief Functions, related to initialization
 *  @{
 *)


(**
 * @brief Fatal error. Application exit. Die with message. Printf-like
 * @param fmt - format string
 *)
PROCEDURE- ErrDie* (msg: char_ptr) "nkc_rdie((char*)msg)";

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
  VAR nkcHandle: Nkc; title: char_ptr; width, height: int; windowMode: window_mode)
  : POINTER TO nk.Context
    "nkc_init(nkcHandle, title, width, height, windowMode)";

(**
 * @brief Poll window's events in a loop
 * @return union nkc_event
 * @see union nkc_event
 *)
PROCEDURE- PollEvents* (VAR nkcHandle: Nkc; event: Event) "event = nkc_poll_events(nkcHandle)";

(** @return Returns nkcHandle->ctx *)
(*NK_API struct nk_context* nkc_get_ctx(struct nkc* nkcHandle);*)

(** @brief Finalizing *)
PROCEDURE- ShutDown* (VAR nkcHandle: Nkc) "nkc_shutdown(nkcHandle)";

(** @} *) (* end of init group *)



(** @defgroup render Rendering
 *  @brief You can render GUI and BG separately.
 *  @{
 */
NK_API void nkc_render_bg(struct nkc* nkcHandle, struct nk_color bgColor);
NK_API void nkc_render_gui(struct nkc* nkcHandle);

/** Just calls nkc_render_bg than nkc_render_gui *)
PROCEDURE- Render* (VAR nkcHandle: Nkc; bgColor: nk.Color) "nkc_render(nkcHandle, bgColor)";
(** @} *) (* end of render group *)



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
  VAR nkcHandle: Nkc; mainLoopProc: MainLoopProc; loopArg: void_ptr)
    "nkc_set_main_loop(nkcHandle, (void*)mainLoopProc, (void*)loopArg)";
(** Exit from main loop *)

PROCEDURE- StopMainLoop* (VAR nkcHandle: Nkc) "nkc_stop_main_loop(nkcHandle)";
(** @} *) (* end of mainloop group *)

END libNuklearX.
