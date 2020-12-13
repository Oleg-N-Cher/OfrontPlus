(* Definitions for C types - Should be available in SYSTEM? *)

MODULE arC;

IMPORT SYSTEM;

TYPE
	char* = SYSTEM.CHAR8;
	uchar* = SYSTEM.UINT8;

	string* = POINTER [notag] TO ARRAY [notag] OF char;

	short* = SYSTEM.INT16;
	int* = SYSTEM.INT32;
	longint* = SYSTEM.INT64;

	float* = SYSTEM.REAL32;
	double* = SYSTEM.REAL64;

	enum* = int;
	set* = int;
	address* = SYSTEM.ADRINT;
	pointer* = SYSTEM.PTR;

END arC.
