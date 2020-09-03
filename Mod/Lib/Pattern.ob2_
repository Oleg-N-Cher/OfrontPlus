(* Assorted string manipulation functions *)

(* SG 2020/08/27 *)

MODULE Pattern;

IMPORT 
	S := ooc2Strings;

TYPE
	String* = POINTER TO ARRAY OF CHAR;
	StringArray* = POINTER TO ARRAY OF String;

(* Make dynamic copy of String on the heap *)

PROCEDURE Copy*(name : ARRAY OF CHAR) : String;
VAR result : String;
BEGIN
	NEW(result, S.Length(name)+1);
	COPY(name, result^);
	RETURN result;
END Copy;

(* Match - A very simple pattern matcher.

The <input> string is matched against the <pattern> string.
Corresponding characters must match, except:
  ' ' matches any number of ' ' characters
  '*' matches all characters until the character following the '*' is found

Inputs:
  pattern: pattern to match (eg. "
  input: input string to be compared with pattern
Outputs:
  result: an array of strings, one for each matched "*" field
Returns:
  TRUE when <input> matches the <pattern>
*)

PROCEDURE Match*(pattern, input : ARRAY OF CHAR; VAR result : StringArray) : BOOLEAN;
VAR
	i : LONGINT;

	patPos, patLength : LONGINT;
	patChar : CHAR;

	inPos, inLength : LONGINT;
	ch : CHAR;

	terminator : CHAR;
	outPos : LONGINT;
	buffer : POINTER TO ARRAY OF CHAR;
	fields : LONGINT;
	match : LONGINT;

	PROCEDURE Next;
	BEGIN
		IF inPos < inLength THEN
			ch := input[inPos];
			INC(inPos);
		ELSE
			ch := 0X;
		END;
	END Next;

BEGIN
	patPos := 0; patLength := S.Length(pattern);
	inPos := 0; inLength := S.Length(input);
	match := 0;

	(* allocate temporary buffer large enough for any substring of input  *)
	NEW(buffer, inLength+1);

	(* determine number of output fields, and allocate result array *)
	fields := 0;
	FOR i := 0 TO patLength-1 DO
		IF pattern[i] = '*' THEN
			INC(fields)
		END
	END;
	IF fields = 0 THEN
		result := NIL; 	(* can't return an array of zero length *)
	ELSE
		NEW(result, fields);
		FOR i := 0 TO fields-1 DO
			result[i] := NIL;
		END;
	END;

	Next;
	WHILE patPos < patLength DO
		patChar := pattern[patPos];
		INC(patPos);

		CASE patChar OF
		| ' ':
			IF ch # ' ' THEN RETURN FALSE END;
			WHILE (ch = ' ') OR (ch = 09X) DO Next; END
		| '*':
			(* scan into buffer until terminator or end of input *)
			(* note: if nothing follows '*' terminator will be 0X *)
			terminator := pattern[patPos];
			outPos := 0;
			WHILE (ch # terminator) & (ch # 0X) DO
				buffer[outPos] := ch;
				INC(outPos);
				Next;
			END;
			buffer[outPos] := 0X;
			(* copy text string into result *)
			result[match] := Copy(buffer^);
			INC(match);
		ELSE
			IF ch # patChar THEN RETURN FALSE END;
			Next;
		END;
	END;
	RETURN TRUE;
END Match;

(* split a string into array of strings, based on delimiter *)

PROCEDURE Split*(string : ARRAY OF CHAR; delimiter : CHAR) : StringArray;
VAR
	result : StringArray;
	begin, i, part : LONGINT;
	length, nParts : LONGINT;

	PROCEDURE Append(start, count : LONGINT);
	BEGIN
		NEW(result[part], count+1);
		S.Extract(string, start, count, result[part]^);
		INC(part);
	END Append;

BEGIN
	(* decide how many parts in the result *)
	length := S.Length(string);
	nParts := 1;
	FOR i := 0 TO length-1 DO
		IF string[i] = delimiter THEN INC(nParts) END;
	END;
	NEW(result, nParts);

	(* now extract sub-strings between terminators *)
	part := 0;
	begin := 0;
	FOR i := 0 TO length-1 DO
		IF string[i] = delimiter THEN
			Append(begin, i-begin);
			begin := i+1;
		END;
	END;
	IF begin <= length THEN
		Append(begin, length-begin);
	END;
	ASSERT(part = nParts);
	RETURN result;
END Split;

END Pattern.
