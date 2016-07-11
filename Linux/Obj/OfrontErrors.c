/* Ofront 1.2 -xtspkael */
#include "SYSTEM.h"
#include "Console.h"




export void OfrontErrors_LogErrMsg (INTEGER n);


void OfrontErrors_LogErrMsg (INTEGER n)
{
	switch (n) {
		case 0: 
			Console_String((CHAR*)"undeclared identifier", (LONGINT)22);
			break;
		case 1: 
			Console_String((CHAR*)"multiply defined identifier", (LONGINT)28);
			break;
		case 2: 
			Console_String((CHAR*)"illegal character in number", (LONGINT)28);
			break;
		case 3: 
			Console_String((CHAR*)"illegal character in string", (LONGINT)28);
			break;
		case 4: 
			Console_String((CHAR*)"identifier does not match procedure name", (LONGINT)41);
			break;
		case 5: 
			Console_String((CHAR*)"comment not closed", (LONGINT)19);
			break;
		case 9: 
			Console_String((CHAR*)"\"=\" expected", (LONGINT)13);
			break;
		case 12: 
			Console_String((CHAR*)"type definition starts with incorrect symbol", (LONGINT)45);
			break;
		case 13: 
			Console_String((CHAR*)"factor starts with incorrect symbol", (LONGINT)36);
			break;
		case 14: 
			Console_String((CHAR*)"statement starts with incorrect symbol", (LONGINT)39);
			break;
		case 15: 
			Console_String((CHAR*)"declaration followed by incorrect symbol", (LONGINT)41);
			break;
		case 16: 
			Console_String((CHAR*)"MODULE expected", (LONGINT)16);
			break;
		case 18: 
			Console_String((CHAR*)"\".\" missing", (LONGINT)12);
			break;
		case 19: 
			Console_String((CHAR*)"\",\" missing", (LONGINT)12);
			break;
		case 20: 
			Console_String((CHAR*)"\":\" missing", (LONGINT)12);
			break;
		case 22: 
			Console_String((CHAR*)"\")\" missing", (LONGINT)12);
			break;
		case 23: 
			Console_String((CHAR*)"\"]\" missing", (LONGINT)12);
			break;
		case 24: 
			Console_String((CHAR*)"\"}\" missing", (LONGINT)12);
			break;
		case 25: 
			Console_String((CHAR*)"OF missing", (LONGINT)11);
			break;
		case 26: 
			Console_String((CHAR*)"THEN missing", (LONGINT)13);
			break;
		case 27: 
			Console_String((CHAR*)"DO missing", (LONGINT)11);
			break;
		case 28: 
			Console_String((CHAR*)"TO missing", (LONGINT)11);
			break;
		case 30: 
			Console_String((CHAR*)"\"(\" missing", (LONGINT)12);
			break;
		case 34: 
			Console_String((CHAR*)"\":=\" missing", (LONGINT)13);
			break;
		case 35: 
			Console_String((CHAR*)"\",\" or OF expected", (LONGINT)19);
			break;
		case 38: 
			Console_String((CHAR*)"identifier expected", (LONGINT)20);
			break;
		case 39: 
			Console_String((CHAR*)"\";\" missing", (LONGINT)12);
			break;
		case 41: 
			Console_String((CHAR*)"END missing", (LONGINT)12);
			break;
		case 44: 
			Console_String((CHAR*)"UNTIL missing", (LONGINT)14);
			break;
		case 46: 
			Console_String((CHAR*)"EXIT not within loop statement", (LONGINT)31);
			break;
		case 47: 
			Console_String((CHAR*)"illegally marked identifier", (LONGINT)28);
			break;
		case 50: 
			Console_String((CHAR*)"expression should be constant", (LONGINT)30);
			break;
		case 51: 
			Console_String((CHAR*)"constant not an integer", (LONGINT)24);
			break;
		case 52: 
			Console_String((CHAR*)"identifier does not denote a type", (LONGINT)34);
			break;
		case 53: 
			Console_String((CHAR*)"identifier does not denote a record type", (LONGINT)41);
			break;
		case 54: 
			Console_String((CHAR*)"result type of procedure is not a basic type", (LONGINT)45);
			break;
		case 55: 
			Console_String((CHAR*)"procedure call of a function", (LONGINT)29);
			break;
		case 56: 
			Console_String((CHAR*)"assignment to non-variable", (LONGINT)27);
			break;
		case 57: 
			Console_String((CHAR*)"pointer not bound to record or array type", (LONGINT)42);
			break;
		case 58: 
			Console_String((CHAR*)"recursive type definition", (LONGINT)26);
			break;
		case 59: 
			Console_String((CHAR*)"illegal open array parameter", (LONGINT)29);
			break;
		case 60: 
			Console_String((CHAR*)"wrong type of case label", (LONGINT)25);
			break;
		case 61: 
			Console_String((CHAR*)"inadmissible type of case label", (LONGINT)32);
			break;
		case 62: 
			Console_String((CHAR*)"case label defined more than once", (LONGINT)34);
			break;
		case 63: 
			Console_String((CHAR*)"illegal value of constant", (LONGINT)26);
			break;
		case 64: 
			Console_String((CHAR*)"more actual than formal parameters", (LONGINT)35);
			break;
		case 65: 
			Console_String((CHAR*)"fewer actual than formal parameters", (LONGINT)36);
			break;
		case 66: 
			Console_String((CHAR*)"element types of actual array and formal open array differ", (LONGINT)59);
			break;
		case 67: 
			Console_String((CHAR*)"actual parameter corresponding to open array is not an array", (LONGINT)61);
			break;
		case 68: 
			Console_String((CHAR*)"control variable must be integer", (LONGINT)33);
			break;
		case 69: 
			Console_String((CHAR*)"parameter must be an integer constant", (LONGINT)38);
			break;
		case 70: 
			Console_String((CHAR*)"pointer or VAR record required as formal receiver", (LONGINT)50);
			break;
		case 71: 
			Console_String((CHAR*)"pointer expected as actual receiver", (LONGINT)36);
			break;
		case 72: 
			Console_String((CHAR*)"procedure must be bound to a record of the same scope", (LONGINT)54);
			break;
		case 73: 
			Console_String((CHAR*)"procedure must have level 0", (LONGINT)28);
			break;
		case 74: 
			Console_String((CHAR*)"procedure unknown in base type", (LONGINT)31);
			break;
		case 75: 
			Console_String((CHAR*)"invalid call of base procedure", (LONGINT)31);
			break;
		case 76: 
			Console_String((CHAR*)"this variable (field) is read only", (LONGINT)35);
			break;
		case 77: 
			Console_String((CHAR*)"object is not a record", (LONGINT)23);
			break;
		case 78: 
			Console_String((CHAR*)"dereferenced object is not a variable", (LONGINT)38);
			break;
		case 79: 
			Console_String((CHAR*)"indexed object is not a variable", (LONGINT)33);
			break;
		case 80: 
			Console_String((CHAR*)"index expression is not an integer", (LONGINT)35);
			break;
		case 81: 
			Console_String((CHAR*)"index out of specified bounds", (LONGINT)30);
			break;
		case 82: 
			Console_String((CHAR*)"indexed variable is not an array", (LONGINT)33);
			break;
		case 83: 
			Console_String((CHAR*)"undefined record field", (LONGINT)23);
			break;
		case 84: 
			Console_String((CHAR*)"dereferenced variable is not a pointer", (LONGINT)39);
			break;
		case 85: 
			Console_String((CHAR*)"guard or test type is not an extension of variable type", (LONGINT)56);
			break;
		case 86: 
			Console_String((CHAR*)"guard or testtype is not a pointer", (LONGINT)35);
			break;
		case 87: 
			Console_String((CHAR*)"guarded or tested variable is neither a pointer nor a VAR-parameter record", (LONGINT)75);
			break;
		case 88: 
			Console_String((CHAR*)"open array not allowed as variable, record field or array element", (LONGINT)66);
			break;
		case 92: 
			Console_String((CHAR*)"operand of IN not an integer, or not a set", (LONGINT)43);
			break;
		case 93: 
			Console_String((CHAR*)"set element type is not an integer", (LONGINT)35);
			break;
		case 94: 
			Console_String((CHAR*)"operand of & is not of type BOOLEAN", (LONGINT)36);
			break;
		case 95: 
			Console_String((CHAR*)"operand of OR is not of type BOOLEAN", (LONGINT)37);
			break;
		case 96: 
			Console_String((CHAR*)"operand not applicable to (unary) +", (LONGINT)36);
			break;
		case 97: 
			Console_String((CHAR*)"operand not applicable to (unary) -", (LONGINT)36);
			break;
		case 98: 
			Console_String((CHAR*)"operand of ~ is not of type BOOLEAN", (LONGINT)36);
			break;
		case 99: 
			Console_String((CHAR*)"ASSERT fault", (LONGINT)13);
			break;
		case 100: 
			Console_String((CHAR*)"incompatible operands of dyadic operator", (LONGINT)41);
			break;
		case 101: 
			Console_String((CHAR*)"operand type inapplicable to *", (LONGINT)31);
			break;
		case 102: 
			Console_String((CHAR*)"operand type inapplicable to /", (LONGINT)31);
			break;
		case 103: 
			Console_String((CHAR*)"operand type inapplicable to DIV", (LONGINT)33);
			break;
		case 104: 
			Console_String((CHAR*)"operand type inapplicable to MOD", (LONGINT)33);
			break;
		case 105: 
			Console_String((CHAR*)"operand type inapplicable to +", (LONGINT)31);
			break;
		case 106: 
			Console_String((CHAR*)"operand type inapplicable to -", (LONGINT)31);
			break;
		case 107: 
			Console_String((CHAR*)"operand type inapplicable to = or #", (LONGINT)36);
			break;
		case 108: 
			Console_String((CHAR*)"operand type inapplicable to relation", (LONGINT)38);
			break;
		case 109: 
			Console_String((CHAR*)"overriding method must be exported", (LONGINT)35);
			break;
		case 110: 
			Console_String((CHAR*)"operand is not a type", (LONGINT)22);
			break;
		case 111: 
			Console_String((CHAR*)"operand inapplicable to (this) function", (LONGINT)40);
			break;
		case 112: 
			Console_String((CHAR*)"operand is not a variable", (LONGINT)26);
			break;
		case 113: 
			Console_String((CHAR*)"incompatible assignment", (LONGINT)24);
			break;
		case 114: 
			Console_String((CHAR*)"string too long to be assigned", (LONGINT)31);
			break;
		case 115: 
			Console_String((CHAR*)"parameter doesn\'t match", (LONGINT)24);
			break;
		case 116: 
			Console_String((CHAR*)"number of parameters doesn\'t match", (LONGINT)35);
			break;
		case 117: 
			Console_String((CHAR*)"result type doesn\'t match", (LONGINT)26);
			break;
		case 118: 
			Console_String((CHAR*)"export mark doesn\'t match with forward declaration", (LONGINT)51);
			break;
		case 119: 
			Console_String((CHAR*)"redefinition textually precedes procedure bound to base type", (LONGINT)61);
			break;
		case 120: 
			Console_String((CHAR*)"type of expression following IF, WHILE, UNTIL or ASSERT is not BOOLEAN", (LONGINT)71);
			break;
		case 121: 
			Console_String((CHAR*)"called object is not a procedure (or is an interrupt procedure)", (LONGINT)64);
			break;
		case 122: 
			Console_String((CHAR*)"actual VAR-parameter is not a variable", (LONGINT)39);
			break;
		case 123: 
			Console_String((CHAR*)"type of actual parameter is not identical with that of formal VAR-parameter", (LONGINT)76);
			break;
		case 124: 
			Console_String((CHAR*)"type of result expression differs from that of procedure", (LONGINT)57);
			break;
		case 125: 
			Console_String((CHAR*)"type of case expression is neither INTEGER nor CHAR", (LONGINT)52);
			break;
		case 126: 
			Console_String((CHAR*)"this expression cannot be a type or a procedure", (LONGINT)48);
			break;
		case 127: 
			Console_String((CHAR*)"illegal use of object", (LONGINT)22);
			break;
		case 128: 
			Console_String((CHAR*)"unsatisfied forward reference", (LONGINT)30);
			break;
		case 129: 
			Console_String((CHAR*)"unsatisfied forward procedure", (LONGINT)30);
			break;
		case 130: 
			Console_String((CHAR*)"WITH clause does not specify a variable", (LONGINT)40);
			break;
		case 131: 
			Console_String((CHAR*)"LEN not applied to array", (LONGINT)25);
			break;
		case 132: 
			Console_String((CHAR*)"dimension in LEN too large or negative", (LONGINT)39);
			break;
		case 135: 
			Console_String((CHAR*)"SYSTEM not imported", (LONGINT)20);
			break;
		case 150: 
			Console_String((CHAR*)"key inconsistency of imported module", (LONGINT)37);
			break;
		case 151: 
			Console_String((CHAR*)"incorrect symbol file", (LONGINT)22);
			break;
		case 152: 
			Console_String((CHAR*)"symbol file of imported module not found", (LONGINT)41);
			break;
		case 153: 
			Console_String((CHAR*)"object or symbol file not opened (disk full\?)", (LONGINT)46);
			break;
		case 154: 
			Console_String((CHAR*)"recursive import not allowed", (LONGINT)29);
			break;
		case 155: 
			Console_String((CHAR*)"generation of new symbol file not allowed", (LONGINT)42);
			break;
		case 156: 
			Console_String((CHAR*)"parameter file not found", (LONGINT)25);
			break;
		case 157: 
			Console_String((CHAR*)"syntax error in parameter file", (LONGINT)31);
			break;
		case 200: 
			Console_String((CHAR*)"not yet implemented", (LONGINT)20);
			break;
		case 201: 
			Console_String((CHAR*)"lower bound of set range greater than higher bound", (LONGINT)51);
			break;
		case 202: 
			Console_String((CHAR*)"set element greater than MAX(SET) or less than 0", (LONGINT)49);
			break;
		case 203: 
			Console_String((CHAR*)"number too large", (LONGINT)17);
			break;
		case 204: 
			Console_String((CHAR*)"product too large", (LONGINT)18);
			break;
		case 205: 
			Console_String((CHAR*)"division by zero", (LONGINT)17);
			break;
		case 206: 
			Console_String((CHAR*)"sum too large", (LONGINT)14);
			break;
		case 207: 
			Console_String((CHAR*)"difference too large", (LONGINT)21);
			break;
		case 208: 
			Console_String((CHAR*)"overflow in arithmetic shift", (LONGINT)29);
			break;
		case 209: 
			Console_String((CHAR*)"case range too large", (LONGINT)21);
			break;
		case 213: 
			Console_String((CHAR*)"too many cases in case statement", (LONGINT)33);
			break;
		case 218: 
			Console_String((CHAR*)"illegal value of parameter  (0 <= p < 256)", (LONGINT)43);
			break;
		case 219: 
			Console_String((CHAR*)"machine registers cannot be accessed", (LONGINT)37);
			break;
		case 220: 
			Console_String((CHAR*)"illegal value of parameter", (LONGINT)27);
			break;
		case 221: 
			Console_String((CHAR*)"too many pointers in a record", (LONGINT)30);
			break;
		case 222: 
			Console_String((CHAR*)"too many global pointers", (LONGINT)25);
			break;
		case 223: 
			Console_String((CHAR*)"too many record types", (LONGINT)22);
			break;
		case 224: 
			Console_String((CHAR*)"too many pointer types", (LONGINT)23);
			break;
		case 225: 
			Console_String((CHAR*)"address of pointer variable too large (move forward in text)", (LONGINT)61);
			break;
		case 226: 
			Console_String((CHAR*)"too many exported procedures", (LONGINT)29);
			break;
		case 227: 
			Console_String((CHAR*)"too many imported modules", (LONGINT)26);
			break;
		case 228: 
			Console_String((CHAR*)"too many exported structures", (LONGINT)29);
			break;
		case 229: 
			Console_String((CHAR*)"too many nested records for import", (LONGINT)35);
			break;
		case 230: 
			Console_String((CHAR*)"too many constants (strings) in module", (LONGINT)39);
			break;
		case 231: 
			Console_String((CHAR*)"too many link table entries (external procedures)", (LONGINT)50);
			break;
		case 232: 
			Console_String((CHAR*)"too many commands in module", (LONGINT)28);
			break;
		case 233: 
			Console_String((CHAR*)"record extension hierarchy too high", (LONGINT)36);
			break;
		case 234: 
			Console_String((CHAR*)"export of recursive type not allowed", (LONGINT)37);
			break;
		case 240: 
			Console_String((CHAR*)"identifier too long", (LONGINT)20);
			break;
		case 241: 
			Console_String((CHAR*)"string too long", (LONGINT)16);
			break;
		case 242: 
			Console_String((CHAR*)"address overflow", (LONGINT)17);
			break;
		case 243: 
			Console_String((CHAR*)"concatenation of module, type, and guarded variable exceeds maximum name length", (LONGINT)80);
			break;
		case 244: 
			Console_String((CHAR*)"cyclic type definition not allowed", (LONGINT)35);
			break;
		case 301: 
			Console_String((CHAR*)"implicit type cast", (LONGINT)19);
			break;
		case 302: 
			Console_String((CHAR*)"guarded variable can be side-effected", (LONGINT)38);
			break;
		case 306: 
			Console_String((CHAR*)"inappropriate symbol file ignored", (LONGINT)34);
			break;
		default: __CASECHK;
	}
}


export void *OfrontErrors__init(void)
{
	__DEFMOD;
	__IMPORT(Console__init);
	__REGMOD("OfrontErrors", 0);
/* BEGIN */
	__ENDMOD;
}
