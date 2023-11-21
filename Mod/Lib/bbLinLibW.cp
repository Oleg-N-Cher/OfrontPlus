MODULE [foreign] bbLinLibW;

	(*
		GNU/Linux
		i386
	*)

	IMPORT SYSTEM, Libc := bbLinLibc;

	CONST
		LC_CTYPE* = 0;
		LC_NUMERIC* = 1;
		LC_TIME* = 2;
		LC_COLLATE* = 3;
		LC_MONETARY* = 4;
		LC_MESSAGES* = 5;
		LC_ALL* = 6;
		LC_PAPER* = 7;
		LC_NAME* = 8;
		LC_ADDRESS* = 9;
		LC_TELEPHONE* = 10;
		LC_MEASUREMENT* = 11;
		LC_IDENTIFICATION* = 12;

	TYPE
		(* int, wchar_t, wint_t, size_t, mbstate_t *)
		int* = INTEGER;
		wchar_t* = INTEGER;
		wint_t* = INTEGER;
		size_t* = SYSTEM.ADRINT;
		mbstate_t* = LONGINT;

		PtrWSTR* = POINTER [untagged] TO ARRAY [untagged] OF wchar_t;

	(*PROCEDURE- includelocale "#include <locale.h>";
	PROCEDURE- includestdio "#include <stdio.h>";
	PROCEDURE- includestdlib "#include <stdlib.h>";
	PROCEDURE- includewchar "#include <wchar.h>";
	PROCEDURE- includewctype "#include <wctype.h>";*)

	PROCEDURE setlocale* (category: int; locale: Libc.PtrSTR): Libc.PtrSTR;

	PROCEDURE mbsinit* (VAR [nil] ps: mbstate_t): int;

	PROCEDURE wctomb* (s: Libc.PtrSTR; wchar: wchar_t): int;
	PROCEDURE wcstombs* (s: Libc.PtrSTR; pwcs: PtrWSTR; n: size_t): size_t;

	PROCEDURE wcrtomb* (s: Libc.PtrSTR; wc: wchar_t; VAR [nil] ps: mbstate_t): size_t;
	PROCEDURE wcsrtombs* (dst: Libc.PtrSTR; VAR src: PtrWSTR; len: size_t; VAR [nil] ps: mbstate_t): size_t;
	PROCEDURE wcsnrtombs* (dst: Libc.PtrSTR; VAR src: PtrWSTR; nwc: size_t; len: size_t; VAR [nil] ps: mbstate_t): size_t;

	PROCEDURE mbtowc* (pwc: PtrWSTR; s: Libc.PtrSTR; n: size_t): int;
	PROCEDURE mbstowcs* (pwcs: PtrWSTR; s: Libc.PtrSTR; n: size_t): size_t;
	PROCEDURE mblen* (s: Libc.PtrSTR; n: size_t): int;

	PROCEDURE mbrtowc* (wc: PtrWSTR; s: Libc.PtrSTR; n: size_t; VAR [nil] mbs: mbstate_t): size_t;
	PROCEDURE mbsrtowcs* (dst: PtrWSTR; VAR src: Libc.PtrSTR; len: size_t; VAR [nil] ps:  mbstate_t): size_t;
	PROCEDURE mbsnrtowcs* (dst: PtrWSTR; VAR src: Libc.PtrSTR; nmc: size_t; len: size_t; VAR [nil] ps: mbstate_t): size_t;
	PROCEDURE mbrlen* (s: Libc.PtrSTR; n: size_t; VAR [nil] ps: mbstate_t): size_t;

	PROCEDURE iswalpha* (wc: wint_t): int;
	PROCEDURE iswlower* (wc: wint_t): int;
	PROCEDURE iswupper* (wc: wint_t): int;
	PROCEDURE towlower* (wc: wint_t): wint_t;
	PROCEDURE towupper* (wc: wint_t): wint_t;

	PROCEDURE wprintf* (fmt: PtrWSTR): int;
	PROCEDURE fputws* (ws: PtrWSTR; fp: Libc.PtrFILE): int;
	PROCEDURE fgetws* (ws: PtrWSTR; n: int; fp: Libc.PtrFILE): PtrWSTR;

END bbLinLibW.
