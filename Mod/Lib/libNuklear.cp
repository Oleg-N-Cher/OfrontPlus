MODULE [foreign] libNuklear;
IMPORT SYSTEM;

TYPE
  char_ptr = POINTER [notag] TO ARRAY [notag] OF SHORTCHAR;
  int = INTEGER;
  float = SHORTREAL;
  struct = SYSTEM.PTR;

CONST
  WINDOW_BORDER*           =  {0}; (* Draws a border around the window to visually separate window from the background *)
  WINDOW_MOVABLE*          =  {1}; (* The movable flag indicates that a window can be moved by user input or by dragging the window header *)
  WINDOW_SCALABLE*         =  {2}; (* The scalable flag indicates that a window can be scaled by user input by dragging a scaler icon at the button of the window *)
  WINDOW_CLOSABLE*         =  {3}; (* adds a closable icon into the header *)
  WINDOW_MINIMIZABLE*      =  {4}; (* adds a minimize icon into the header *)
  WINDOW_NO_SCROLLBAR*     =  {5}; (* Removes the scrollbar from the window *)
  WINDOW_TITLE*            =  {6}; (* Forces a header at the top at the window showing the title *)
  WINDOW_SCROLL_AUTO_HIDE* =  {7}; (* Automatically hides the window scrollbar if no user interaction: also requires delta time in `nk_context` to be set each frame *)
  WINDOW_BACKGROUND*       =  {8}; (* Always keep window in the background *)
  WINDOW_SCALE_LEFT*       =  {9}; (* Puts window scaler in the left-ottom corner instead right-bottom*)
  WINDOW_NO_INPUT*         = {10}; (* Prevents window of scaling, moving or getting focus *)

(* ============================================================================
 *
 *                                  API
 *
 * =========================================================================== *)

TYPE
  Context* = RECORD [notag] END;
  Color* = SET;
  Rect* = RECORD [notag] x, y, w, h: float END;

PROCEDURE- RectStruct* (x, y, w, h: float; rect: Rect) "rect = nk_rect(x, y, w, h)";

PROCEDURE- defStructContext "#define libNuklear_Context struct nk_context";
PROCEDURE- defStructColor "#define libNuklear_Color struct nk_color";
PROCEDURE- defStructRect "#define libNuklear_Rect struct nk_rect";

(*  nk_begin - starts a new window; needs to be called every frame for 
 *  every window (unless hidden) or otherwise the window gets removed
 *  Parameters:
 *      @ctx must point to an previously initialized `nk_context` struct
 *      @title window title and identifier. Needs to be persistent over
 *        frames to identify the window
 *      @bounds initial position and window size. However if you do not
 *        define `NK_WINDOW_SCALABLE` or `NK_WINDOW_MOVABLE` you can set
 *        window position and size every frame
 *      @flags window flags defined in `enum nk_panel_flags` with a number
 *        of different window behaviors
 *  Return values:
 *      returns 1 if the window can be filled up with widgets from this
 *        point until `nk_end or 0 otherwise for example if minimized `*)

PROCEDURE- Begin* (
	VAR ctx: Context; title: char_ptr; bounds: Rect; flags: SET): int
    "nk_begin(ctx, title, bounds, flags)";

(*  nk_end - needs to be called at the end of the window building process
 *    to process scaling, scrollbars and general cleanup. All widget calls
 *    after this functions will result in asserts or no state changes
 *  Parameters:
 *      @ctx must point to an previously initialized `nk_context` struct *)
PROCEDURE- End* (VAR ctx: Context) "nk_end(ctx)";

(* =============================================================================
 *
 *                                  COLOR
 *
 * ============================================================================= *)

PROCEDURE- RGB* (r, g, b: int): Color "nk_rgb(r, g, b)";

END libNuklear.
