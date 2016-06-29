/* Ofront 1.2 -xtpkaml */
#include "SYSTEM.h"
#include "Args.h"
#include "Console.h"


static LONGINT TestArgs_i;
static SHORTINT TestArgs_n;
static CHAR TestArgs_arg[1024], TestArgs_var[1024];
static SHORTINT TestArgs__for__2;
static LONGINT TestArgs__for__1;


static LONGINT TestArgs_Length (CHAR *str, LONGINT str__len);


static LONGINT TestArgs_Length (CHAR *str, LONGINT str__len)
{
	LONGINT len;
	len = 0;
	while (len < (LONGINT)((int)str__len) && str[__X(len, str__len)] != 0x00) {
		len += 1;
	}
	return len;
}


export main(int argc, char **argv)
{
	__INIT(argc, argv);
	__IMPORT(Args__init);
	__IMPORT(Console__init);
	__REGMAIN("TestArgs", 0);
/* BEGIN */
	Console_String((CHAR*)"Args.argc = ", (LONGINT)13);
	Console_Int(Args_argc, 0);
	Console_Ln();
	Console_Ln();
	TestArgs__for__2 = (int)(Args_argc - 1);
	TestArgs_n = 1;
	while (TestArgs_n <= TestArgs__for__2) {
		Args_Get(TestArgs_n, (void*)TestArgs_arg, 1024);
		Console_Int(TestArgs_n, 0);
		Console_Char(':');
		Console_String(TestArgs_arg, 1024);
		Console_Ln();
		TestArgs__for__1 = (int)TestArgs_Length((void*)TestArgs_arg, 1024) - 1;
		TestArgs_i = 0;
		while (TestArgs_i <= TestArgs__for__1) {
			Console_Int((int)TestArgs_arg[__X(TestArgs_i, 1024)], 0);
			Console_Char(',');
			TestArgs_i += 1;
		}
		Console_Ln();
		TestArgs_n += 1;
	}
	Args_GetEnv((CHAR*)"OBERON", (LONGINT)7, (void*)TestArgs_var, 1024);
	Console_Ln();
	if (TestArgs_var[0] != 0x00) {
		Console_String((CHAR*)"OBERON=", (LONGINT)8);
		Console_String(TestArgs_var, 1024);
		Console_Ln();
	}
	TestArgs_var[0] = 0x00;
	Args_GetEnv((CHAR*)"PATH", (LONGINT)5, (void*)TestArgs_var, 1024);
	Console_Ln();
	if (TestArgs_var[0] != 0x00) {
		Console_String((CHAR*)"PATH=", (LONGINT)6);
		Console_String(TestArgs_var, 1024);
		Console_Ln();
	}
	TestArgs_var[0] = 0x00;
	Args_GetEnv((CHAR*)"HOME", (LONGINT)5, (void*)TestArgs_var, 1024);
	Console_Ln();
	if (TestArgs_var[0] != 0x00) {
		Console_String((CHAR*)"HOME=", (LONGINT)6);
		Console_String(TestArgs_var, 1024);
		Console_Ln();
	}
	TestArgs_var[0] = 0x00;
	__FINI;
} 
