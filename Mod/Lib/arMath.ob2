(* Fast Math using math.h *)

MODULE [noinit] arMath;

	(* OMAKE LINK "m" *)

IMPORT 
	SYSTEM, 
	C := arC;

CONST
	PI* = 3.14159265358979323846264338327950288;
	E* = 2.71828182845904523536028747135266250;

PROCEDURE -includeMath- "#include <math.h>";

PROCEDURE -sin*(x : C.double) : C.double "sin((double)x)";
PROCEDURE -cos*(x : C.double) : C.double "cos((double)x)";
PROCEDURE -tan*(x : C.double) : C.double "tan((double)x)";
PROCEDURE -sinh*(x : C.double) : C.double "sinh((double)x)";
PROCEDURE -cosh*(x : C.double) : C.double "cosh((double)x)";
PROCEDURE -tanh*(x : C.double) : C.double "tanh((double)x)";

PROCEDURE -exp*(x : C.double) : C.double "exp((double)x)";
PROCEDURE -log*(x : C.double) : C.double "log((double)x)";
PROCEDURE -log2*(x : C.double) : C.double "log2((double)x)";
PROCEDURE -log10*(x : C.double) : C.double "log10((double)x)";
PROCEDURE -pow*(x, y : C.double) : C.double "pow((double)x, (double)y)";
PROCEDURE -fmod*(x, y : C.double) : C.double "fmod((double)x, (double)y)";

PROCEDURE -fabs*(x : C.double) : C.double "fabs((double)x)";
PROCEDURE -sqrt*(x : C.double) : C.double "sqrt((double)x)";
PROCEDURE -ceil*(x : C.double) : C.double "ceil((double)x)";
PROCEDURE -floor*(x : C.double) : C.double "floor((double)x)";
PROCEDURE -round*(x : C.double) : C.double "round((double)x)";
PROCEDURE -int*(x : C.double) : C.int "(int)(x)";
PROCEDURE -modf*(x : C.double; VAR i : C.double) : C.double "modf(x, i)";

PROCEDURE -sinf*(x : C.float) : C.float "sinf((float)x)";
PROCEDURE -cosf*(x : C.float) : C.float "cosf((float)x)";
PROCEDURE -tanf*(x : C.float) : C.float "tanf((float)x)";
PROCEDURE -sinhf*(x : C.float) : C.float "sinhf((float)x)";
PROCEDURE -coshf*(x : C.float) : C.float "coshf((float)x)";
PROCEDURE -tanhf*(x : C.float) : C.float "tanhf((float)x)";

PROCEDURE -expf*(x : C.float) : C.float "expf((float)x)";
PROCEDURE -logf*(x : C.float) : C.float "logf((float)x)";
PROCEDURE -log2f*(x : C.float) : C.float "log2f((float)x)";
PROCEDURE -log10f*(x : C.float) : C.float "log10f((float)x)";
PROCEDURE -powf*(x, y : C.float) : C.float "powf((float)x, (float)y)";

PROCEDURE -fabsf*(x : C.float) : C.float "fabsf((float)x)";
PROCEDURE -sqrtf*(x : C.float) : C.float "sqrtf((float)x)";
PROCEDURE -ceilf*(x : C.float) : C.float "ceilf((float)x)";
PROCEDURE -floorf*(x : C.float) : C.float "floorf((float)x)";
PROCEDURE -roundf*(x : C.float) : C.float "roundf((float)x)";
PROCEDURE -fmodf*(x, y : C.float) : C.float "fmod((float)x, (float)y)";

END arMath.
