(* List of String, used mainly for arConfiguration and arPattern *)

MODULE arStringList;

(* enhanced ooc2Strings *)

IMPORT
	S := arStrings;

TYPE
	String* = S.StringPtr;
	Array* = POINTER TO ARRAY OF String;

	List* = POINTER TO RECORD
		name* : String;
		next* : List;
	END;

(* Make dynamic copy of String on the heap *)

PROCEDURE Copy*(name : ARRAY OF CHAR) : String;
VAR result : String;
BEGIN
	NEW(result, S.Length(name)+1);
	COPY(name, result^);
	RETURN result;
END Copy;

PROCEDURE Contains*(list : List; string : String) : BOOLEAN;
BEGIN
	WHILE list # NIL DO
		IF S.Equal(list.name^, string^) THEN RETURN TRUE END;
		list := list.next;
	END;
	RETURN FALSE;
END Contains;

PROCEDURE Add*(VAR list : List; string : String);
VAR element : List;
BEGIN
	NEW(element);
	element.name := string;
	element.next := list;
	list := element;
END Add;

PROCEDURE Reverse*(VAR list : List);
VAR l, this : List;
BEGIN
	l := list;
	list := NIL;
	WHILE l # NIL DO
		this := l;
		l := l.next;
		this.next := list;
		list := this;
	END;
END Reverse;

PROCEDURE ToArray*(list : List) : Array;
VAR 
	l : List;
	a : Array;
	length : LONGINT;
BEGIN
	l := list;
	length := 0;
	WHILE l # NIL DO
		INC(length);
		l := l.next;
	END;
	NEW(a, length);
	l := list;
	length := 0;
	WHILE l # NIL DO
		a[length] := l.name;
		INC(length);
		l := l.next;
	END;
	RETURN a;
END ToArray;

PROCEDURE Include*(VAR list : List; string : String);
BEGIN
	IF ~Contains(list, string) THEN
		Add(list, string);
	END;
END Include;

PROCEDURE Merge*(src : List; VAR dest : List);
BEGIN
	WHILE src # NIL DO
		Include(dest, src.name);
		src := src.next;
	END;
END Merge;

PROCEDURE ArrayContains*(array : Array; string : String) : BOOLEAN;
VAR i : LONGINT;
BEGIN
	FOR i := 0 TO LEN(array^)-1 DO
		IF S.Equal(array[i]^, string^) THEN RETURN TRUE END;
	END;
	RETURN FALSE;
END ArrayContains;

END arStringList.
