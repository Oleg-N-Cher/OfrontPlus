(* Utility functions to handle C strings *)

MODULE arCString;

IMPORT SYSTEM, C := arC;

TYPE
	String* = POINTER TO ARRAY OF CHAR;

PROCEDURE -includeString* "#include <string.h>";

PROCEDURE -strlen(argStr : C.string) : C.int
	"strlen((const char *)argStr)";

PROCEDURE -strncpy(dst, src : C.string; len : C.int) : C.string
	"strncpy((char *)dst, (const char *)src, len)";

(* Copy a C string into an ARRAY OF CHAR *)

PROCEDURE CopyToArray*(src : C.string; VAR dst : ARRAY OF CHAR);
VAR
	dummy : C.string;
BEGIN
	dummy := strncpy(SYSTEM.VAL(C.string, SYSTEM.ADR(dst)), src, LEN(dst));
END CopyToArray;

(* Copy a C string into an array that is dynamically allocated to fit *)

PROCEDURE Copy*(src : C.string) : String;
VAR
	result : String;
	i, len : LONGINT;
BEGIN
	len := strlen(src);
	NEW(result, len+1);
	CopyToArray(src, result^);
	RETURN result;
END Copy;
	
END arCString.
