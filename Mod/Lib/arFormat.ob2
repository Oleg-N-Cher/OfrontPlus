
(* This module attempts some basic Formatting in the spirit of printf. We don't
have variable argument lists, so there are families of function for specific
numbers of parameters. Arguments are of type Value.Object, so primitive types
must be boxed like so:

   Out3("{} plus {} equals {}\n", V.int(x), V.int(y), V.int(x+y));

Use OutN to print a Formatted string with N paramters. 

The corresponding FormatN functions return a Formatted Value.String. For example:

  text := Format2("Moved to position ({}, {})\n", V.real(x), V.real(y));

For Value.Real and Value.Integer, standard printf Formatting options are
supported. Don't include the '%' as this is added automatically. For example:

  Out3("MIDI: {02x} {02x} {02x}\n", V.int(status), V.int(data1), V.int(data2));

*)

MODULE arFormat;

IMPORT
	SYSTEM,
	arStream,
	V := arValue,

	CFormat := arCFormat,
	S := arStrings, 
	Text := arText;

(* Handle conversion options. <options> is an option string of length <oPos>.
THe string representation of <o> will be stored in <output>, returning TRUE on
success. *)

PROCEDURE ConvertOption(VAR options: ARRAY OF CHAR; oPos : INTEGER; VAR output : ARRAY OF CHAR; o : V.Object) : BOOLEAN;
VAR type : CHAR;
BEGIN
	IF o = NIL THEN RETURN FALSE END;
	CASE options[oPos-1] OF
	| 'd', 'i', 'u', 'x', 'c':
		WITH o : V.Integer DO
			CFormat.IntToString(output, options, o.value); RETURN TRUE;
		ELSE
		END
	| 'e', 'f', 'g':
		WITH o : V.Real DO
			CFormat.RealToString(output, options, o.value); RETURN TRUE;
		ELSE
		END
	| 's' :
		WITH o : V.String DO
			CFormat.StringToString(output, options, o.value^); RETURN TRUE;
		ELSE
		END
	ELSE
	END;
	RETURN FALSE;
END ConvertOption;

(* Format with arguments stored in ARRAY OF V.Object. For fixed argument
lengths, this will be allocated on the stack *)

PROCEDURE Format*(VAR w : Text.Writer; fmt : ARRAY OF CHAR; VAR args : ARRAY OF V.Object);
VAR
	i, arg : LONGINT; ch : CHAR;
	inFormat : BOOLEAN;
	options : ARRAY 32 OF CHAR; 
	conversion : ARRAY 32 OF CHAR; 
	oPos : INTEGER;
	o : V.Object;
	type : CHAR;
