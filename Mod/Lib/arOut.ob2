MODULE arOut;

IMPORT
	arText,
	arFile;

VAR
	writer : arText.StreamWriter;

PROCEDURE String*(text-: ARRAY OF CHAR);
BEGIN
	writer.String(text);
END String;

PROCEDURE Int*(value : LONGINT);
BEGIN
	writer.Integer(value);
END Int;

PROCEDURE Hex*(value : LONGINT);
BEGIN
	writer.Hex(value);
END Hex;

PROCEDURE Char*(value : CHAR);
BEGIN
	writer.Char(value);
END Char;

PROCEDURE Real*(value : LONGREAL);
BEGIN
	writer.Real(value);
END Real;

PROCEDURE Ln*;
BEGIN
	writer.Ln;
END Ln;

PROCEDURE Flush*;
BEGIN
	writer.Flush;
END Flush;

PROCEDURE Bool*(value : BOOLEAN);
BEGIN
	writer.Boolean(value);
END Bool;

BEGIN
	writer.Init(arFile.out.GetStream());
END arOut.
