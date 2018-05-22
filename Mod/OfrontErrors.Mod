MODULE OfrontErrors; IMPORT Log := Console;
(*
List of Ofront error numbers

NW, RC, JT / 16.1.95
*)

PROCEDURE LogErrMsg*(n: SHORTINT);
BEGIN
  CASE n OF
(*
    1. Incorrect use of language Oberon
*)
      0: Log.String("undeclared identifier")
  |   1: Log.String("multiply defined identifier")
  |   2: Log.String("illegal character in number")
  |   3: Log.String("illegal character in string")
  |   4: Log.String("identifier does not match procedure name")
  |   5: Log.String("comment not closed")

  |   9: Log.String('"=" expected')

  |  12: Log.String("type definition starts with incorrect symbol")
  |  13: Log.String("factor starts with incorrect symbol")
  |  14: Log.String("statement starts with incorrect symbol")
  |  15: Log.String("declaration followed by incorrect symbol")
  |  16: Log.String("MODULE expected")

  |  18: Log.String('"." missing')
  |  19: Log.String('"," missing')
  |  20: Log.String('":" missing')

  |  22: Log.String('")" missing')
  |  23: Log.String('"]" missing')
  |  24: Log.String('"}" missing')
  |  25: Log.String("OF missing")
  |  26: Log.String("THEN missing")
  |  27: Log.String("DO missing")
  |  28: Log.String("TO missing")

  |  30: Log.String('"(" missing')

  |  34: Log.String('":=" missing')
  |  35: Log.String('"," or OF expected')

  |  38: Log.String("identifier expected")
  |  39: Log.String('";" missing')

  |  41: Log.String("END missing")

  |  44: Log.String("UNTIL missing")

  |  46: Log.String("EXIT not within loop statement")
  |  47: Log.String("illegally marked identifier")

  |  50: Log.String("expression should be constant")
  |  51: Log.String("constant not an integer")
  |  52: Log.String("identifier does not denote a type")
  |  53: Log.String("identifier does not denote a record type")
  |  54: Log.String("result type of procedure is not a basic type")
  |  55: Log.String("procedure call of a function")
  |  56: Log.String("assignment to non-variable")
  |  57: Log.String("pointer not bound to record or array type")
  |  58: Log.String("recursive type definition")
  |  59: Log.String("illegal open array parameter")
  |  60: Log.String("wrong type of case label")
  |  61: Log.String("inadmissible type of case label")
  |  62: Log.String("case label defined more than once")
  |  63: Log.String("illegal value of constant")
  |  64: Log.String("more actual than formal parameters")
  |  65: Log.String("fewer actual than formal parameters")
  |  66: Log.String("element types of actual array and formal open array differ")
  |  67: Log.String("actual parameter corresponding to open array is not an array")
  |  68: Log.String("control variable must be integer")
  |  69: Log.String("parameter must be an integer constant")
  |  70: Log.String("pointer or VAR / IN record required as formal receiver")
  |  71: Log.String("pointer expected as actual receiver")
  |  72: Log.String("procedure must be bound to a record of the same scope")
  |  73: Log.String("procedure must have level 0")
  |  74: Log.String("procedure unknown in base type")
  |  75: Log.String("invalid call of base procedure")
  |  76: Log.String("this variable (field) is read only")
  |  77: Log.String("object is not a record")
  |  78: Log.String("dereferenced object is not a variable")
  |  79: Log.String("indexed object is not a variable")
  |  80: Log.String("index expression is not an integer")
  |  81: Log.String("index out of specified bounds")
  |  82: Log.String("indexed variable is not an array")
  |  83: Log.String("undefined record field")
  |  84: Log.String("dereferenced variable is not a pointer")
  |  85: Log.String("guard or test type is not an extension of variable type")
  |  86: Log.String("guard or testtype is not a pointer")
  |  87: Log.String("guarded or tested variable is neither a pointer nor a VAR- or IN-parameter record")
  |  88: Log.String("open array not allowed as variable, record field or array element")

  |  92: Log.String("operand of IN not an integer, or not a set")
  |  93: Log.String("set element type is not an integer")
  |  94: Log.String("operand of & is not of type BOOLEAN")
  |  95: Log.String("operand of OR is not of type BOOLEAN")
  |  96: Log.String("operand not applicable to (unary) +")
  |  97: Log.String("operand not applicable to (unary) -")
  |  98: Log.String("operand of ~ is not of type BOOLEAN")
  |  99: Log.String("ASSERT fault")
  | 100: Log.String("incompatible operands of dyadic operator")
  | 101: Log.String("operand type inapplicable to *")
  | 102: Log.String("operand type inapplicable to /")
  | 103: Log.String("operand type inapplicable to DIV")
  | 104: Log.String("operand type inapplicable to MOD")
  | 105: Log.String("operand type inapplicable to +")
  | 106: Log.String("operand type inapplicable to -")
  | 107: Log.String("operand type inapplicable to = or #")
  | 108: Log.String("operand type inapplicable to relation")
  | 109: Log.String("overriding method must be exported")
  | 110: Log.String("operand is not a type")
  | 111: Log.String("operand inapplicable to (this) function")
  | 112: Log.String("operand is not a variable")
  | 113: Log.String("incompatible assignment")
  | 114: Log.String("string too long to be assigned")
  | 115: Log.String("parameter doesn't match")
  | 116: Log.String("number of parameters doesn't match")
  | 117: Log.String("result type doesn't match")
  | 118: Log.String("export mark doesn't match with forward declaration")
  | 119: Log.String("redefinition textually precedes procedure bound to base type")
  | 120: Log.String("type of expression following IF, WHILE, UNTIL or ASSERT is not BOOLEAN")
  | 121: Log.String("called object is not a procedure (or is an interrupt procedure)")
  | 122: Log.String("actual VAR-, IN-, or OUT-parameter is not a variable")
  | 123: Log.String("type is not identical with that of formal VAR-, IN-, or OUT-parameter")
  | 124: Log.String("type of result expression differs from that of procedure")
  | 125: Log.String("type of case expression is neither INTEGER nor CHAR")
  | 126: Log.String("this expression cannot be a type or a procedure")
  | 127: Log.String("illegal use of object")
  | 128: Log.String("unsatisfied forward reference")
  | 129: Log.String("unsatisfied forward procedure")
  | 130: Log.String("WITH clause does not specify a variable")
  | 131: Log.String("LEN not applied to array")
  | 132: Log.String("dimension in LEN too large or negative")
  | 136: Log.String("LEN applied to untagged array")
  | 133: Log.String("system flag doesn't match forward declaration")
  | 135: Log.String("SYSTEM not imported")
  | 138: Log.String("NEW not allowed for untagged structures")
  | 139: Log.String("test applied to untagged record")

  | 150: Log.String("key inconsistency of imported module")
  | 151: Log.String("incorrect symbol file")
  | 152: Log.String("symbol file of imported module not found")
  | 153: Log.String("object or symbol file not opened (disk full?)")
  | 154: Log.String("recursive import not allowed")
  | 155: Log.String("generation of new symbol file not allowed")
  | 156: Log.String("default target machine address size and alignment used")
  | 157: Log.String("syntax error in parameter file")
  | 177: Log.String("IN only allowed for records and arrays")

