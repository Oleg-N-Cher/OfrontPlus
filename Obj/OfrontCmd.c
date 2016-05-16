/*  Ofront 1.2 -xtspkaem */
#include "SYSTEM.h"
#include "Kernel.h"
#include "OfrontOPB.h"
#include "OfrontOPC.h"
#include "OfrontOPM.h"
#include "OfrontOPP.h"
#include "OfrontOPT.h"
#include "OfrontOPV.h"
#include "Unix.h"




export void OfrontCmd_Module (BOOLEAN *done);
export void OfrontCmd_Translate (void);
static void OfrontCmd_Trap (LONGINT sig, LONGINT code, Unix_SigCtxPtr scp);

#define OfrontCmd_fin()	SYSTEM_FINALL()
#define OfrontCmd_halt()	SYSTEM_halt
#define OfrontCmd_signal(sig, func)	signal(sig, func)

/*============================================================================*/

static void OfrontCmd_Trap (LONGINT sig, LONGINT code, Unix_SigCtxPtr scp)
{
	OfrontCmd_fin();
	if (sig == 3) {
		Unix_Exit(0);
	} else {
		if (sig == 4 && OfrontCmd_halt() == -15) {
			OfrontOPM_LogWStr((CHAR*)" --- ofront: internal error", (LONGINT)28);
			OfrontOPM_LogWLn();
		}
		Unix_Exit(2);
	}
}

void OfrontCmd_Module (BOOLEAN *done)
{
	BOOLEAN ext, new;
	OfrontOPT_Node p = NIL;
	OfrontOPP_Module(&p, OfrontOPM_opt);
	if (OfrontOPM_noerr) {
		OfrontOPV_Init();
		OfrontOPV_AdrAndSize(OfrontOPT_topScope);
		OfrontOPT_Export(&ext, &new);
		if (OfrontOPM_noerr) {
			OfrontOPM_OpenFiles((void*)OfrontOPT_SelfName, 32);
			OfrontOPC_Init();
			OfrontOPV_Module(p);
			if (OfrontOPM_noerr) {
				if (__IN(10, OfrontOPM_opt) && __STRCMP(OfrontOPM_modName, "SYSTEM") != 0) {
					OfrontOPM_DeleteNewSym();
					OfrontOPM_LogWStr((CHAR*)"  main program", (LONGINT)15);
				} else {
					if (new) {
						OfrontOPM_LogWStr((CHAR*)"  new symbol file", (LONGINT)18);
						OfrontOPM_RegisterNewSym();
					} else if (ext) {
						OfrontOPM_LogWStr((CHAR*)"  extended symbol file", (LONGINT)23);
						OfrontOPM_RegisterNewSym();
					}
				}
			} else {
				OfrontOPM_DeleteNewSym();
			}
		}
	}
	OfrontOPM_CloseFiles();
	OfrontOPT_Close();
	OfrontOPM_LogWLn();
	*done = OfrontOPM_noerr;
}

/*----------------------------------------------------------------------------*/
void OfrontCmd_Translate (void)
{
	BOOLEAN done;
	OfrontOPM_OpenPar();
	OfrontOPT_bytetyp->size = OfrontOPM_ByteSize;
	OfrontOPT_sysptrtyp->size = OfrontOPM_PointerSize;
	OfrontOPT_chartyp->size = OfrontOPM_CharSize;
	OfrontOPT_settyp->size = OfrontOPM_SetSize;
	OfrontOPT_realtyp->size = OfrontOPM_RealSize;
	OfrontOPT_inttyp->size = OfrontOPM_IntSize;
	OfrontOPT_linttyp->size = OfrontOPM_LIntSize;
	OfrontOPT_lrltyp->size = OfrontOPM_LRealSize;
	OfrontOPT_sinttyp->size = OfrontOPM_SIntSize;
	OfrontOPT_booltyp->size = OfrontOPM_BoolSize;
	for (;;) {
		OfrontOPM_Init(&done);
		if (!done) {
			break;
		}
		OfrontOPM_InitOptions();
		Kernel_GC(0);
		OfrontCmd_Module(&done);
		if (!done) {
			Unix_Exit(1);
		}
	}
}

/*----------------------------------------------------------------------------*/

export main(int argc, char **argv)
{
	__INIT(argc, argv);
	__IMPORT(Kernel__init);
	__IMPORT(OfrontOPB__init);
	__IMPORT(OfrontOPC__init);
	__IMPORT(OfrontOPM__init);
	__IMPORT(OfrontOPP__init);
	__IMPORT(OfrontOPT__init);
	__IMPORT(OfrontOPV__init);
	__IMPORT(Unix__init);
	__REGMAIN("OfrontCmd", 0);
	__REGCMD("Translate", OfrontCmd_Translate);
/* BEGIN */
	OfrontCmd_signal(2, OfrontCmd_Trap);
	OfrontCmd_signal(3, OfrontCmd_Trap);
	OfrontCmd_signal(4, OfrontCmd_Trap);
	OfrontOPB_typSize = OfrontOPV_TypSize;
	OfrontOPT_typSize = OfrontOPV_TypSize;
	OfrontCmd_Translate();
	__FINI;
}