BEGIN
	i := 0; arg := 0;
	inFormat := FALSE;
	WHILE (i < LEN(fmt)) & (fmt[i] # 0X) DO
		ch := fmt[i]; INC(i);
		IF inFormat THEN
			IF ch = '}' THEN
				inFormat := FALSE;
				IF arg < LEN(args) THEN
					o := args[arg]; INC(arg);
					(* if options specified, try a specific conversion *)
					IF (oPos > 0) & ConvertOption(options, oPos, conversion, o) THEN
						w.String(conversion);
					ELSE
						(* otherwise, use default conversion to string *)
						V.Format(w, o);
					END;
				END
			ELSE
				IF oPos < LEN(options)-1 THEN
					options[oPos] := ch; INC(oPos);
					options[oPos] := 0X;
				END;
			END
		ELSE
			CASE ch OF
			| '{': 
				inFormat := TRUE; 
				options[0] := '%';
				options[1] := 0X;
				oPos := 1;
			| '\':
				IF i < LEN(fmt) THEN
					ch := fmt[i];
					CASE ch OF
					| 'a': ch := 07X;
					| 'b': ch := 08X;
					| 't': ch := 09X;
					| 'n': ch := 0AX;
					| 'r': ch := 0DX;
					ELSE
					END;
					INC(i); w.Char(ch);
				END
			ELSE
				w.Char(ch);
			END;
		END
	END;
END Format;

PROCEDURE FormatString*(VAR fmt : ARRAY OF CHAR; VAR args : ARRAY OF V.Object) : V.String;
VAR result : Text.Buffer;
BEGIN
	result.Init(32);
	Format(result, fmt, args);
	RETURN V.strb(result);
END FormatString;

PROCEDURE Format0*(fmt : ARRAY OF CHAR) : V.String;
VAR args : ARRAY 1 OF V.Object;
BEGIN
	args[0] := NIL;
	RETURN FormatString(fmt, args);
END Format0;

PROCEDURE Format1*(fmt : ARRAY OF CHAR; p1 : V.Object) : V.String;
VAR args : ARRAY 1 OF V.Object; 
BEGIN
	args[0] := p1;
	RETURN FormatString(fmt, args);
END Format1;

PROCEDURE Format2*(fmt : ARRAY OF CHAR; p1, p2 : V.Object) : V.String;
VAR args : ARRAY 2 OF V.Object;
BEGIN
	args[0] := p1; args[1] := p2;
	RETURN FormatString(fmt, args);
END Format2;

PROCEDURE Format3*(fmt : ARRAY OF CHAR; p1, p2, p3 : V.Object) : V.String;
VAR args : ARRAY 3 OF V.Object;
BEGIN
	args[0] := p1; args[1] := p2; args[2] := p3;
	RETURN FormatString(fmt, args);
END Format3;

PROCEDURE Format4*(fmt : ARRAY OF CHAR; p1, p2, p3, p4 : V.Object) : V.String;
VAR args : ARRAY 4 OF V.Object;
BEGIN
	args[0] := p1; args[1] := p2; args[2] := p3; args[3] := p4;
	RETURN FormatString(fmt, args);
END Format4;

PROCEDURE Format5*(fmt : ARRAY OF CHAR; p1, p2, p3, p4, p5 : V.Object) : V.String;
VAR args : ARRAY 5 OF V.Object;
BEGIN
	args[0] := p1; args[1] := p2; args[2] := p3; args[3] := p4; args[4] := p5;
	RETURN FormatString(fmt, args);
END Format5;

(* Format with arguments stored in Value.List *)

PROCEDURE FormatList*(w : Text.Writer; fmt : ARRAY OF CHAR; args : V.List);
VAR 
	argsArray : V.ObjectArray;
BEGIN
	argsArray := args.ToObjectArray();
	Format(w, fmt, argsArray^);
END FormatList;

(* Out0 .. Out5 output Formatted messages with fixed number of paramters *)

PROCEDURE Out0*(fmt : ARRAY OF CHAR);
VAR args : ARRAY 1 OF V.Object;
BEGIN
	args[0] := NIL;
	Format(arStream.out, fmt, args);
END Out0;

PROCEDURE Out1*(fmt : ARRAY OF CHAR; p1 : V.Object);
VAR args : ARRAY 1 OF V.Object; 
BEGIN
	args[0] := p1;
	Format(arStream.out, fmt, args);
END Out1;

PROCEDURE Out2*(fmt : ARRAY OF CHAR; p1, p2 : V.Object);
VAR args : ARRAY 2 OF V.Object;
BEGIN
	args[0] := p1; args[1] := p2;
	Format(arStream.out, fmt, args);
END Out2;

PROCEDURE Out3*(fmt : ARRAY OF CHAR; p1, p2, p3 : V.Object);
VAR args : ARRAY 3 OF V.Object;
BEGIN
	args[0] := p1; args[1] := p2; args[2] := p3;
	Format(arStream.out, fmt, args);
END Out3;

PROCEDURE Out4*(fmt : ARRAY OF CHAR; p1, p2, p3, p4 : V.Object);
VAR args : ARRAY 4 OF V.Object;
BEGIN
	args[0] := p1; args[1] := p2; args[2] := p3; args[3] := p4;
	Format(arStream.out, fmt, args);
END Out4;

PROCEDURE Out5*(fmt : ARRAY OF CHAR; p1, p2, p3, p4, p5 : V.Object);
VAR args : ARRAY 5 OF V.Object;
BEGIN
	args[0] := p1; args[1] := p2; args[2] := p3; args[3] := p4; args[4] := p5;
	Format(arStream.out, fmt, args);
END Out5;

(* Equivalent to Out1("{}", o) *)

PROCEDURE Show*(o : V.Object);
BEGIN
	V.Format(arStream.out, o);
END Show;

PROCEDURE ShowLn*(o : V.Object);
BEGIN
	V.Format(arStream.out, o);
	arStream.out.Ln;
END ShowLn;

END arFormat.
