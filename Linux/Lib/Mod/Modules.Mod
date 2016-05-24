MODULE Modules;  (* jt 27.2.95 *)

  IMPORT SYSTEM, Console, Unix(*for errno*), Kernel;

  CONST
    ModNameLen* = 20;

  TYPE
    ModuleName* = ARRAY ModNameLen OF CHAR;
    Module* = POINTER TO ModuleDesc;
    Cmd* = POINTER TO CmdDesc;
    ModuleDesc* = RECORD  (* cf. SYSTEM.Mod *)
      next-: Module;
      name-: ModuleName;
      refcnt-: LONGINT;
      cmds-: Cmd;
      types-: LONGINT;
      enumPtrs-: PROCEDURE (P: PROCEDURE(p: LONGINT));
      reserved1, reserved2: LONGINT;
    END ;

    Command* = PROCEDURE;

    CmdDesc* = RECORD
      next-: Cmd;
      name-: ARRAY 24 OF CHAR;
      cmd-: Command
    END ;

  VAR
    res*: INTEGER;
    resMsg*: ARRAY 256 OF CHAR;
    imported*, importing*: ModuleName;
    trace*: BOOLEAN; (* trace module loading activities *)


  PROCEDURE -include()
    "#include <dlfcn.h>";

  PROCEDURE -dlopen(path: ARRAY OF CHAR): LONGINT
    "(long)dlopen((const char*)path, RTLD_LAZY)";

  PROCEDURE -dlsym(handle: LONGINT; name: ARRAY OF CHAR): Command
    "(Modules_Command)dlsym((void*)handle, name)";

  PROCEDURE -dlclose(handle: LONGINT): LONGINT
    "dlclose((void*)handle)";

  PROCEDURE -dlerror(VAR s: ARRAY OF CHAR)
    "__COPY(dlerror(), s, s__len)";

  PROCEDURE -next(): LONGINT
    "(LONGINT)RTLD_NEXT";

  PROCEDURE -modules*(): Module
      "(Modules_Module)SYSTEM_modules";

  PROCEDURE -setmodules*(m: Module)
      "SYSTEM_modules = m";


  PROCEDURE Append(VAR a: ARRAY OF CHAR; b: ARRAY OF CHAR);
    VAR i, j: INTEGER;
  BEGIN
    i := 0; WHILE a[i] # 0X DO INC(i) END;
    j := 0; WHILE b[j] # 0X DO a[i] := b[j]; INC(i); INC(j) END;
    a[i] := 0X
  END Append;

  PROCEDURE GetSubsys1(n: ARRAY OF CHAR; VAR s: ARRAY OF CHAR);  (* digit treated as upper case *)
    VAR i: INTEGER; ch: CHAR;
  BEGIN
    ch := n[0]; i := 0;
    WHILE (ch # 0X) & ((ch < "a") OR (ch > "z")) DO s[i] := ch; INC(i); ch := n[i] END ;
    WHILE (ch >= "a") & (ch <= "z") DO s[i] := ch; INC(i); ch := n[i] END ;
    IF ch = 0X THEN s[0] := 0X ELSE s[i] := 0X END
  END GetSubsys1;

  PROCEDURE GetSubsys2(n: ARRAY OF CHAR; VAR s: ARRAY OF CHAR);  (* digit treated as lower case *)
    VAR i: INTEGER; ch: CHAR;
  BEGIN
    ch := n[0]; i := 0;
    WHILE (ch >= "A") & (ch <= "Z") DO s[i] := ch; INC(i); ch := n[i] END ;
    WHILE (ch # 0X) & ((ch < "A") OR (ch > "Z")) DO s[i] := ch; INC(i); ch := n[i] END ;
    IF ch = 0X THEN s[0] := 0X ELSE s[i] := 0X END
  END GetSubsys2;

  PROCEDURE FullName(VAR n: ARRAY OF CHAR): BOOLEAN;
    VAR i: INTEGER; ch: CHAR;
  BEGIN
    i := 0; ch := n[0];
    WHILE ch # 0X DO
      IF ch = "." THEN RETURN TRUE END ;
      INC(i); ch := n[i]
    END ;
    RETURN FALSE
  END FullName;

  PROCEDURE err();
    VAR i: INTEGER; s: ARRAY 1024 OF CHAR;
  BEGIN i := 0; dlerror(s);
    IF trace THEN
      Console.String(s);
      Console.Ln;
    END
  END err;

  PROCEDURE Load(name, bodyname: ARRAY OF CHAR; VAR lib: LONGINT; VAR body: Command);
    VAR libname: ARRAY 256 OF CHAR;
  BEGIN
    libname := "lib"; Append(libname, name); Append(libname, ".so");
    IF trace THEN
      Console.String("Modules.Load(libname=");
      Console.String(libname);
      Console.String(", entry=");
      Console.String(bodyname);
      Console.String(")");
      Console.Ln;
    END ;
    lib := dlopen(libname);
    IF lib # 0 THEN body := dlsym(lib, bodyname);
      IF body = NIL THEN err() END
    ELSE err()
    END ;
  END Load;

  PROCEDURE ThisMod* (name: ARRAY OF CHAR): Module;
    VAR m: Module; bodyname, libname1, libname2: ARRAY 64 OF CHAR;
      body: Command; lib, handle: LONGINT;
  BEGIN
    m := modules();
    WHILE (m # NIL) & (m.name # name) DO m := m.next END ;
    IF m = NIL THEN
      IF trace THEN
        Console.String("Modules.ThisMod(name=");
        Console.String(name);
        Console.String(")");
        Console.Ln;
      END ;
      handle := 0; body := NIL;
      COPY(name, bodyname); Append(bodyname, "__init");
      Load(name, bodyname, lib, body);
      IF body # NIL THEN
        handle := lib (* lib belongs to module 1:1 *)
      ELSE
        GetSubsys1(name, libname1);
        IF libname1[0] # 0X THEN
          Load(libname1, bodyname, lib, body)
        END
      END ;
      IF body = NIL THEN
        GetSubsys2(name, libname2);
        IF libname2[0] # 0X THEN
          IF (libname2 # libname1) THEN
            Load(libname2, bodyname, lib, body)
          END
        END
      END ;
      IF body = NIL THEN
        IF FullName(Kernel.LIB) THEN lib := dlopen(Kernel.LIB);
          IF lib # 0 THEN body := dlsym(lib, bodyname) ELSE err() END
        ELSE Load(Kernel.LIB, bodyname, lib, body)
        END
      END ;
      IF body # NIL THEN
        body(); m := modules();
        WHILE (m # NIL) & (m.name # name) DO m := m.next END ;
        IF m # NIL THEN m.reserved1 := handle END
      END
    END ;
    IF m # NIL THEN res := 0; resMsg := ""
    ELSE res := 1; COPY(name, importing);
      resMsg := ' module "'; Append(resMsg, name); Append(resMsg, '" not found');
    END ;
    RETURN m
  END ThisMod;

  PROCEDURE ThisCommand* (mod: Module; name: ARRAY OF CHAR): Command;
    VAR c: Cmd;
  BEGIN c := mod.cmds;
    WHILE (c # NIL) & (c.name # name) DO c := c.next END ;
    IF c # NIL THEN res := 0; resMsg := ""; RETURN c.cmd
    ELSE res := 2; resMsg := ' command "'; COPY(name, importing);
      Append(resMsg, mod.name); Append(resMsg, "."); Append(resMsg, name); Append(resMsg, '" not found');
      RETURN NIL
    END
  END ThisCommand;

  PROCEDURE Free*(name: ARRAY OF CHAR; all: BOOLEAN);
    VAR m, p: Module;
  BEGIN m := modules();
    IF all THEN
      res := 1; resMsg := 'unloading "all" not yet supported'
    ELSE
      WHILE (m # NIL) & (m.name # name) DO p := m; m := m.next END ;
      IF (m # NIL) & (m.refcnt = 0) THEN
        IF m.reserved1 # 0 THEN
          IF dlclose(m.reserved1) # 0 THEN
            res := 1; dlerror(resMsg);
          ELSE res := 0;
            IF m = modules() THEN setmodules(m.next)
            ELSE p.next := m.next
            END
          END
        ELSE res := 1;
          resMsg := "module not loaded in separate library"
        END
      ELSE res := 1;
        IF m = NIL THEN resMsg := "module not found"
        ELSE resMsg := "clients of this module exist"
        END
      END
    END
  END Free;

END Modules.
