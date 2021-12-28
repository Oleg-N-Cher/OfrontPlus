MODULE [noinit] arDir;

IMPORT 
	C := arC,
	SYSTEM, 
	arCString;

TYPE
	DIR = SYSTEM.ADRINT;
	DIRENT = SYSTEM.ADRINT;

	FileFinder* = RECORD
		dir : DIR;
		ent : DIRENT;
	END;


PROCEDURE -AAincludeDirent- "#include <dirent.h>";
PROCEDURE -opendir(name : ARRAY OF CHAR) : DIR "(SYSTEM_ADRINT) opendir((const char *)name)";
PROCEDURE -closedir(dir : DIR) "closedir((DIR*) dir)";
PROCEDURE -readdir(dir : DIR) : DIRENT "(SYSTEM_ADRINT) readdir((DIR*) dir)";
PROCEDURE -getdirname(ent : DIRENT) : C.string "(CHAR *)(((struct dirent *) ent) -> d_name)";
PROCEDURE -getdirtype(ent : DIRENT) : C.int "(int)((struct dirent *) ent) -> d_type)";
PROCEDURE -typeDir "DT_DIR";
PROCEDURE -typeReg "DT_REG";
PROCEDURE -typeLink "DT_LNK";

PROCEDURE (VAR f : FileFinder) Open*(path-: ARRAY OF CHAR) : BOOLEAN;
(* Start enumerating files in directory <path> *)
BEGIN
	f.dir := opendir(path);
	RETURN f.dir # 0;
END Open;

PROCEDURE (VAR f : FileFinder) Next*(VAR name : ARRAY OF CHAR) : BOOLEAN;
(* Find the next file in directory. Return FALSE when no more are available *)
BEGIN
	ASSERT(f.dir # 0);
	f.ent := readdir(f.dir);
	IF f.ent = 0 THEN
		RETURN FALSE;
	END;
	arCString.CopyToArray(getdirname(f.ent), name);
	RETURN TRUE;
END Next;

PROCEDURE (VAR f : FileFinder) Close*;
(* Stop enumerating files. MUST BE CALLED to avoid leaking C directory streams *)
BEGIN
	IF f.dir # 0 THEN
		closedir(f.dir);
	END;
END Close;

END arDir.
