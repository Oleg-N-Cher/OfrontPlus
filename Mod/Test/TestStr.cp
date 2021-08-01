MODULE TestStr; IMPORT SYSTEM;

VAR
  arr: ARRAY 100 OF SHORTCHAR;
  dynarr: POINTER TO ARRAY OF SHORTCHAR;
  cstr: POINTER TO ARRAY [untagged] OF SHORTCHAR;

PROCEDURE Len (IN str: ARRAY OF SHORTCHAR);
  VAR len: INTEGER;
BEGIN
  len := LEN("string");
  len := LEN(cstr^$);
  len := LEN(str); len := LEN(str$);
  len := LEN(arr); len := LEN(arr$);
  len := LEN(dynarr^); len := LEN(dynarr^$);
END Len;

PROCEDURE Assign (str: ARRAY OF SHORTCHAR);
BEGIN
  cstr := "111111111"; cstr^ := "111111111";
  arr := cstr^$; str := cstr$; cstr := str;
  arr := dynarr^$; SYSTEM.COPY(dynarr^, arr);
  arr := "string"; SYSTEM.COPY("string", arr);
  arr := arr; arr := arr$; SYSTEM.COPY(arr, arr);
  dynarr := dynarr; dynarr^ := dynarr^$; SYSTEM.COPY(dynarr^, dynarr^);
  dynarr^ := arr$; SYSTEM.COPY(arr, dynarr^);
  dynarr^ := "string"; SYSTEM.COPY("string", dynarr^);
END Assign;

PROCEDURE Concat (str: ARRAY OF SHORTCHAR);
BEGIN
  cstr^ := "123" + cstr^; cstr^ := cstr^ + "321";
  arr := cstr^$ + "111"; dynarr^ := cstr^$ + "111";
  arr := dynarr^$ + "111" + arr;
  dynarr^ := dynarr^$ + "111" + arr;
  dynarr^ := "111" + dynarr^$ + arr;
  str := "111" + str$; str := str + "111";
END Concat;

END TestStr.
