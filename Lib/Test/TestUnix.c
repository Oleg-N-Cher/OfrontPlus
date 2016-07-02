/* Ofront 1.2 -xtpkaml */
#include "SYSTEM.h"
#include "Unix.h"
#include "Console.h"





#include <sys/stat.h>
#include <sys/time.h>
#include "_winsock.h"
#define TestUnix_fdsetSIZE()	sizeof(struct fd_set)
#define TestUnix_statSIZE()	sizeof(struct stat)
#define TestUnix_timevalSIZE()	sizeof(struct timeval)
#define TestUnix_timezoneSIZE()	sizeof(struct timezone)


export main(int argc, char **argv)
{
	__INIT(argc, argv);
	__IMPORT(Unix__init);
	__IMPORT(Console__init);
	__REGMAIN("TestUnix", 0);
/* BEGIN */
	if (36 != TestUnix_statSIZE()) {
		Console_String((CHAR*)"SIZE(Unix.Status) = ", (LONGINT)21);
		Console_Int(36, 0);
		Console_String((CHAR*)"; sizeof(struct stat) = ", (LONGINT)25);
		Console_Int(TestUnix_statSIZE(), 0);
		Console_Ln();
	}
	if (8 != TestUnix_timevalSIZE()) {
		Console_String((CHAR*)"SIZE(Unix.Timeval) = ", (LONGINT)22);
		Console_Int(8, 0);
		Console_String((CHAR*)"; sizeof(struct timeval) = ", (LONGINT)28);
		Console_Int(TestUnix_timevalSIZE(), 0);
		Console_Ln();
	}
	if (8 != TestUnix_timezoneSIZE()) {
		Console_String((CHAR*)"SIZE(Unix.Timezone) = ", (LONGINT)23);
		Console_Int(8, 0);
		Console_String((CHAR*)"; sizeof(struct timezone) = ", (LONGINT)29);
		Console_Int(TestUnix_timezoneSIZE(), 0);
		Console_Ln();
	}
	if (32 != TestUnix_fdsetSIZE()) {
		Console_String((CHAR*)"SIZE(Unix.FdSet) = ", (LONGINT)20);
		Console_Int(32, 0);
		Console_String((CHAR*)"; sizeof(struct fd_set) = ", (LONGINT)27);
		Console_Int(TestUnix_fdsetSIZE(), 0);
		Console_Ln();
	}
	Console_Flush();
	__FINI;
}
