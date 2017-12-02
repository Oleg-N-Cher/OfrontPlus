MODULE Math;  (* jt, 10.1.95 *)

IMPORT SYSTEM;

CONST
  Pi* = 3.1415926535;
  E* = 2.7182818285;
  Modulo = 2147483647; (* =2^31-1 *)

VAR
  z: INTEGER;

PROCEDURE -AAincludemath	"#include <math.h>"; (* define the proper return types! *)
PROCEDURE -AAincludewindows0	"#if defined(WIN32) || defined(_WIN32)";
PROCEDURE -AAincludewindows1	'#  include "_windows.h"';
PROCEDURE -AAincludewindows2	"#  define __Math_Randomize__ WinApi_GetTickCount()";
PROCEDURE -AAincludewindows3	"#else";
PROCEDURE -AAincludewindows4	"#  include <time.h>";
PROCEDURE -AAincludewindows5	"#  define __Math_Randomize__ time(0)";
PROCEDURE -AAincludewindows6	"#endif";

PROCEDURE -randomize (): INTEGER	"__Math_Randomize__";

PROCEDURE -sin (x: REAL): REAL	"(sin(x))";
PROCEDURE -cos (x: REAL): REAL	"(cos(x))";
PROCEDURE -arctan (x: REAL): REAL	"(atan(x))";
PROCEDURE -exp (x: REAL): REAL	"(exp(x))";
PROCEDURE -sqrt (x: REAL): REAL	"(sqrt(x))";
PROCEDURE -ln (x: REAL): REAL	"(log(x))";

PROCEDURE Sin* (x: REAL): REAL;
BEGIN RETURN sin(x)
END Sin;

PROCEDURE Cos* (x: REAL): REAL;
BEGIN RETURN cos(x)
END Cos;

PROCEDURE ArcTan* (x: REAL): REAL;
BEGIN RETURN arctan(x)
END ArcTan;

PROCEDURE Exp* (x: REAL): REAL;
BEGIN RETURN exp(x)
END Exp;

PROCEDURE Sqrt* (x: REAL): REAL;
BEGIN RETURN sqrt(x)
END Sqrt;

PROCEDURE Ln* (x: REAL): REAL;
BEGIN RETURN ln(x)
END Ln;

PROCEDURE -getTickCount (): INTEGER	"GetTickCount()";

(* 
For details on this random algorithm take a look at
  Park S.K. and Miller K.W. (1988). Random number generators, good ones are
  hard to find. Communications of the ACM, 31, 1192-1201.
*)
PROCEDURE PutSeed (seed: INTEGER);
(* Set 'seed' as the new seed value. Any values for 'seed' are allowed, but
   values beyond the intervall [1..2^31-2] will be mapped into this range *)
BEGIN
  seed := seed MOD Modulo;
  IF (seed = 0) THEN
    z := 1
  ELSE
    z := seed
  END;
END PutSeed;

PROCEDURE NextRnd;
CONST
  A = 16807;
  Q = 127773; (* m DIV a *)
  R = 2836; (* m MOD a *)
VAR
  lo, hi, test: INTEGER;
BEGIN
  hi := z DIV Q;
  lo := z MOD Q;
  test := A * lo - R * hi;
  IF (test > 0) THEN
    z := test;
  ELSE
    z := test + Modulo;
  END;
END NextRnd;

(* Returns a random number within the range rangeMin to rangeMax inclusively. *)
PROCEDURE RndRange* (rangeMin, rangeMax: INTEGER): INTEGER;
BEGIN
  ASSERT(rangeMin <= rangeMax);
  NextRnd; RETURN ((z + getTickCount()) MOD (rangeMax - rangeMin + 1)) + rangeMin
END RndRange;

PROCEDURE Randomize* ; (* Initialize random generator *)
BEGIN
  PutSeed(randomize());
END Randomize;

BEGIN
  z := 1; Randomize;
END Math.
