/*  Ofront 1.2 -xtspkaem */
#include "SYSTEM.h"
#include "CmdlnTexts.h"
#include "Watson0.h"




static void WatsonCmd_ShowDef (void);


/*============================================================================*/

static void WatsonCmd_ShowDef (void)
{
	CmdlnTexts_Text T = NIL;
	CHAR mod[32];
	__MOVE("Watson0", mod, 8);
	__NEW(T, CmdlnTexts_TextDesc);
	CmdlnTexts_Open(T, (CHAR*)"", (LONGINT)1);
	Watson0_ShowDef((void*)mod, 32, T);
}


int main(int argc, char **argv)
{
	__INIT(argc, argv);
	__IMPORT(CmdlnTexts__init);
	__IMPORT(Watson0__init);
	__REGMAIN("WatsonCmd", 0);
/* BEGIN */
	WatsonCmd_ShowDef();
	__FINI;
}
