MODULE Modules;  (* jt 6.1.96 *)

  (* access to list of modules and commands, based on ETH Oberon *)


  IMPORT SYSTEM, Platform, Heap; (* Note, must import Platform before Heap *)

  TYPE
    ModuleName* = Heap.ModuleName;
    Module*     = Heap.Module;
    Cmd*        = Heap.Cmd;
    Command*    = Heap.Command;
  VAR
    res*:       INTEGER;
    resMsg*:    ARRAY 256 OF SHORTCHAR;
    imported*:  ModuleName;
    importing*: ModuleName;


(* Module and command lookup by name *)

  PROCEDURE -modules(): Module     "(Heap_Module)Heap_modules";
  (*PROCEDURE -setmodules(m: Module) "Heap_modules = m";*)

  PROCEDURE ThisMod* (IN name: ARRAY OF SHORTCHAR): Module;
    VAR m: Module; bodyname: ARRAY 64 OF SHORTCHAR; body: Command;
  BEGIN m := modules();
    WHILE (m # NIL) & (m.name # name) DO m := m.next END;
    IF m # NIL THEN res := 0; resMsg := ""
    ELSE res := 1; importing := name$;
      resMsg := ' module "' + name + '" not found'
    END;
    RETURN m
  END ThisMod;

  PROCEDURE ThisCommand* (mod: Module; IN name: ARRAY OF SHORTCHAR): Command;
    VAR c: Cmd;
  BEGIN c := mod.cmds;
    WHILE (c # NIL) & (c.name # name) DO c := c.next END;
    IF c # NIL THEN res := 0; resMsg := ""; RETURN c.cmd
    ELSE res := 2; importing := name$;
      resMsg := ' command "' + mod.name + "." + name + '" not found';
      RETURN NIL
    END
  END ThisCommand;

  PROCEDURE Free*(IN name: ARRAY OF SHORTCHAR; all: BOOLEAN);
    VAR m, p: Module; refcount: LONGINT;
  BEGIN m := modules();
    IF all THEN
      res := 1; resMsg := 'unloading "all" not yet supported'
    ELSE
      refcount := Heap.FreeModule(name);
      IF refcount = 0 THEN
        res := 0
      ELSE
        IF refcount < 0 THEN resMsg := "module not found"
        ELSE resMsg := "clients of this module exist"
        END;
        res := 1
      END
    END
  END Free;

END Modules.
