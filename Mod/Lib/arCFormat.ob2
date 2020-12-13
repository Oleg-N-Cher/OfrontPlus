(* Conversions to and from strings for integers and reals. This is done using
the C standard sscanf and snprintf functions defined in stdio.h. Note that
snprintf is safe, since we pass the array size. We can't check if the
format matches the argument, so use with caution. However, the C compiler
should emit a warning if the wrong format is used. *)

MODULE arCFormat;

IMPORT 
	SYSTEM, 
	C := arC;

PROCEDURE -includeStdio* "#include <stdio.h>";

PROCEDURE -IntToString*(VAR result : ARRAY OF CHAR; format : ARRAY OF CHAR; value : C.int)
	"snprintf(result, result__len, (const char *)format, value)";

PROCEDURE -RealToString*(VAR result : ARRAY OF CHAR; format : ARRAY OF CHAR; value : C.double)
	"snprintf(result, result__len, (const char *)format, value)";

PROCEDURE -StringToString*(VAR result : ARRAY OF CHAR; format : ARRAY OF CHAR; value : ARRAY OF CHAR)
	"snprintf(result, result__len, (const char *)format, (const char *)value)";

PROCEDURE -StringToInt*(str : ARRAY OF CHAR; VAR value : C.int) : C.int
	'sscanf((const char *)str, "%d", value)';

PROCEDURE -StringToReal*(str : ARRAY OF CHAR; VAR value : C.double) : C.int
	'sscanf((const char *)str, "%lf", value)';

END arCFormat.
