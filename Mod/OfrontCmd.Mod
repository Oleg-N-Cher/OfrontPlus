MODULE OfrontCmd;	(* J. Templ 3.2.95 *)

	IMPORT
		Heap, Kernel,
		OPP := OfrontOPP, OPB := OfrontOPB, OPT := OfrontOPT,
		OPV := OfrontOPV, OPC := OfrontOPC, OPM := OfrontOPM;

	PROCEDURE Trap(sig: INTEGER);
	BEGIN
		Heap.FINALL;
		IF sig = 3 THEN Kernel.Exit(1)
		ELSE
			IF (sig = 4) & (Kernel.HaltCode = -15) THEN
				OPM.LogWStr(" --- Ofront+: internal error"); OPM.LogWLn
			END ;
			Kernel.Exit(2)
		END
	END Trap;

	PROCEDURE Module*(VAR done: BOOLEAN);
		VAR ext, new: BOOLEAN; p: OPT.Node;
	BEGIN
		OPV.Init; OPP.Module(p, OPM.opt);
		IF OPM.noerr THEN
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
		OPM.LogWLn; done := OPM.noerr;
		IF OPM.warning156 THEN OPM.Mark(-156, -1) END
	END Module;

	PROCEDURE Translate*;
		VAR done: BOOLEAN;
	BEGIN
		OPM.OpenPar("Ofront+ (TM) - Oberon family of languages to C Translator v1.0", "ofront+");
		(* gclock();   slightly faster translation but may lead to opening "too many files" *)

		LOOP
			OPM.Init("translating", done);
			IF ~done THEN
				IF ~OPM.noerr THEN (* source file not found *) Heap.FINALL; Kernel.Exit(1) END;
				EXIT (* end of argument vector *)
			END ;
			OPM.InitOptions;
			Heap.GC(FALSE);
			Module(done); (* Compile source to .c and .h files *)
			IF ~done THEN Heap.FINALL; Kernel.Exit(1) END
		END
	END Translate;

BEGIN
	Kernel.SetInterruptHandler(Trap);      (* interrupt *)
	Kernel.SetQuitHandler(Trap);           (* quit *)
	Kernel.SetBadInstructionHandler(Trap); (* illegal instruction, HALT *)
	OPB.typSize := OPV.TypSize; OPT.typSize := OPV.TypSize; Translate
END OfrontCmd.
