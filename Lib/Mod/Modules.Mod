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
    resMsg*:    ARRAY 256 OF CHAR;
    imported*:  ModuleName;
    importing*: ModuleName;


(* Module and command lookup by name *)

  PROCEDURE -modules(): Module     "(Heap_Module)Heap_modules";
  (*PROCEDURE -setmodules(m: Module) "Heap_modules = m";*)

	PROCEDURE Append(IN b: ARRAY OF CHAR; VAR a: ARRAY OF CHAR);
		VAR i, j: INTEGER;
	BEGIN
		i := 0; WHILE a[i] # 0X DO INC(i) END;
		j := 0; WHILE b[j] # 0X DO a[i] := b[j]; INC(i); INC(j) END;
		a[i] := 0X
	END Append;

  PROCEDURE ThisMod* (IN name: ARRAY OF CHAR): Module;
    VAR m: Module; bodyname: ARRAY 64 OF CHAR; body: Command;
  BEGIN m := modules();
    WHILE (m # NIL) & (m.name # name) DO m := m.next END ;
    IF m # NIL THEN res := 0; resMsg := ""
    ELSE res := 1; COPY(name, importing);
      resMsg := ' module "'; Append(name, resMsg); Append('" not found', resMsg);
    END ;
    RETURN m
  END ThisMod;

  PROCEDURE ThisCommand* (mod: Module; IN name: ARRAY OF CHAR): Command;
    VAR c: Cmd;
  BEGIN c := mod.cmds;
    WHILE (c # NIL) & (c.name # name) DO c := c.next END ;
    IF c # NIL THEN res := 0; resMsg := ""; RETURN c.cmd
    ELSE res := 2; resMsg := ' command "'; COPY(name, importing);
      Append(mod.name, resMsg); Append(".", resMsg); Append(name, resMsg); Append('" not found', resMsg);
      RETURN NIL
    END
  END ThisCommand;

  PROCEDURE Free*(IN name: ARRAY OF CHAR; all: BOOLEAN);
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
