/* Ofront 1.2 -xtpkaml */
#include "SYSTEM.h"
#include "Kernel.h"
#include "ooc2RandomNumbers.h"
#include "Console.h"


static INTEGER TestOoc2RandomNumbers_i;





export main(int argc, char **argv)
{
	__INIT(argc, argv);
	__IMPORT(Kernel__init);
	__IMPORT(ooc2RandomNumbers__init);
	__IMPORT(Console__init);
	__REGMAIN("TestOoc2RandomNumbers", 0);
/* BEGIN */
	ooc2RandomNumbers_PutSeed((int)Kernel_Time());
	TestOoc2RandomNumbers_i = 1;
	while (TestOoc2RandomNumbers_i <= 100) {
		Console_Int(ooc2RandomNumbers_RND(10), 2);
		TestOoc2RandomNumbers_i += 1;
	}
	Console_Ln();
	__FINI;
}
