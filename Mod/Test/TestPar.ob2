MODULE TestPar; IMPORT SYSTEM;

PROCEDURE ReadBuf*(VAR b: ARRAY OF SYSTEM.BYTE);
BEGIN

END ReadBuf;


   PROCEDURE Read*(VAR ch: CHAR);
   BEGIN
    ReadBuf(SYSTEM.VAL(SYSTEM.BYTE, ch));
   END Read;

  PROCEDURE Unpack(VAR d: ARRAY OF SYSTEM.BYTE);
  END Unpack;
  
  PROCEDURE ConvertH* (VAR d: ARRAY OF CHAR);
  BEGIN Unpack(d)
  END ConvertH;
  
END TestPar.
