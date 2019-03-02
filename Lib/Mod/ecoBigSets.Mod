(***************************************************************************

     $RCSfile: BigSets.mod $
  Description: An implementation of sets bigger than a machine word.

   Created by: fjc (Frank Copeland)
    $Revision: 1.2 $
      $Author: fjc $
        $Date: 1994/05/12 20:08:14 $

  Copyright © 1994, Frank Copeland.
  This file is part of the Oberon-A Library.
  See Oberon-A.doc for conditions of use and distribution.

  Log entries are at the end of the file.

  ftp://ftp.uni-stuttgart.de/pub/systems/amiga/amok/amok104/Oberon-A.lha

***************************************************************************)

MODULE EcoBigSets;

(* $L+ use absolute long addressing for global variables *)

CONST

  BitsPerSet * = MAX (SET) + 1;
  CharSetElements = ORD (MAX (SHORTCHAR)) DIV BitsPerSet;

TYPE

  CHARSET * = ARRAY CharSetElements OF SET;

(*------------------------------------*)
PROCEDURE Empty * ( VAR set : ARRAY OF SET );

  VAR index : INTEGER;

BEGIN (* Empty *)
  index := 0;
  WHILE index < LEN (set) DO
    set [index] := {};
    INC (index)
  END; (* WHILE *)
END Empty;

(*------------------------------------*)
PROCEDURE IsEmpty * ( VAR set : ARRAY OF SET ) : BOOLEAN;

  VAR index : INTEGER; empty : BOOLEAN;

BEGIN (* IsEmpty *)
  empty := TRUE; index := 0;
  WHILE empty & (index < LEN (set)) DO
    empty := (set [index] = {});
    INC( index );
  END; (* WHILE *)
  RETURN empty;
END IsEmpty;

(*------------------------------------*)
PROCEDURE In * ( VAR set : ARRAY OF SET; element : INTEGER ) : BOOLEAN;

  VAR index, bit : INTEGER;

BEGIN (* In *)
  index := element DIV BitsPerSet;
  bit := element MOD BitsPerSet;
  RETURN (bit IN set [index]);
END In;

(*------------------------------------*)
PROCEDURE Incl * ( VAR set : ARRAY OF SET; element : INTEGER );

  VAR index, bit : INTEGER;

BEGIN (* Incl *)
  index := element DIV BitsPerSet;
  bit := element MOD BitsPerSet;
  INCL (set [index], bit);
END Incl;

(*------------------------------------*)
PROCEDURE Excl * ( VAR set : ARRAY OF SET; element : INTEGER );

  VAR index, bit : INTEGER;

BEGIN (* Excl *)
  index := element DIV BitsPerSet;
  bit := element MOD BitsPerSet;
  EXCL (set [index], bit);
END Excl;

(*------------------------------------*)
PROCEDURE InclRange * (
  VAR set : ARRAY OF SET; firstElement, lastElement : INTEGER );

  VAR index, bit, count : INTEGER;

BEGIN (* InclRange *)
  index := firstElement DIV BitsPerSet;
  bit := firstElement MOD BitsPerSet;
  count := lastElement - firstElement + 1;
  WHILE count > 0 DO
    INCL (set [index], bit);
    INC (bit);
    IF bit = BitsPerSet THEN
      bit := 0;
      INC (index);
    END; (* IF *)
    DEC (count);
  END; (* WHILE *)
END InclRange;

(*------------------------------------*)
PROCEDURE ExclRange * (
  VAR set : ARRAY OF SET; firstElement, lastElement : INTEGER );

  VAR index, bit, count : INTEGER;

BEGIN (* ExclRange *)
  index := firstElement DIV BitsPerSet;
  bit := firstElement MOD BitsPerSet;
  count := lastElement - firstElement + 1;
  WHILE count > 0 DO
    EXCL (set [index], bit);
    INC (bit);
    IF bit = BitsPerSet THEN
      bit := 0;
      INC (index);
    END; (* IF *)
    DEC (count);
  END; (* WHILE *)
END ExclRange;

(*------------------------------------*)
PROCEDURE Union * ( VAR firstSet, secondSet, destSet : ARRAY OF SET );

  VAR index, maxIndex : INTEGER;

BEGIN (* Union *)
  index := 0; maxIndex := SHORT (LEN (firstSet));
  WHILE index < maxIndex DO
    destSet [index] := firstSet [index] + secondSet [index];
    INC (index)
  END; (* WHILE *)
END Union;

(*------------------------------------*)
PROCEDURE Difference * ( VAR firstSet, secondSet, destSet : ARRAY OF SET );

  VAR index, maxIndex : INTEGER;

BEGIN (* Difference *)
  index := 0; maxIndex := SHORT (LEN (firstSet));
  WHILE index < maxIndex DO
    destSet [index] := firstSet [index] - secondSet [index];
    INC (index)
  END; (* WHILE *)
END Difference;

(*------------------------------------*)
PROCEDURE Intersection * (VAR firstSet, secondSet, destSet : ARRAY OF SET);

  VAR index, maxIndex : INTEGER;

BEGIN (* Intersection *)
  index := 0; maxIndex := SHORT (LEN (firstSet));
  WHILE index < maxIndex DO
    destSet [index] := firstSet [index] * secondSet [index];
    INC (index)
  END; (* WHILE *)
END Intersection;

(*------------------------------------*)
PROCEDURE SymmetricDiff * (VAR firstSet, secondSet, destSet : ARRAY OF SET);

  VAR index, maxIndex : INTEGER;

BEGIN (* SymmetricDiff *)
  index := 0; maxIndex := SHORT (LEN (firstSet));
  WHILE index < maxIndex DO
    destSet [index] := firstSet [index] / secondSet [index];
    INC (index)
  END; (* WHILE *)
END SymmetricDiff;

END EcoBigSets.

(***************************************************************************

  $Log: BigSets.mod $
  Revision 1.2  1994/05/12  20:08:14  fjc
  - Prepared for release

  Revision 1.1  1994/01/15  21:39:12  fjc
  - Start of revision control

***************************************************************************)

