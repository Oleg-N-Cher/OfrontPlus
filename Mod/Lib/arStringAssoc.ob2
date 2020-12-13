(* Mapping from String -> String, used mainly by arConfiguration. *)

MODULE arStringAssoc;

IMPORT
	SL := arStringList,
	S := arStrings;

TYPE
	String* = SL.String;

	Assoc* = POINTER TO RECORD
		key* : String;
		value* : String;
		next* : Assoc;
	END;

PROCEDURE Find*(assoc : Assoc; key : ARRAY OF CHAR) : Assoc;
BEGIN
	WHILE assoc # NIL DO
		IF S.Equal(key, assoc.key^) THEN
			RETURN assoc;
		END;
		assoc := assoc.next;
	END;
	RETURN NIL;
END Find;

PROCEDURE FindValue*(assoc : Assoc; key : ARRAY OF CHAR) : Assoc;
BEGIN
	WHILE assoc # NIL DO
		IF S.Equal(key, assoc.value^) THEN
			RETURN assoc;
		END;
		assoc := assoc.next;
	END;
	RETURN NIL;
END FindValue;

(* add a possibly multi-valued association *)

PROCEDURE Set0*(VAR assoc : Assoc; key, value : ARRAY OF CHAR);
VAR binding : Assoc;
BEGIN
	NEW(binding);
	binding.key := SL.Copy(key);
	binding.value := SL.Copy(value);
	binding.next := assoc;
	assoc := binding;
END Set0;

(* add a single association *)

PROCEDURE Set*(VAR assoc : Assoc; key, value : ARRAY OF CHAR);
VAR binding : Assoc;
BEGIN
	binding := Find(assoc, key);
	IF binding # NIL THEN
		binding.value := SL.Copy(value);
	ELSE
		Set0(assoc, key, value);
	END;
END Set;

(* return the first value for key *)

PROCEDURE Get*(assoc : Assoc; key : ARRAY OF CHAR) : String;
VAR binding : Assoc;
BEGIN
	binding := Find(assoc, key);
	IF binding # NIL THEN
		RETURN binding.value;
	END;
	RETURN NIL;
END Get;

(* return key for value, or NIL if there is no association *)

PROCEDURE GetKey*(assoc : Assoc; value : ARRAY OF CHAR) : String;
VAR binding : Assoc;
BEGIN
	binding := FindValue(assoc, value);
	IF binding # NIL THEN
		RETURN binding.key;
	END;
	RETURN NIL;
END GetKey;

(* return list of all values for key: may be zero, one, or more elements *)

PROCEDURE GetAll*(assoc : Assoc; key : ARRAY OF CHAR) : SL.List;
VAR result : SL.List;
BEGIN
	result := NIL;
	WHILE assoc # NIL DO
		IF S.Equal(key, assoc.key^) THEN
			SL.Add(result, assoc.value)
		END;
		assoc := assoc.next;
	END;
	RETURN result;
END GetAll;

END arStringAssoc.
