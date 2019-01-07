MODULE Kernel;
IMPORT SYSTEM, Heap, Platform, Out := Console;

TYPE
  CString = Platform.ADRINT;
  HaltProcedure = PROCEDURE(code: INTEGER; mod: CString; pos: INTEGER);
  SignalHandler = PROCEDURE(signal: INTEGER);

VAR
  HaltCode-: INTEGER;

(* Signals and traps *)

(* TODO *)

(* Ctrl/C handling *)

PROCEDURE -SetInterruptHandler*     (h: SignalHandler) "SystemSetInterruptHandler((SYSTEM_ADRINT)h)";
PROCEDURE -SetQuitHandler*          (h: SignalHandler) "SystemSetQuitHandler((SYSTEM_ADRINT)h)";
PROCEDURE -SetBadInstructionHandler*(h: SignalHandler) "SystemSetBadInstructionHandler((SYSTEM_ADRINT)h)";


PROCEDURE DisplayHaltCode(code: INTEGER);
BEGIN
  CASE code OF
  | -1: Out.String("Assertion failure.")
  | -2: Out.String("Index out of range.")
  | -3: Out.String("Reached end of function without reaching RETURN.")
  | -4: Out.String("CASE statement: no matching label and no ELSE.")
  | -5: Out.String("Type guard failed.")
  | -6: Out.String("Implicit type guard in record assignment failed.")
  | -7: Out.String("Invalid case in WITH statement.")
  | -8: Out.String("Value out of range.")
  | -9: Out.String("Heap interrupted while locked, but lockdepth = 0 at unlock.")
  |-10: Out.String("NIL access.");
  |-11: Out.String("Alignment error.");
  |-12: Out.String("Divide by zero.");
  |-13: Out.String("Arithmetic overflow/underflow.");
  |-14: Out.String("Invalid function argument.");
  |-15: Out.String("Internal error, e.g. Type descriptor size mismatch.") 
  |-20: Out.String("Too many, or negative number of, elements in dynamic array.")
  |-25: Out.String("Memory allocation error.")
  ELSE
  END
END DisplayHaltCode;

PROCEDURE OutModName(s: CString; pos: INTEGER);
VAR
  i: INTEGER; ch: SHORTCHAR;
BEGIN
  Out.Ln;
  i := 0; SYSTEM.GET(s, ch);
  WHILE ch # 0X DO Out.Char(ch); INC(i); SYSTEM.GET(s + i, ch) END; Out.Char(" ");
  IF pos > 0 THEN (* pos > 0: line*256 + column *)
    Out.Int(pos DIV 256, 0); Out.Char(":"); Out.Int(pos MOD 256 + 1, 0)
  ELSE (* pos <= 0: raw text position *)
    Out.Int(-pos, 0)
  END
END OutModName;

PROCEDURE Halt*(code: INTEGER; mod: CString; pos: INTEGER);
BEGIN
  Heap.FINALL;
  HaltCode := code;
  OutModName(mod, pos); Out.String(" Terminated by Halt("); Out.Int(code, 0); Out.String("). ");
  IF code < 0 THEN DisplayHaltCode(code) END;
  Out.Ln;
  Platform.ExitOS(code)
END Halt;

PROCEDURE AssertFail*(code: INTEGER; mod: CString; pos: INTEGER);
BEGIN
  Heap.FINALL;
  OutModName(mod, pos); Out.String(" Assertion failure.");
  IF code # 0 THEN
    Out.String(" ASSERT code "); Out.Int(code, 0); Out.Char(".")
  END;
  Out.Ln;
  Platform.ExitOS(code)
END AssertFail;

PROCEDURE Exit*(code: INTEGER);
BEGIN
  Platform.ExitOS(code)
END Exit;

PROCEDURE -AAExternHaltHandler "extern void (*SYSTEM_HaltHandler)(INTEGER code, SYSTEM_ADRINT mod, INTEGER pos);";
PROCEDURE -AAExternAssertFailHandler "extern void (*SYSTEM_AssertFailHandler)(INTEGER code, SYSTEM_ADRINT mod, INTEGER pos);";

PROCEDURE -SetHaltHandler (p: HaltProcedure) "SYSTEM_HaltHandler = p";
PROCEDURE -SetAssertFail (p: HaltProcedure) "SYSTEM_AssertFailHandler = p";

PROCEDURE SetHalt*(p: HaltProcedure); BEGIN SetHaltHandler(p) END SetHalt;

BEGIN
  HaltCode := -128; SetHalt(Halt); SetAssertFail(AssertFail)
END Kernel.
