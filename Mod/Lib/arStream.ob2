MODULE arStream;

IMPORT
	SYSTEM,
	Text := arText;

TYPE
	FILE* = POINTER [untagged] TO RECORD [untagged] END;

	(* A Text.Stream that writes to a stdio FILE *)

	StreamPtr = POINTER TO Stream;
	Stream* = RECORD (Text.Stream)
		file : FILE;
	END;

	File* = RECORD (Text.StreamWriter)
	END;

VAR
	out* : Text.StreamWriter;
	err* : Text.StreamWriter;

PROCEDURE -AAincludeStdio* "#include <stdio.h>";
PROCEDURE -getStdOut():FILE "(arStream_FILE) stdout";
PROCEDURE -getStdErr():FILE "(arStream_FILE) stderr";
PROCEDURE -fputc(file : FILE; ch : CHAR) "fputc(ch,(FILE *)file)";
PROCEDURE -fputs(file : FILE; str : ARRAY OF CHAR) "fputs((const char *)str,(FILE *)file)";
PROCEDURE -fflush(file : FILE) "fflush((FILE *)file)";
PROCEDURE -fclose*(file : FILE) "fclose((FILE *)file)";
PROCEDURE -fopen*(name, mode : ARRAY OF CHAR) : FILE
	"(arStream_FILE) fopen((const char *)name, (const char *)mode)";

(* ------ Stream ------ *)

PROCEDURE (VAR self : Stream) Char*(value : CHAR);
BEGIN
	fputc(self.file, value);
END Char;

PROCEDURE (VAR self : Stream) String*(value-: ARRAY OF CHAR);
BEGIN
	fputs(self.file, value);
END String;

PROCEDURE (VAR self : Stream) Flush*;
BEGIN
	fflush(self.file);
END Flush;

PROCEDURE NewStream(file : FILE) : StreamPtr;
VAR s : StreamPtr;
BEGIN
	NEW(s); s.file := file;
	RETURN s;
END NewStream;

(* ------ File ------ *)

PROCEDURE (VAR self : File) Close*;
BEGIN
	fclose(self.stream(StreamPtr).file)
END Close;

PROCEDURE (VAR self : File) Open*(name, mode : ARRAY OF CHAR) : BOOLEAN;
VAR file : FILE;
BEGIN
	file := fopen(name, mode);
	IF file = NIL THEN RETURN FALSE END;
	self.Init(NewStream(file));
	RETURN TRUE;
END Open;

BEGIN
	out.Init(NewStream(getStdOut()));
	err.Init(NewStream(getStdOut()));
END arStream.
