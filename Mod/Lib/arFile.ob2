MODULE arFile;

IMPORT
	C := arC,
	Text := arText,
	SYSTEM;

CONST
	seekSet* = 0;
	seekCur* = 1;
	seekEnd* = 2;

TYPE
	FILE* = POINTER [untagged] TO RECORD [untagged] END;

	FilePos* = SYSTEM.INT64;

	StreamPtr* = POINTER TO Stream;
	Stream* = RECORD (Text.Stream)
		file : FILE;
	END;

	File* = POINTER TO RECORD
		file : FILE;
		success- : BOOLEAN;
	END;

VAR
	out* : File;	(* standard output *)
	err* : File;	(* standard error *)

PROCEDURE -AAincludeStdio- "#include <stdio.h>";
PROCEDURE -AAincludeErrno- "#include <errno.h>";
PROCEDURE -getStdOut() : FILE "(arFile_FILE) stdout";
PROCEDURE -getStdErr() : FILE "(arFile_FILE) stderr";

PROCEDURE -fclose*(file : FILE) : C.int "fclose((FILE *)file)";
PROCEDURE -feof*(file : FILE) : C.int "feof((FILE *)file)";
PROCEDURE -fflush(file : FILE) : C.int "fflush((FILE *)file)";
PROCEDURE -fopen*(name, mode : ARRAY OF CHAR) : FILE
	"(arFile_FILE) fopen((const char *)name, (const char *)mode)";
PROCEDURE -fputc(file : FILE; ch : CHAR) : C.int "fputc(ch,(FILE *)file)";
PROCEDURE -fputs(file : FILE; str : ARRAY OF CHAR) : C.int "fputs((const char *)str,(FILE *)file)";
PROCEDURE -fseek*(file : FILE; offset : C.longint; whence : C.int) : C.int "fseek((FILE *)file, offset, whence)";
PROCEDURE -ftell(file : FILE) : C.longint "ftell((FILE *)file)";
PROCEDURE -errno() : C.int "errno";

(* ------ Stream ------ *)

PROCEDURE (VAR self : Stream) Char*(value : CHAR);
VAR result : C.int;
BEGIN
	result := fputc(self.file, value);
END Char;

PROCEDURE (VAR self : Stream) String*(value-: ARRAY OF CHAR);
VAR result : C.int;
BEGIN
	result := fputs(self.file, value);
END String;

PROCEDURE (VAR self : Stream) Flush*;
VAR result : C.int;
BEGIN
	result := fflush(self.file);
END Flush;

PROCEDURE NewStream(file : FILE) : StreamPtr;
VAR s : StreamPtr;
BEGIN
	NEW(s); s.file := file;
	RETURN s;
END NewStream;

(* ------ File ------ *)

PROCEDURE (self : File) Init(file : FILE);
BEGIN
	self.file := file;
	self.success := TRUE;
END Init;

PROCEDURE (self : File) Close*;
BEGIN
	self.success := fclose(self.file) = 0;
END Close;

PROCEDURE (self : File) Flush;
BEGIN
	self.success := fflush(self.file) = 0;
END Flush;

PROCEDURE (self : File) Seek*(offset : C.longint; whence : C.int) : C.int;
BEGIN
	self.success := fseek(self.file, offset, whence) = 0;
	RETURN 0
END Seek;

PROCEDURE (self : File) Tell*() : C.longint;
VAR result : C.longint;
BEGIN
	result := ftell(self.file);
	self.success := result >= 0;
	RETURN 0
END Tell;

PROCEDURE (self : File) PutC*(ch : CHAR);
BEGIN
	self.success := fputc(self.file, ch) = 0;
END PutC;

PROCEDURE (self : File) PutS*(str : ARRAY OF CHAR) : C.int;
BEGIN
	self.success := fputs(self.file, str) = 0;
	RETURN 0
END PutS;

PROCEDURE (self : File) EOF*() : BOOLEAN;
BEGIN
	RETURN feof(self.file) # 0;
END EOF;

PROCEDURE (self : File) Error() : C.int;
BEGIN
	RETURN errno();
END Error;

PROCEDURE NewFile(f : FILE) : File;
VAR file : File;
BEGIN
	NEW(file);
	file.file := f;
	RETURN file;
END NewFile;

PROCEDURE (self : File) GetStream*() : StreamPtr;
BEGIN
	RETURN NewStream(self.file);
END GetStream;

PROCEDURE Open*(name, mode : ARRAY OF CHAR) : File;
VAR 
	f : FILE;
	file : File;
BEGIN
	f := fopen(name, mode);
	IF f = NIL THEN RETURN NIL END;
	RETURN NewFile(f);
END Open;

BEGIN
	out := NewFile(getStdOut());
	err := NewFile(getStdErr());
END arFile.
