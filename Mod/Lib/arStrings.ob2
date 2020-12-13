(* Assorted functions for strings represented in CHAR ARRAYs. This includes
some functions from ooc2Strings. *)

MODULE arStrings;

TYPE
	String* = ARRAY OF CHAR;
	StringPtr* = POINTER TO String;
	LengthType* = LONGINT;

PROCEDURE Length* (stringVal-: String): LengthType;
(* Returns the length of `stringVal'.  This is equal to the number of 
   characters in `stringVal' up to and excluding the first 0X.  *)
  VAR
    i: LengthType;
  BEGIN
    i := 0;
    WHILE (stringVal[i] # 0X) DO
      INC (i)
    END;
    RETURN i
  END Length;

PROCEDURE Equal* (stringVal1-, stringVal2-: String): BOOLEAN;
(* Returns `stringVal1 = stringVal2'.  Unlike the predefined operator `=', this
   procedure can be assigned to a procedure variable. *)
  VAR
    i: LengthType;
  BEGIN
    i := 0;
    WHILE (stringVal1[i] # 0X) & (stringVal1[i] = stringVal2[i]) DO
      INC (i)
    END;
    RETURN (stringVal1[i] = 0X) & (stringVal2[i] = 0X)
  END Equal;
 
PROCEDURE Assign* (source-: String; VAR destination: String);
(* Copies `source' to `destination'.  Equivalent to the predefined procedure 
   COPY.  Unlike COPY, this procedure can be assigned to a procedure 
   variable.  *)
  VAR
    i: LengthType;
  BEGIN
    i := -1;
    REPEAT
      INC (i);
      destination[i] := source[i]
    UNTIL (destination[i] = 0X) OR (i = LEN (destination)-1);
    destination[i] := 0X
  END Assign;
  
PROCEDURE Extract* (source-: String; startPos, numberToExtract: LengthType;
                    VAR destination: String);
(* Copies at most `numberToExtract' characters from `source' to `destination',
   starting at position `startPos' in `source'.  An empty string value will be
   extracted if `startPos' is greater than or equal to `Length(source)'.  
   pre: `startPos' and `numberToExtract' are not negative.  *)
  VAR
    sourceLength, i: LengthType;
  BEGIN
    (* make sure that we get an empty string if `startPos' refers to an array
       index beyond `Length (source)'  *)
    sourceLength := Length (source);
    IF (startPos > sourceLength) THEN
      startPos := sourceLength
    END;
    
    (* make sure that `numberToExtract' doesn't exceed the capacity 
       of `destination' *)
    IF (numberToExtract >= LEN (destination)) THEN
      numberToExtract := LEN (destination)-1
    END;
    
    (* copy up to `numberToExtract' characters to `destination' *)
    i := 0;
    WHILE (i < numberToExtract) & (source[startPos+i] # 0X) DO
      destination[i] := source[startPos+i];
      INC (i)
    END;
    destination[i] := 0X
  END Extract;

PROCEDURE Append* (source-: String; VAR destination: String);
(* Appends source to destination. *)
  VAR
    destLength, i: LengthType;
  BEGIN
    destLength := Length (destination);
    i := 0;
    WHILE (destLength < LEN (destination)-1) & (source[i] # 0X) DO
      destination[destLength] := source[i];
      INC (destLength);
      INC (i)
    END;
    destination[destLength] := 0X
  END Append;

PROCEDURE Compare* (stringVal1-, stringVal2-: String): SHORTINT;
(* Returns `less', `equal', or `greater', according as `stringVal1' is 
   lexically less than, equal to, or greater than `stringVal2'.
   Note that Oberon-2 already contains predefined comparison operators on 
   strings.  *)
  VAR
    i: LengthType;
  BEGIN
    i := 0;
    WHILE (stringVal1[i] # 0X) & (stringVal1[i] = stringVal2[i]) DO
      INC (i)
    END;
    IF (stringVal1[i] < stringVal2[i]) THEN
      RETURN -1 
    ELSIF (stringVal1[i] > stringVal2[i]) THEN
      RETURN 1
    ELSE
      RETURN 0
    END
  END Compare;

PROCEDURE StartsWith*(str-, prefix- : String) : BOOLEAN;
VAR lenStr, lenPrefix, i  : LengthType;
BEGIN
	lenStr := Length(str);
	lenPrefix := Length(prefix);
	IF lenPrefix > lenStr THEN RETURN FALSE END;
	FOR i := 0 TO lenPrefix-1 DO
		IF prefix[i] # str[i] THEN RETURN FALSE END;
	END;
	RETURN TRUE;
END StartsWith;

PROCEDURE EndsWith*(str-, suffix- : String) : BOOLEAN;
VAR lenStr, lenSuffix, i  : LengthType;
BEGIN
	lenStr := Length(str);
	lenSuffix := Length(suffix);
	IF lenSuffix > lenStr THEN RETURN FALSE END;
	FOR i := 0 TO lenSuffix-1 DO
		IF suffix[i] # str[lenStr-lenSuffix+i] THEN RETURN FALSE END;
	END;
	RETURN TRUE;
END EndsWith;

PROCEDURE IndexOf*(str- : String; ch : CHAR; pos : LengthType) : LengthType;
VAR length : LengthType;
BEGIN
	length := Length(str);
	IF length = 0 THEN RETURN -1 END;
	IF pos < 0 THEN pos := length + pos END;
	IF (pos < 0) OR (pos>=length) THEN RETURN -1 END;
	WHILE pos < length DO
		IF str[pos] = ch THEN RETURN pos END;
		INC(pos);
	END;
	RETURN -1;
END IndexOf;

PROCEDURE LastIndexOf*(str- : String; ch : CHAR; pos : LengthType) : LengthType;
VAR length : LengthType;
BEGIN
	length := Length(str);
	IF length = 0 THEN RETURN -1 END;
	IF pos < 0 THEN pos := length + pos END;
	IF (pos < 0) OR (pos>=length) THEN RETURN -1 END;
	WHILE pos >= 0 DO
		IF str[pos] = ch THEN RETURN pos END;
		DEC(pos);
	END;
	RETURN -1;
END LastIndexOf;

PROCEDURE Copy*(value- : String) : StringPtr;
VAR length : LengthType; result : StringPtr;
BEGIN
	length := Length(value);
	NEW(result, length+1);
	COPY(value, result^);
	RETURN result;
END Copy;

END arStrings.
