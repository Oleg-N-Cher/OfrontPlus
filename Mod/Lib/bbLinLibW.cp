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

	PROCEDURE- includelocale "#include <locale.h>";
	PROCEDURE- includestdio "#include <stdio.h>";
	PROCEDURE- includestdlib "#include <stdlib.h>";
	PROCEDURE- includewchar "#include <wchar.h>";
	PROCEDURE- includewctype "#include <wctype.h>";

	PROCEDURE- setlocale* (category: int; locale: Libc.PtrSTR): Libc.PtrSTR
		"(CHAR*)setlocale(category, (const char *)locale)";

	PROCEDURE- mbsinit* (VAR [nil] ps: mbstate_t): int
		"mbsinit((const mbstate_t *)ps)";

	PROCEDURE- wctomb* (s: Libc.PtrSTR; wchar: wchar_t): int
		"wctomb((char *)s, (const wchar_t *)wchar)";
	PROCEDURE- wcstombs* (s: Libc.PtrSTR; pwcs: PtrWSTR; n: size_t): size_t
		"(SYSTEM_ADRINT)wcstombs((char *)s, (const wchar_t *)pwcs, (size_t) n)";

	PROCEDURE- wcrtomb* (s: Libc.PtrSTR; wc: wchar_t; VAR [nil] ps: mbstate_t): size_t
		"(SYSTEM_ADRINT)wcrtomb((char *)s, (wchar_t)wc, (mbstate_t *)ps)";
	PROCEDURE- wcsrtombs* (dst: Libc.PtrSTR; VAR src: PtrWSTR; len: size_t; VAR [nil] ps: mbstate_t): size_t
		"(SYSTEM_ADRINT)wcsrtombs((char *)dst, (const wchar_t **)wc, (size_t)len, (mbstate_t *)ps)";
	PROCEDURE- wcsnrtombs* (dst: Libc.PtrSTR; VAR src: PtrWSTR; nwc: size_t; len: size_t; VAR [nil] ps: mbstate_t): size_t
		"(SYSTEM_ADRINT)wcsnrtombs((char *)dst, (const wchar_t **)src, (size_t)nwc, (size_t)len, (mbstate_t *)ps)";

	PROCEDURE- mbtowc* (pwc: PtrWSTR; s: Libc.PtrSTR; n: size_t): int
		"mbtowc((wchar_t *)pwc, (const char *)s, (size_t)n)";
	PROCEDURE- mbstowcs* (pwcs: PtrWSTR; s: Libc.PtrSTR; n: size_t): size_t
		"(SYSTEM_ADRINT)mbstowcs((wchar_t *)pwcs, (const char *)s, (size_t)n)";
	PROCEDURE- mblen* (s: Libc.PtrSTR; n: size_t): int
		"mblen((const char *)s, (size_t)n)";

	PROCEDURE- mbrtowc* (wc: PtrWSTR; s: Libc.PtrSTR; n: size_t; VAR [nil] mbs: mbstate_t): size_t
		"(SYSTEM_ADRINT)mbrtowc((wchar_t *)wc, (const char *)s, (size_t)n, (mbstate_t *)mbs)";
	PROCEDURE- mbsrtowcs* (dst: PtrWSTR; VAR src: Libc.PtrSTR; len: size_t; VAR [nil] ps:  mbstate_t): size_t
		"(SYSTEM_ADRINT)mbsrtowcs((wchar_t *)dst, (const char **)src, (size_t)len, (mbstate_t *)ps);";
	PROCEDURE- mbsnrtowcs* (dst: PtrWSTR; VAR src: Libc.PtrSTR; nmc: size_t; len: size_t; VAR [nil] ps: mbstate_t): size_t
		"(SYSTEM_ADRINT)mbsnrtowcs((wchar_t *)dst, (const char **)src, (size_t)nmc, (size_t)len, (mbstate_t *)ps)";
	PROCEDURE- mbrlen* (s: Libc.PtrSTR; n: size_t; VAR [nil] ps: mbstate_t): size_t
		"(SYSTEM_ADRINT)mbrlen((const char *)s, (size_t)n, (mbstate_t *)ps)";

	PROCEDURE- iswalpha* (wc: wint_t): int "iswalpha(wc)";
	PROCEDURE- iswlower* (wc: wint_t): int "iswlower(wc)";
	PROCEDURE- iswupper* (wc: wint_t): int "iswupper(wc)";
	PROCEDURE- towlower* (wc: wint_t): wint_t "towlower(wc)";
	PROCEDURE- towupper* (wc: wint_t): wint_t "towupper(wc)";

	PROCEDURE- wprintf* (fmt: PtrWSTR): int "wprintf((const wchar_t *)fmt)";
	PROCEDURE- fputws* (ws: PtrWSTR; fp: Libc.PtrFILE): int
		"fputws((const wchar_t *)ws, (FILE *)fp)";
	PROCEDURE- fgetws* (ws: PtrWSTR; n: int; fp: Libc.PtrFILE): PtrWSTR
		"(INTEGER *)fgetws((wchar_t *)ws, n, (FILE *)fp)";

END bbLinLibW.
