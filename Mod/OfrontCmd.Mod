MODULE OfrontCmd;	(* J. Templ 3.2.95 *)

	IMPORT
		SYSTEM, Unix, Kernel,
		OPP := OfrontOPP, OPB := OfrontOPB, OPT := OfrontOPT,
		OPV := OfrontOPV, OPC := OfrontOPC, OPM := OfrontOPM;

	PROCEDURE -signal(sig: LONGINT; func: Unix.SignalHandler)
		"signal(sig, func)";

	PROCEDURE -fin()
		"SYSTEM_FINALL()";

	PROCEDURE -halt(): LONGINT
		"SYSTEM_halt";

(*
	PROCEDURE -gclock()
		"SYSTEM_gclock = 1";
*)

	PROCEDURE Trap(sig, code: LONGINT; scp: Unix.SigCtxPtr);
	BEGIN fin();
		IF sig = 3 THEN Unix.Exit(0)
		ELSE
			IF (sig = 4) & (halt() = -15) THEN OPM.LogWStr(" --- ofront: internal error");  OPM.LogWLn END ;
			Unix.Exit(2)
		END
	END Trap;

	PROCEDURE Module*(VAR done: BOOLEAN);
		VAR ext, new: BOOLEAN; p: OPT.Node;
	BEGIN
		OPP.Module(p, OPM.opt);
		IF OPM.noerr THEN
			OPV.Init;
			OPV.AdrAndSize(OPT.topScope);
			OPT.Export(ext, new);
			IF OPM.noerr THEN
				OPM.OpenFiles(OPT.SelfName);
				OPC.Init;
				OPV.Module(p);
				IF OPM.noerr THEN
					IF (OPM.mainprog IN OPM.opt) & (OPM.modName # "SYSTEM") THEN
						OPM.DeleteNewSym; OPM.LogWStr("  main program")
					ELSE
						IF new THEN OPM.LogWStr("  new symbol file"); OPM.RegisterNewSym
						ELSIF ext THEN OPM.LogWStr("  extended symbol file"); OPM.RegisterNewSym
						END
					END
				ELSE OPM.DeleteNewSym
				END
			END
		END ;
		OPM.CloseFiles; OPT.Close;
		OPM.LogWLn; done := OPM.noerr
	END Module;

	PROCEDURE Translate*;
		VAR done: BOOLEAN;
	BEGIN
		OPM.OpenPar; (* gclock();   slightly faste rtranslation but may lead to opening "too many files" *)
		OPT.bytetyp.size := OPM.ByteSize;
		OPT.sysptrtyp.size := OPM.PointerSize;
		OPT.chartyp.size := OPM.CharSize;
		OPT.settyp.size := OPM.SetSize;
		OPT.realtyp.size := OPM.RealSize;
		OPT.inttyp.size := OPM.IntSize;
		OPT.linttyp.size := OPM.LIntSize;
		OPT.lrltyp.size := OPM.LRealSize;
		OPT.sinttyp.size := OPM.SIntSize;
		OPT.booltyp.size := OPM.BoolSize;
		LOOP
			OPM.Init(done);
			IF ~done THEN EXIT END ;
			OPM.InitOptions;
			Kernel.GC(FALSE);
			Module(done);
			IF ~done THEN Unix.Exit(1) END
		END
	END Translate;

BEGIN (*$MAIN*)
	signal(2, Trap);	(* interrupt *)
	signal(3, Trap);	(* quit *)
	signal(4, Trap);	(* illegal instruction, HALT *)
	OPB.typSize := OPV.TypSize; OPT.typSize := OPV.TypSize; Translate
END OfrontCmd.
