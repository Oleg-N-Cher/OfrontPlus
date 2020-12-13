(* Assorted string manipulation functions *)

(* SG 2020/08/27 *)

MODULE arPattern;

IMPORT 
	Platform,
	Out := arOut,
	Char := arChar,
	SL := arStringList,
	SA := arStringAssoc,
	Text := arText,
	S := arStrings;

TYPE
	String* = SL.String;
	StringArray* = SL.Array;

(* Match - A very simple pattern matcher.

The <input> string is matched against the <pattern> string.
Corresponding characters must match, except:
  ' ' matches any number of ' ' characters
  '*' matches all characters until the character following the '*' is found

Inputs:
  pattern: pattern to match
  input: input string to be compared with pattern
Outputs:
  result: an array of strings, one for each matched "*" field
Returns:
  TRUE when <input> matches the <pattern>
*)

PROCEDURE Match*(pattern-, input- : ARRAY OF CHAR; VAR result : StringArray) : BOOLEAN;
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
			result[match] := SL.Copy(buffer^);
			INC(match);
		ELSE
			IF ch # patChar THEN RETURN FALSE END;
			Next;
		END;
	END;
	RETURN TRUE;
END Match;

(* split a string into array of strings, based on delimiter *)

PROCEDURE Split*(string- : ARRAY OF CHAR; delimiter : CHAR) : StringArray;
VAR
	result : StringArray;
	begin, i, length : S.LengthType;
	part, nParts : INTEGER;

	PROCEDURE Append(start, count : S.LengthType);
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

(* Parse a string as a shell-like arguments. Arguments are separated by
whitespace, and may be quoted (eg. if the value is a string containing
spaces).
*)

PROCEDURE ParseArgs*(string- : ARRAY OF CHAR) : StringArray;
VAR
	buffer : Text.Buffer;
	list : SL.List;
	i : LONGINT;
	ch, lastCh : CHAR;
	inQuote : BOOLEAN;

	PROCEDURE Append;
	BEGIN
		SL.Add(list, buffer.GetString());
		buffer.Init(buffer.capacity);
	END Append;

BEGIN
	buffer.Init(32);
	list := NIL;

	lastCh := 0X;
	inQuote := FALSE;
	FOR i := 0 TO S.Length(string)-1 DO
		ch := string[i];
		IF (ch = ' ') & ~inQuote THEN
			IF lastCh # ' ' THEN Append; END;
		ELSIF ch = '"' THEN
			inQuote := ~inQuote;
		ELSE
			buffer.Char(ch);
		END;
		lastCh := ch;
	END;
	Append;
	SL.Reverse(list);
	RETURN SL.ToArray(list);
END ParseArgs;

(* Expand environment variables in <string>. *)
PROCEDURE ExpandEnvironmentVariables*(string-: ARRAY OF CHAR) : String;
VAR
	i : LONGINT;
	ch : CHAR;
	outBuffer : Text.Buffer;
	keyBuffer : Text.Buffer; key: String;
	arg : ARRAY 256 OF CHAR;
BEGIN
	i := 0;
	outBuffer.Init(32);
	keyBuffer.Init(32);
	LOOP
		IF i >= LEN(string) THEN EXIT END;
		ch := string[i]; INC(i);
		IF ch = '$' THEN
			keyBuffer.Clear;
			ch := string[i]; INC(i);
			WHILE Char.IsAlphaNumeric(ch) DO
				keyBuffer.Char(ch);
				ch := string[i]; INC(i);
			END;
			key := keyBuffer.GetString();
			Platform.GetEnv(key^, arg);
			outBuffer.String(arg);
		END;
		IF ch = 0X THEN EXIT END;
		outBuffer.Char(ch);
	END;
	outBuffer.Terminate;
	(* Out.String("Expand '"); Out.String(string); 
	Out.String("' -> '"); Out.String(outBuffer.value^);
	Out.String("'"); Out.Ln; *)
	RETURN outBuffer.GetString();
END ExpandEnvironmentVariables;

(* Substitute variable names inside {} braces with the corresponding strings in the <environment> *)

PROCEDURE Substitute*(string : ARRAY OF CHAR; environment : SA.Assoc) : String;
VAR
	i : LONGINT;
	ch, terminator : CHAR;
	inFormat : BOOLEAN;
	outBuffer : Text.Buffer;
	keyBuffer : Text.Buffer; key, value : String;
BEGIN
	i := 0;
	inFormat := FALSE;
	outBuffer.Init(32);
	keyBuffer.Init(32);
	LOOP
		IF i >= LEN(string) THEN EXIT END;
		ch := string[i]; INC(i);
		IF ch=0X THEN EXIT END;
		IF inFormat THEN
			IF ch = terminator THEN
				inFormat := FALSE;
				key := keyBuffer.GetString();
				value := SA.Get(environment, key^);
				IF value # NIL THEN
					IF terminator = ']' THEN
						value := ExpandEnvironmentVariables(value^);
					END;
					(* Out.String("Expand "); Out.String(key^);
					Out.String("-->"); Out.String(value^);
					Out.Ln; *)
					outBuffer.String(value^);	
				ELSE
					Out.String("Undefined key "); Out.String(key^); Out.Ln;
				END;
			ELSE
				keyBuffer.Char(ch);
			END
		ELSE
			CASE ch OF
			| '{': 
				inFormat := TRUE; terminator := '}';
				keyBuffer.Clear;
			| '[':
				inFormat := TRUE; terminator := ']';
				keyBuffer.Clear;
			| '\':
				(* escape any character, including '\' and '{' *)
				IF i < LEN(string) THEN
					ch := string[i]; INC(i);
					outBuffer.Char(ch);
				END;
			ELSE
				outBuffer.Char(ch);
			END;
		END
	END;
	RETURN outBuffer.GetString();
END Substitute;

END arPattern.