(*
    2. Limitations of implementation
*)
  | 200: Log.String("not yet implemented")
  | 201: Log.String("lower bound of set range greater than higher bound")
  | 202: Log.String("set element greater than MAX(SET) or less than 0")
  | 203: Log.String("number too large")
  | 204: Log.String("product too large")
  | 205: Log.String("division by zero")
  | 206: Log.String("sum too large")
  | 207: Log.String("difference too large")
  | 208: Log.String("overflow in arithmetic shift")
  | 209: Log.String("case range too large")
  | 213: Log.String("too many cases in case statement")
  | 218: Log.String("illegal value of parameter  (0 <= p < 256)")
  | 219: Log.String("machine registers cannot be accessed")
  | 220: Log.String("illegal value of parameter")
  | 221: Log.String("too many pointers in a record")
  | 222: Log.String("too many global pointers")
  | 223: Log.String("too many record types")
  | 224: Log.String("too many pointer types")
  | 225: Log.String("address of pointer variable too large (move forward in text)")
  | 226: Log.String("too many exported procedures")
  | 227: Log.String("too many imported modules")
  | 228: Log.String("too many exported structures")
  | 229: Log.String("too many nested records for import")
  | 230: Log.String("too many constants (strings) in module")
  | 231: Log.String("too many link table entries (external procedures)")
  | 232: Log.String("too many commands in module")
  | 233: Log.String("record extension hierarchy too high")
  | 234: Log.String("export of recursive type not allowed")
  | 240: Log.String("identifier too long")
  | 241: Log.String("string too long")
  | 242: Log.String("address overflow")
  | 243: Log.String("concatenation of module, type, and guarded variable exceeds maximum name length")
  | 244: Log.String("cyclic type definition not allowed")

(*
    3. Compiler Warnings
*)
  | 301: Log.String("implicit type cast")
  | 302: Log.String("guarded variable can be side-effected")
  | 306: Log.String("inappropriate symbol file ignored")
  | 308: Log.String("SYSTEM.VAL result includes memory past end of source variable") (* DCWB *)

(*
    4. Analyzer Warnings

  900	never used
  901	never set
  902	used before set
  903	set but never used
  904	used as varpar, possibly not set
  905	also declared in outer scope
  906	access/assignment to intermediate
  907	redefinition
  999	ERROR, notify author


    5. Run-time Error Numbers

SYSTEM_halt
	  0	silent HALT(0)
   1..255  HALT(n), cf. SYSTEM_halt
	-1	assertion failed, cf. SYSTEM_assert
	-2	invalid array index
	-3	function procedure without RETURN statement
	-4	invalid case in CASE statement
	-5	type guard failed
	-6	implicit type guard in record assignment failed
	-7	invalid case in WITH statement
	-8	value out of range
	-9	(delayed) interrupt
	-10	NIL access
	-11	alignment error
	-12	zero divide
	-13	arithmetic overflow/underflow
	-14	invalid function argument
	-15	internal error


Unix signals

  1
  2	interrupt signal
  3
  4	invalid instruction, HALT
  5
  6
  7
  8	arithmetic exception: division by zero, overflow; IU = integer unit, FPU = floating point unit
  9	
10	bus error, unaligned data access
11	segmentation violation, NIL-access
12
13	access to closed pipe

*)
  END
END LogErrMsg;

END OfrontErrors.
