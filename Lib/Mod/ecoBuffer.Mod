MODULE EcoBuffer; (*cyclic buffer of integers*)
 CONST N = 100;
 VAR nonempty-, nonfull-: BOOLEAN;
   in, out, n: INTEGER;
   buf: ARRAY N OF INTEGER;
  PROCEDURE put*(x: INTEGER);
  BEGIN
   IF n < N THEN
      buf[in] := x; in := (in+1) MOD N;
      INC(n); nonfull := n < N; nonempty := TRUE
   END
  END put;
  PROCEDURE get*(VAR x: INTEGER);
  BEGIN
   IF n > 0 THEN
      x := buf[out]; out := (out+1) MOD N;
      DEC(n); nonempty := n > 0; nonfull := TRUE
   END
  END get;
BEGIN (*initialize*) n := 0; in := 0; out := 0;
 nonempty := FALSE; nonfull := TRUE
END EcoBuffer.
