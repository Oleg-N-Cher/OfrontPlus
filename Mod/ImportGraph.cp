MODULE ImportGraph;

(* ------------------------------------------------------------------------
 * (c) 2008 - 2010 by Alexander Iljin
 * 2019, adapted for Ofront+ by Oleg N. Cher
 * ------------------------------------------------------------------------ *)

IMPORT Out := Console, CmdArgs, Files, Str := Strings, Platform;

(** -----------------------------------------------------------------------
  * This console program outputs a module import graph starting from a given
  * set of modules. The output format is the GraphVis' DOT text file.
  * This version is adapted for use with BlackBox sources.
  * ----------------------------------------------------------------------- *)

TYPE
   CHAR = SHORTCHAR;
   FileName = ARRAY 260 OF CHAR;

CONST
   MaxIdentLen = 64;
   NumKeywords = 2;
   (* symbols *)
   null = 0;
   becomes = 1; (* := *)
   import = 2; (* IMPORT *)
   comma = 3; (* , *)
   semicolon = 4; (* ; *)
   ident = 5; (* identifier *)
   eof = 6; (* end of file reached *)
   colon = 7; (* : *)
   module = 8; (* MODULE *)
   lparen = 9; (* ( *)
   less = 10; (* < *)
   minus = 11; (* - *)

   (* Values for ModuleDesc.attr *)
   mFound = 0; (* the module file was found and open for scanning *)
   mError = 1; (* there was an error during the module scanning *)
   mSystem = 2; (* the module imports SYSTEM *)
   mWindows = 3; (* the module imports Windows *)

TYPE
   Module = POINTER TO ModuleDesc;
   ModuleRef = POINTER TO ModuleRefDesc;

   ModuleDesc = RECORD
      name: ARRAY MaxIdentLen OF CHAR;
      imp : ModuleRef; (* modules imported by this one  *)
      attr: SET;
      mark: SHORTINT;  (* used for "marking" the object *)
      next: Module
   END;

   ModuleRefDesc = RECORD
      mod : Module;
      next: ModuleRef
   END;

VAR
   ch: CHAR;
   eot: BOOLEAN;
   f: Files.File; r: Files.Rider;
   id, arg: ARRAY MaxIdentLen OF CHAR;
   keyTab: ARRAY NumKeywords OF
      RECORD sym: INTEGER; id: ARRAY 7 OF CHAR END;
   numKW: INTEGER; (* number of keywords defined (0..NumKeywords-1) *)
   first: Module; (* the first item is actually a dummy *)

PROCEDURE Read;
(* Read next character from stream s to ch. Set eot if appropriate. *)
BEGIN
   IF ~eot THEN
      Files.ReadChar (r, ch);
      eot := r.eof
   ELSE
      ch := 0X
   END
END Read;

PROCEDURE GetSym (VAR sym: INTEGER);

   PROCEDURE Comment;
   (* The current ch upon entry must point to the "*" of the starting sequence. *)
   BEGIN
      Read;
      LOOP
         LOOP
            WHILE ch = "(" DO
               Read;
               IF ch = "*" THEN Comment END
            END;
            IF ch = "*" THEN Read; EXIT END;
            IF eot THEN EXIT END;
            Read
         END;
         IF ch = ")" THEN Read; EXIT END;
         IF eot THEN EXIT END
      END
   END Comment;

   PROCEDURE CompDir;
   (* A compiler directive <* *>. Can't be nested. *)
   BEGIN
      Read;
      LOOP
         LOOP
            IF ch = "*" THEN Read; EXIT END;
            IF eot THEN EXIT END;
            Read
         END;
        IF ch = ">" THEN Read; EXIT END;
        IF eot THEN EXIT END
      END
   END CompDir;

   PROCEDURE WingComment ();
   (* Wing comment spans to the end of line. *)
   BEGIN
      Read;
      LOOP
         IF (ch = 0DX) OR (ch = 0AX) THEN Read; EXIT END;
         IF eot THEN EXIT END;
         Read
      END
   END WingComment;

   PROCEDURE Ident ();
   (* Read identifier or possibly a keyword. *)
   VAR i: INTEGER;
   BEGIN
      i := 0;
      REPEAT
         IF i < MaxIdentLen THEN id [i] := ch; INC (i) END;
         Read;
      UNTIL (ch < "0") OR (ch > "9") & (CAP (ch) < "A") OR (CAP (ch) > "Z");
      id [i] := 0X;
      i := 0;
      WHILE (i < numKW) & (id # keyTab [i].id) DO INC (i) END;
      IF i < numKW THEN sym := keyTab [i].sym ELSE sym := ident END
   END Ident;

BEGIN
   WHILE ~eot & (ch <= " ") DO Read END;
   IF eot THEN
      sym := eof
   ELSE
      CASE ch OF
      | "A".."Z", "a".."z":
         Ident
      | ":":
         Read;
         IF ch = "=" THEN Read; sym := becomes ELSE sym := colon END
      | ";":
         Read; sym := semicolon
      | ",":
         Read; sym := comma
      | "(":
         Read;
         IF ch = "*" THEN Comment; GetSym (sym) ELSE sym := lparen END
      | "<":
         Read;
         IF ch = "*" THEN CompDir; GetSym (sym) ELSE sym := less END
      | "-":
         Read;
         IF ch = "-" THEN WingComment; GetSym (sym) ELSE sym := minus END
      ELSE
         Read; sym := null
      END
   END
END GetSym;

PROCEDURE EnlistModule (VAR name: ARRAY OF CHAR): ModuleRef;
(* Add module name to the global processing list if it is not there yet.
 * Return a new instance of ModuleRef pointing to the module in the global
 * list. *)
VAR
   m,new: Module;
   res: ModuleRef;
BEGIN
   m := first;
   WHILE (m.next # NIL) & (m.next.name # name) DO m := m.next END;
   IF m.next = NIL THEN
      NEW (new);
      new.name := name$;
      new.attr := {};
      m.next := new;
   END;
   NEW (res);
   res.mod := m.next;
   RETURN res
END EnlistModule;

PROCEDURE ExtractSubsystem (VAR ident, subsystem, name: ARRAY OF CHAR);
(* Split ident into subsystem and name parts. *)
VAR
   i: INTEGER; (* subsystem name end position *)
BEGIN
   i := 0;
   WHILE (ident [i] # 0X) & ("A" <= ident [i]) & (ident [i] <= "Z") DO
      INC (i);
   END;
   WHILE (ident [i] # 0X) & ("a" <= ident [i]) & (ident [i] <= "z") DO
      INC (i);
   END;
   subsystem := "";
   IF ident [i] # 0X THEN
			Str.Extract (ident, 0, i, subsystem);
      name := "";
      Str.Extract (ident, i, LEN (name), name);
   ELSE
      name := ident$;
   END;
END ExtractSubsystem;

PROCEDURE BlackListed (VAR name: ARRAY OF CHAR): BOOLEAN;
(* Return TRUE if the module name is to be excluded from view. *)
VAR
   sub, tmp: FileName;
BEGIN
   ExtractSubsystem (id, sub, tmp);
   RETURN (name = "SYSTEM") OR (name = "COM") OR (sub = "Win")
END BlackListed;

PROCEDURE OpenFile (VAR path: ARRAY OF CHAR): Files.File;
VAR
   i: INTEGER;
BEGIN
   IF Platform.Unix THEN
      i := 0;
      WHILE (i < LEN(path)) & (path[i] # 0X) DO
         IF path[i] = "\" THEN path[i] := "/" END;
         INC(i);
      END;
   END;
   RETURN Files.Old (path)
END OpenFile;

PROCEDURE OpenStream (VAR fname: ARRAY OF CHAR): BOOLEAN;
(* Open the file fname in the global stream s for reading. File with the name fname is
 * looked for in the current folder and in some other folders relative to current.
 * 'path' is set according to the folder in which the module was found.
 * Return TRUE on success. *)
VAR
   name, full: FileName;
BEGIN
   name := fname$; Str.Append (".Mod", name);
   f := OpenFile (name);
   IF f = NIL THEN
      full := "Mod\"; Str.Append (name, full);
      f := OpenFile (full);
      IF f = NIL THEN
         full := "Lib\Mod\"; Str.Append (name, full);
         f := OpenFile (full);
         IF f = NIL THEN
            ExtractSubsystem (fname, full, name);
            IF full # "" THEN
               Str.Append ("\Mod\", full);
            ELSE (* no subsystem part found => "System" subsystem *)
               full := "System\Mod\";
            END;
            Str.Append (name, full);
            Str.Append (".odc", full);
            f := OpenFile (full);
         END;
      END;
   END;
   IF f # NIL THEN Files.Set(r, f, 0) END;
   RETURN f # NIL
END OpenStream;

PROCEDURE LoadImports (m: Module);
(* Fill m.imp, i.e. load the list of imported modules by opening and scanning the file m.name. *)
VAR sym: INTEGER;

   PROCEDURE SkipToModuleSection (): BOOLEAN;
   (* Skip module heading up to the MODULE section, return TRUE on success. *)
   BEGIN
      eot := FALSE; Read;
      REPEAT
         GetSym (sym);
      UNTIL eot OR (sym = module);
      IF sym = module THEN
         GetSym (sym);
         IF sym = ident THEN
            GetSym (sym);
            IF sym = semicolon THEN
               RETURN TRUE
            END
         END
      END;
      RETURN FALSE
   END SkipToModuleSection;

   PROCEDURE SkipToImportSection (): BOOLEAN;
   (* Skip module heading up to the IMPORT section, return TRUE on success. *)
   BEGIN
      IF ~eot THEN
         REPEAT
            GetSym (sym);
         UNTIL eot OR (sym = import);
         IF sym = import THEN
            GetSym (sym);
            RETURN TRUE
         END;
      END;
      RETURN FALSE
   END SkipToImportSection;

   PROCEDURE ProcessImportSection ();
   (* Process IMPORT section and fill m.attr and m.imp. *)
   VAR
      ref: ModuleRef;
      err: BOOLEAN;
      sub,name: FileName;
   BEGIN
      err := FALSE;
      WHILE (sym = ident) & ~err DO
         GetSym (sym);
         IF sym = becomes THEN
            GetSym (sym);
            err := sym # ident;
            IF ~err THEN
               GetSym (sym);
               err := (sym # comma) & (sym # semicolon)
            END
         END;
         IF ~err THEN
            IF (sym = comma) OR (sym = semicolon) THEN
               IF id = "SYSTEM" THEN
                  INCL (m.attr, mSystem);
               ELSIF (id = "KERNEL32") OR (id = "GDI32") OR (id = "USER32")
                  OR (id = "SHELL32") OR (id = "COMDLG32") OR (id = "ADVAPI32")
                  OR (id = "COM")
               THEN
                  INCL (m.attr, mWindows);
               ELSE
                  ExtractSubsystem (id, sub, name);
                  IF sub = "Win" THEN
                     INCL (m.attr, mWindows);
                  END;
               END;
               IF ~BlackListed (id) THEN
                  ref := EnlistModule (id);
                  ref.next := m.imp;
                  m.imp := ref;
               END;
               IF sym = comma THEN GetSym (sym) END
            END
         END
      END;
      IF err THEN
         INCL (m.attr, mError);
      END;
   END ProcessImportSection;

BEGIN
   IF OpenStream (m.name) THEN
      INCL (m.attr, mFound);
      IF SkipToModuleSection () THEN m.name := id$;
         IF SkipToImportSection () THEN
            ProcessImportSection ();
         END;
      END;
   END;
END LoadImports;

PROCEDURE OutputData (m: Module; markedImportOnly: BOOLEAN): BOOLEAN;
(* Output the data according to the Module's information. If markedImportOnly,
 * then only output import links to the modules with .mark = 1. Return TRUE if
 * anything was written to the Out. *)
VAR
   res: BOOLEAN;
   ref: ModuleRef;

   PROCEDURE OutputName (m: Module);
   BEGIN
      Out.Char ('"');
      Out.String (m.name);
      Out.Char ('"');
   END OutputName;

   PROCEDURE OutputLink (from, to: Module);
   BEGIN
      OutputName (from);
      Out.String ("->");
      OutputName (to);
      Out.String ("; ");
   END OutputLink;

BEGIN
   IF mFound IN m.attr THEN
      res := FALSE;
      IF ~(mError IN m.attr) THEN
         IF mSystem IN m.attr THEN
            (* highlight modules importing SYSTEM *)
            OutputName (m);
            Out.String (" [fontcolor = red]; ");
            res := TRUE;
         END;
         IF mWindows IN m.attr THEN
            (* shape modules importing Windows *)
            OutputName (m);
            Out.String (" [shape = box]; ");
            res := TRUE;
         END;
         ref := m.imp;
         IF markedImportOnly THEN
            WHILE ref # NIL DO
               IF ref.mod.mark = 1 THEN
                  OutputLink (m, ref.mod);
                  res := TRUE;
               END;
               ref := ref.next;
            END;
         ELSE
            res := res OR (ref # NIL);
            WHILE ref # NIL DO
               OutputLink (m, ref.mod);
               ref := ref.next;
            END;
         END;
      END;
   ELSE
      res := TRUE;
      Out.String (m.name);
      Out.String (" [shape = plaintext];")
   END;
   RETURN res
END OutputData;

PROCEDURE InitScanner ();

   PROCEDURE AddKeyword (sym: INTEGER; IN id: ARRAY OF CHAR);
   BEGIN
      keyTab [numKW].sym := sym; keyTab [numKW].id := id$;
      INC (numKW)
   END AddKeyword;

BEGIN
   numKW := 0;
   AddKeyword (module, "MODULE");
   AddKeyword (import, "IMPORT")
END InitScanner;

PROCEDURE OutputImports (reduced: BOOLEAN);
(* If 'reduced' = TRUE, then use marking to reduce the number of import links
 * shown. E.g., given (A->B, B->C, A->C) do not display A->C. *)
VAR i: INTEGER; m: Module; tmp: ModuleRef;

   PROCEDURE ClearMarks ();
   VAR m: Module;
   BEGIN
      m := first.next;
      WHILE m # NIL DO
         m.mark := 0;
         m := m.next
      END
   END ClearMarks;

   PROCEDURE MarkImported (ref: ModuleRef);
   BEGIN
      WHILE ref # NIL DO
         IF ref.mod # m THEN
            IF ref.mod.mark < 2 THEN (* this check is to avoid overflow *)
               INC (ref.mod.mark);
               IF ref.mod.mark < 2 THEN
                  MarkImported (ref.mod.imp);
               END;
            END;
         ELSE
            (* circular import *)
         END;
         ref := ref.next;
      END;
   END MarkImported;

BEGIN
   InitScanner;
   i := 1; NEW (first); first.name [0] := 0X;
   WHILE i <= CmdArgs.Count DO
			CmdArgs.Get(i, arg);
			IF arg = "-r" THEN reduced := ~reduced;
			ELSE tmp := EnlistModule (arg);
			END;
      INC (i)
   END;
   m := first.next;
   WHILE m # NIL DO
      LoadImports (m);
      m := m.next
   END;
   m := first.next;
   WHILE m # NIL DO
      IF reduced THEN
         ClearMarks;
         MarkImported (m.imp);
      END;
      IF OutputData (m, reduced) THEN Out.Ln END;
      m := m.next
   END
END OutputImports;

(*
PROCEDURE DumpList ();
(* For debugging: output module list. *)
VAR m: Module;
BEGIN
   m := first;
   WHILE m # NIL DO
      Out.String (m.name); Out.Char (";");
      m := m.next
   END
END DumpList;
*)

BEGIN
   IF CmdArgs.Count # 0 THEN
      Files.SetSearchPath(".");
      Out.String ("strict digraph G{");
      Out.Ln;
      OutputImports (FALSE);
      Out.Char ("}");
      Out.Ln;
(*      DumpList *)
   ELSE
      Out.String("Usage: [-r] module1 [module2 ...]"); Out.Ln;
      Out.String('  If "-r", then use marking to reduce the number of import links shown.'); Out.Ln;
      Out.String('  E.g., given (A->B, B->C, A->C) do not display A->C.');
      Out.Ln
   END
END ImportGraph.
