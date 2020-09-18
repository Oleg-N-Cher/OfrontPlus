MODULE [foreign] libNuklear;
IMPORT SYSTEM;

(* ============================================================================
 *
 *                                  API
 *
 * =========================================================================== *)

TYPE
  context* = RECORD [notag] END;

PROCEDURE- defStructContext "#define libNuklear_context struct nk_context";

END libNuklear.
