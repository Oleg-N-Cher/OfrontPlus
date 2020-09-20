MODULE [foreign] libNuklear;
IMPORT SYSTEM;

TYPE
  char_ptr = POINTER [notag] TO ARRAY [notag] OF SHORTCHAR;
  int = INTEGER;
  enum = INTEGER;
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

  DYNAMIC* = 0; STATIC* = 1;

(* ============================================================================
 *
 *                                  API
 *
 * =========================================================================== *)

TYPE
  Context* = RECORD [notag] END;
  Color* = SET;
  Rect* = RECORD [notag] x, y, w, h: float END;

(* =============================================================================
 *                                  TEXT
 * ============================================================================= *)
CONST
  TEXT_ALIGN_LEFT*     = {};
  TEXT_ALIGN_CENTERED* = {1};
  TEXT_ALIGN_RIGHT*    = {2};
  TEXT_ALIGN_TOP*      = {3};
  TEXT_ALIGN_MIDDLE*   = {4};
  TEXT_ALIGN_BOTTOM*   = {5};

  TEXT_LEFT*     = TEXT_ALIGN_MIDDLE + TEXT_ALIGN_LEFT;
  TEXT_CENTERED* = TEXT_ALIGN_MIDDLE + TEXT_ALIGN_CENTERED;
  TEXT_RIGHT*    = TEXT_ALIGN_MIDDLE + TEXT_ALIGN_RIGHT;


(* =============================================================================
 *                                  TEXT EDIT
 * ============================================================================= *)
  EDIT_DEFAULT*              = {};
  EDIT_READ_ONLY*            = {0};
  EDIT_AUTO_SELECT*          = {1};
  EDIT_SIG_ENTER*            = {2};
  EDIT_ALLOW_TAB*            = {3};
  EDIT_NO_CURSOR*            = {4};
  EDIT_SELECTABLE*           = {5};
  EDIT_CLIPBOARD*            = {6};
  EDIT_CTRL_ENTER_NEWLINE*   = {7};
  EDIT_NO_HORIZONTAL_SCROLL* = {8};
  EDIT_ALWAYS_INSERT_MODE*   = {9};
  EDIT_MULTILINE*            = {10};
  EDIT_GOTO_END_ON_ACTIVATE* = {11};

  EDIT_SIMPLE* = EDIT_ALWAYS_INSERT_MODE;
  EDIT_FIELD*  = EDIT_SIMPLE + EDIT_SELECTABLE + EDIT_CLIPBOARD;
  EDIT_BOX*    = EDIT_ALWAYS_INSERT_MODE + EDIT_SELECTABLE + EDIT_MULTILINE + EDIT_ALLOW_TAB + EDIT_CLIPBOARD;
  EDIT_EDITOR* = EDIT_SELECTABLE + EDIT_MULTILINE + EDIT_ALLOW_TAB + EDIT_CLIPBOARD;

  EDIT_ACTIVE*      = {0}; (* edit widget is currently being modified *)
  EDIT_INACTIVE*    = {1}; (* edit widget is not active and is not being modified *)
  EDIT_ACTIVATED*   = {2}; (* edit widget went from state inactive to state active *)
  EDIT_DEACTIVATED* = {3}; (* edit widget went from state active to state inactive *)
  EDIT_COMMITED*    = {4}; (* edit widget has received an enter and lost focus *)

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
	VAR ctx: Context; title: char_ptr; bounds: Rect; flags: SET): BOOLEAN
    "(nk_begin(ctx, title, bounds, flags) != 0)";

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

(* -------------------------------------------------------------
 *
 *                          LAYOUT
 *
 * --------------------------------------------------------------*)

PROCEDURE- LayoutRowDynamic* (VAR ctx: Context; height: float; cols: int)
  "nk_layout_row_dynamic(ctx, height, cols)";

PROCEDURE- LayoutRowStatic* (VAR ctx: Context; height: float; item_width, cols: int)
  "nk_layout_row_static(ctx, height, item_width, cols)";

(*  nk_layout_row - specifies row columns in array as either window ratio or size
 *  Parameters:
 *      @ctx must point to an previously initialized `nk_context`
 *      @fmt either `NK_DYNAMIC` for window ratio or `NK_STATIC` for fixed size columns
 *      @row_height holds height of each widget in row or zero for auto layouting
 *      @cols number of widget inside row *)
PROCEDURE- LayoutRow* (
  VAR ctx: Context; layout_format: enum; height: float; cols: int;
  ratio: POINTER TO ARRAY [notag] OF SHORTREAL)
    "nk_layout_row(ctx, layout_format, height, cols, ratio)";

(* =============================================================================
 *
 *                                  TEXT
 *
 * ============================================================================= *)

(*PROCEDURE- Text* (VAR ctx: Context;  nk_text(struct nk_context*, const char*, int, nk_flags);
  "nk_text(struct nk_context*, const char*, int, nk_flags)";
PROCEDURE- TextColored* (VAR ctx: Context;  nk_text_colored(struct nk_context*, const char*, int, nk_flags, struct nk_color);
  "nk_text_colored(struct nk_context*, const char*, int, nk_flags, struct nk_color)";
PROCEDURE- TextWrap* (VAR ctx: Context;  nk_text_wrap(struct nk_context*, const char*, int);
  "nk_text_wrap(struct nk_context*, const char*, int)";
PROCEDURE- TextWrapColored* (VAR ctx: Context;  nk_text_wrap_colored(struct nk_context*, const char*, int, struct nk_color);
  "nk_text_wrap_colored(struct nk_context*, const char*, int, struct nk_color)";*)
PROCEDURE- Label* (VAR ctx: Context; text: char_ptr; align: SET)
  "nk_label(ctx, text, align)";

(*----------------------------------------------------------------
 *
 *                          BUTTON
 *
 * --------------------------------------------------------------*)

PROCEDURE- ButtonLabel* (VAR ctx: Context; title: char_ptr): BOOLEAN
  "(nk_button_label(ctx, title) != 0)";


(* =============================================================================
 *
 *                                  TEXT EDIT
 *
 * ============================================================================= *)

PROCEDURE- EditString* (
  VAR ctx: Context; flags: SET; buffer: char_ptr; VAR len: int; max: int): SET
    "nk_edit_string(ctx, flags, buffer, len, max, nk_filter_default)";
(*
NK_API nk_flags nk_edit_string_zero_terminated(struct nk_context*, nk_flags, char *buffer, int max, nk_plugin_filter);
NK_API nk_flags nk_edit_buffer(struct nk_context*, nk_flags, struct nk_text_edit*, nk_plugin_filter);
NK_API void nk_edit_focus(struct nk_context*, nk_flags flags);
NK_API void nk_edit_unfocus(struct nk_context* );
*)

END libNuklear.
