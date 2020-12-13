(* Functions for manipulating file paths *)

MODULE arPath;

IMPORT
	Out := arOut,
	Strings := arStrings;

CONST
	separator = '/';

PROCEDURE DirName*(path-: ARRAY OF CHAR; VAR dir : ARRAY OF CHAR);
VAR index : Strings.LengthType;
BEGIN
	index := Strings.LastIndexOf(path, separator, -1);
	IF index >= 0 THEN
		Strings.Extract(path, 0, index, dir);
	ELSE
		dir[0] := 0X;
	END;
END DirName;

PROCEDURE BaseName*(path-: ARRAY OF CHAR; VAR base : ARRAY OF CHAR);
VAR index : Strings.LengthType;
BEGIN
	index := Strings.LastIndexOf(path, separator, -1);
	IF index >= 0 THEN
		Strings.Extract(path, index+1, LEN(base), base);
	ELSE
		Strings.Assign(path, base);
	END;
END BaseName;
  
PROCEDURE ExtName*(path-: ARRAY OF CHAR; VAR ext : ARRAY OF CHAR);
VAR index : Strings.LengthType;
BEGIN
	index := Strings.LastIndexOf(path, ".", -1);
	IF (index >= 0) & (Strings.IndexOf(path, separator, index)<0) THEN
		Strings.Extract(path, index, LEN(ext), ext);
	ELSE
		ext[0] := 0X;
	END;
END ExtName;

END arPath.
