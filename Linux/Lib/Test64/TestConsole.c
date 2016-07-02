/* Ofront 1.2 -xtpkaml */
#include "SYSTEM.h"
#include "Console.h"


static INTEGER TestConsole_int;
static LONGINT TestConsole_long;





export main(int argc, char **argv)
{
	__INIT(argc, argv);
	__IMPORT(Console__init);
	__REGMAIN("TestConsole", 0);
/* BEGIN */
	Console_LongInt(0, 0);
	Console_Char(' ');
	Console_LongHex(0);
	Console_Ln();
	TestConsole_int = (-2147483647-1);
	Console_String((CHAR*)"MIN(INTEGER)=", (LONGINT)14);
	Console_Int(TestConsole_int, 0);
	Console_Char(' ');
	Console_Hex(TestConsole_int);
	Console_Char(' ');
	TestConsole_int += 1;
	Console_Int(TestConsole_int, 0);
	Console_Char(' ');
	Console_Hex(TestConsole_int);
	Console_Ln();
	TestConsole_int = 2147483647;
	Console_String((CHAR*)"MAX(INTEGER)=", (LONGINT)14);
	Console_Int(TestConsole_int, 0);
	Console_Char(' ');
	Console_Hex(TestConsole_int);
	Console_Char(' ');
	TestConsole_int -= 1;
	Console_Int(TestConsole_int, 0);
	Console_Char(' ');
	Console_Hex(TestConsole_int);
	Console_Ln();
	TestConsole_long = (-2147483647-1);
	Console_String((CHAR*)"MIN(LONGINT)=", (LONGINT)14);
	Console_LongInt(TestConsole_long, 0);
	Console_Char(' ');
	Console_LongHex(TestConsole_long);
	Console_Char(' ');
	TestConsole_long += 1;
	Console_LongInt(TestConsole_long, 0);
	Console_Char(' ');
	Console_LongHex(TestConsole_long);
	Console_Ln();
	TestConsole_long = 2147483647;
	Console_String((CHAR*)"MAX(LONGINT)=", (LONGINT)14);
	Console_LongInt(TestConsole_long, 0);
	Console_Char(' ');
	Console_LongHex(TestConsole_long);
	Console_Char(' ');
	TestConsole_long -= 1;
	Console_LongInt(TestConsole_long, 0);
	Console_Char(' ');
	Console_LongHex(TestConsole_long);
	Console_Ln();
	__FINI;
}
