(*

A set of abstract data types.

This is useful for defining data resources for programs. Oberon lacks any
structured constants. Types implemented here can be read directly from JSON
strings or files, and can also be serialised to JSON text

Boxed types for Oberon scalars:

* Set      BaseSet = SET
* Boolean  BOOLEAN
* Real     BaseReal = LONGREAL
* Integer  BaseReal = LONGINT
* String   (backed by POINTER TO ARRAY OF CHAR)

Collection types:

* List     (variable length linked list)
* Record   (similar to LISP a-lists, and Python dictrionaries)
* Array    (fixed length, backed by POINTER TO ARRAY OF Object)

Object base type supports:

* Equals   test for equality
* Copy and DeepCopy
* Convert to String
* Read structured object from JSON string
* Write structured object to text or JSON

Note that this module contains 9 different types: Object, Set, Boolean, Real,
Integer, String, List, Record, Array. While you might think it would be
elegant to split into smaller modules, this is actually not a good idea.

1) There are recursive relationships between types due to their methods.
Other languages can handle recursive definitions between types in different
modules (eg. Java), but not Oberon. Examples are:
  Object.ToString -> String
  String.Split -> List
  List.Join -> String

2) Where these types are used they tend to be used together, and its
inconvenient to have to import all the modules separately.

*)

MODULE arValue;

IMPORT
	Text := arText, 
	CFormat := arCFormat, 
	S := arStrings; 

TYPE
	BaseSet* = SET;
	BaseReal* = LONGREAL;
	BaseInteger* = LONGINT;
	BaseString* = Text.String;

	Object* = POINTER TO ObjectDesc;
	ObjectDesc* = RECORD
	END;

	IntArray* = POINTER TO ARRAY OF LONGINT;
	StringArray* = POINTER TO ARRAY OF BaseString;
	ObjectArray* = POINTER TO ARRAY OF Object;

	String* = POINTER TO RECORD (ObjectDesc)
		value- : BaseString;
		length- : LONGINT;
	END;

	Boolean* = POINTER TO RECORD (ObjectDesc)
		value- : BOOLEAN;
	END;

	Integer* = POINTER TO RECORD (ObjectDesc)
		value- : BaseInteger;
	END;

	Real* = POINTER TO RECORD (ObjectDesc)
		value- : BaseReal;
	END;

	Set* = POINTER TO RECORD (ObjectDesc)
		value- : BaseSet;
	END;

	ListElement* = POINTER TO  RECORD
		value- : Object;
		next- : ListElement;
	END;

	List* = POINTER TO RECORD (ObjectDesc)
		first-, last- : ListElement;
		length- : LONGINT;
	END;

	Array* = POINTER TO RECORD (ObjectDesc)
		value-: ObjectArray;
		length-: LONGINT;
	END;

	RecordElement* = POINTER TO RECORD
		key- : String;
		value* : Object;
		next- : RecordElement;
	END;

	Record* = POINTER TO RECORD (ObjectDesc)
		scope- : RecordElement;
		length- : LONGINT;		(* number of keys for faster equality check *)
		last : RecordElement;
	END;
	
TYPE
	MapFunction* = PROCEDURE (o : Object) : Object;

PROCEDURE Min*(a, b : LONGINT) : LONGINT;
BEGIN
	IF a < b THEN RETURN a ELSE RETURN b END;
END Min;

PROCEDURE Max*(a, b : LONGINT) : LONGINT;
BEGIN
	IF a > b THEN RETURN a ELSE RETURN b END;
END Max;

PROCEDURE ^StringFromCharArray*(value-: ARRAY OF CHAR) : String;
PROCEDURE ^StringFromBuffer*(VAR buffer : Text.Buffer) : String;

(* ----- Object ----- *)

(* Format this object as text. For debugging, or conversion to a format like JSON *)

PROCEDURE (self : Object) Format*(VAR w : Text.Writer);
BEGIN
	(* abstract *)
END Format;

PROCEDURE (self : Object) Copy*() : Object;
BEGIN
END Copy;

PROCEDURE (self : Object) DeepCopy*() : Object;
BEGIN
	RETURN self.Copy();
END DeepCopy;

PROCEDURE (self : Object) ToString*() : String;
VAR buffer : Text.Buffer;
BEGIN
	buffer.Init(16);
	self.Format(buffer);
	RETURN StringFromBuffer(buffer);
END ToString;

PROCEDURE (self : Object) ToJSON*() : String;
VAR buffer : Text.Buffer;
BEGIN
	buffer.Init(16);
	buffer.quoteLiterals := TRUE;
	buffer.setAsList := TRUE;
	self.Format(buffer);
	RETURN StringFromBuffer(buffer);
END ToJSON;

PROCEDURE (self : Object) Equals*(other : Object) : BOOLEAN;
BEGIN
	RETURN FALSE;
END Equals;

PROCEDURE ToString*(self : Object) : String;
BEGIN
	IF self = NIL THEN RETURN StringFromCharArray("null") END;
	RETURN self.ToString();
END ToString;

PROCEDURE ToJSON*(self : Object) : String;
BEGIN
	IF self = NIL THEN RETURN StringFromCharArray("null") END;
	RETURN self.ToJSON();
END ToJSON;

PROCEDURE Format*(VAR w : Text.Writer; o : Object);
BEGIN
	IF o = NIL THEN
		w.String("null");
	ELSE
		o.Format(w)
	END;
END Format;

PROCEDURE Copy*(o : Object) : Object;
BEGIN
	IF o = NIL THEN 
		RETURN NIL;
	ELSE
		RETURN o.Copy();
	END
END Copy;

PROCEDURE DeepCopy*(o : Object) : Object;
BEGIN
	IF o = NIL THEN 
		RETURN NIL;
	ELSE
		RETURN o.DeepCopy();
	END
END DeepCopy;

PROCEDURE Equals*(a, b : Object) : BOOLEAN;
BEGIN
	IF (a=NIL) & (b=NIL) THEN
		RETURN TRUE;
	ELSIF (a=NIL) OR (b=NIL) THEN
		RETURN FALSE;
	ELSE
		RETURN a.Equals(b);
	END;
END Equals;

PROCEDURE FormatQ*(VAR w : Text.Writer; o : Object);
VAR wasQuoted : BOOLEAN;
BEGIN
	wasQuoted := w.quoteLiterals;
	w.quoteLiterals := TRUE;
	Format(w, o);
	w.quoteLiterals := wasQuoted;
END FormatQ;

(* ----- String ----- *)

PROCEDURE StringFromCharArray*(value-: ARRAY OF CHAR) : String;
VAR s : String;
BEGIN
	NEW(s); 
	s.length := S.Length(value);
	NEW(s.value, s.length+1);
	COPY(value, s.value^);
	RETURN s;
END StringFromCharArray;

PROCEDURE char*(value : CHAR) : String;
VAR array : ARRAY 2 OF CHAR;
BEGIN
	array[0] := value; array[1] := 0X;
	RETURN StringFromCharArray(array);
END char;

(* Create a String from an existing dynamic array. Uses the array directly
without making a copy *)

PROCEDURE StringFromCharPointer*(value : BaseString) : String;
VAR s : String;
BEGIN
	NEW(s);
	s.value := value;
	s.length := S.Length(value^);
	RETURN s;
END StringFromCharPointer;

(* Create a String from an existing String.Buffer *)

PROCEDURE StringFromBuffer*(VAR buffer : Text.Buffer) : String;
BEGIN
	RETURN StringFromCharPointer(buffer.GetString());
END StringFromBuffer;

PROCEDURE (self : String) Equals*(other : Object) : BOOLEAN;
BEGIN
	WITH other : String DO
		RETURN S.Equal(self.value^, other.value^)
	ELSE
		RETURN FALSE
	END
END Equals;

PROCEDURE (self : String) CopyT*() : String;
BEGIN
	RETURN StringFromCharPointer(self.value);
END CopyT;

PROCEDURE (self : String) Copy*() : Object;
BEGIN
	RETURN self.CopyT();
END Copy;

PROCEDURE (self : String) Format*(VAR w : Text.Writer);
BEGIN
	w.StringLiteral(self.value^)
END Format;

PROCEDURE (self : String) Concat*(arg : String) : String;
VAR result : String;
BEGIN
	NEW(result);
	result.length := self.length + arg.length;
	NEW(result.value, result.length+1);
	COPY(self.value^, result.value^);
	S.Append(arg.value^, result.value^);
	RETURN result;
END Concat;

PROCEDURE (self : String) ToInteger*() : LONGINT;
VAR
	i, res : LONGINT;
BEGIN
	i := 0;
	res := CFormat.StringToInt(self.value^, i);
	RETURN i;
END ToInteger;

PROCEDURE (self : String) ToReal*() : LONGREAL;
VAR
	res : LONGINT; value : LONGREAL;
BEGIN
	value := 0;
	res := CFormat.StringToReal(self.value^, value);
	RETURN value;
END ToReal;

PROCEDURE (self : String) Extract*(start, count : LONGINT) : String;
VAR 
	result : BaseString;
	i : LONGINT;
BEGIN
	IF start < 0 THEN
		start := self.length + start;
	END;
	start := Min(self.length, Max(start, 0));
	count := Min(Max(count, 0), self.length-start);
	NEW(result, count+1);
	FOR i := 0 TO count-1 DO
		result[i] := self.value[i+start]
	END;
	result[count] := 0X;
	RETURN StringFromCharPointer(result);
END Extract;

PROCEDURE (self : String) Compare*(value-: ARRAY OF CHAR) : SHORTINT;
BEGIN
	RETURN S.Compare(self.value^, value);
END Compare;

PROCEDURE (self : String) EndsWith*(value-: ARRAY OF CHAR) : BOOLEAN;
VAR i, length : LONGINT;
BEGIN
	length := S.Length(value);
	IF self.length < length THEN RETURN FALSE END;
	FOR i := 0 TO length-1 DO
		IF value[i] # self.value[self.length-length+i] THEN RETURN FALSE END;
	END;
	RETURN TRUE;
END EndsWith;

PROCEDURE (self : String) StartsWith*(value-: ARRAY OF CHAR) : BOOLEAN;
VAR i, length : LONGINT;
BEGIN
	length := S.Length(value);
	IF self.length < length THEN RETURN FALSE END;
	FOR i := 0 TO length-1 DO
		IF value[i] # self.value[i] THEN RETURN FALSE END;
	END;
	RETURN TRUE;
END StartsWith;

(* ----- Boolean ----- *)

PROCEDURE NewBoolean*(value : BOOLEAN) : Boolean;
VAR b : Boolean;
BEGIN
	NEW(b); b.value := value; RETURN b;
END NewBoolean;

PROCEDURE ParseBool*(value-: ARRAY OF CHAR) : Boolean;
BEGIN
	IF value = "true" THEN
		RETURN NewBoolean(TRUE);
	ELSIF value = "false" THEN
		RETURN NewBoolean(FALSE);
	END;
	RETURN NIL;
END ParseBool;

PROCEDURE (self : Boolean) CopyT*() : Boolean;
BEGIN
	RETURN NewBoolean(self.value);
END CopyT;

PROCEDURE (self : Boolean) Copy*() : Object;
BEGIN
	RETURN self.CopyT();
END Copy;

PROCEDURE (self : Boolean) Equals*(other : Object) : BOOLEAN;
BEGIN
	WITH other : Boolean DO
		RETURN self.value = other.value
	ELSE
		RETURN FALSE
	END
END Equals;

PROCEDURE (self : Boolean) Format*(VAR w : Text.Writer);
BEGIN
	w.Boolean(self.value);
END Format;

(* ----- Set ----- *)

PROCEDURE NewSet*(value : BaseSet) : Set;
VAR s : Set;
BEGIN
	NEW(s); s.value := value; RETURN s;
END NewSet;

PROCEDURE (self : Set) Equals*(other : Object) : BOOLEAN;
BEGIN
	WITH other : Set DO
		RETURN self.value = other.value
	ELSE
		RETURN FALSE
	END
END Equals;

PROCEDURE (self : Set) CopyT*() : Set;
BEGIN
	RETURN NewSet(self.value);
END CopyT;

PROCEDURE (self : Set) Copy*() : Object;
BEGIN
	RETURN self.CopyT();
END Copy;

PROCEDURE (self : Set) Format*(VAR w : Text.Writer);
BEGIN
	w.Set(self.value)
END Format;

(* ----- Integer ----- *)

PROCEDURE NewInteger*(value : BaseInteger) : Integer;
VAR i : Integer;
BEGIN
	NEW(i); i.value := value; RETURN i;
END NewInteger;

PROCEDURE ParseInt*(value-: ARRAY OF CHAR) : Integer;
VAR
	i : BaseInteger;
	res : LONGINT;
BEGIN
	res := CFormat.StringToInt(value, i);
	IF res = 1 THEN
		RETURN NewInteger(i)
	END;
	RETURN NIL;
END ParseInt;

PROCEDURE (self : Integer) Equals*(other : Object) : BOOLEAN;
BEGIN
	WITH other : Integer DO
		RETURN self.value = other.value
	ELSE
		RETURN FALSE
	END
END Equals;

PROCEDURE (self : Integer) CopyT*() : Integer;
BEGIN
	RETURN NewInteger(self.value);
END CopyT;

PROCEDURE (self : Integer) Copy*() : Object;
BEGIN
	RETURN self.CopyT();
END Copy;

PROCEDURE (self : Integer) Format*(VAR w : Text.Writer);
BEGIN
	w.Integer(self.value)
END Format;

(* ----- Real ----- *)

(* TODO: fix string representation *)

PROCEDURE NewReal*(value : BaseReal) : Real;
VAR r : Real;
BEGIN
	NEW(r); r.value := value; RETURN r;
END NewReal;

PROCEDURE ParseReal*(value-: ARRAY OF CHAR) : Real;
VAR
	r : BaseReal;
	res : LONGINT;
BEGIN
	res := CFormat.StringToReal(value, r);
	IF res = 1 THEN
		RETURN NewReal(r)
	END;
	RETURN NIL;
END ParseReal;

PROCEDURE (self : Real) Equals*(other : Object) : BOOLEAN;
BEGIN
	WITH other : Real DO
		RETURN self.value = other.value
	ELSE
		RETURN FALSE
	END
END Equals;

PROCEDURE (self : Real) CopyT*() : Real;
BEGIN
	RETURN NewReal(self.value);
END CopyT;

PROCEDURE (self : Real) Copy*() : Object;
BEGIN
	RETURN self.CopyT();
END Copy;

PROCEDURE (self : Real) Format*(VAR w : Text.Writer);
BEGIN
	w.Real(self.value);
END Format;

(* ---- List ----- *)

(* List of objects, stored in a linked list. 
TODO: doubly linked list would be better for insertion / deletion
*)

PROCEDURE NewList*() : List;
VAR
	l : List;
BEGIN
	NEW(l);
	l.first := NIL;
	l.last := NIL;
	l.length := 0;
	RETURN l;
END NewList;

PROCEDURE (self : List) Equals*(other : Object) : BOOLEAN;
VAR a, b : ListElement;
BEGIN
	WITH other : List DO
		IF self.length # other.length THEN RETURN FALSE END;
		a := self.first; b := other.first;
		WHILE (a # NIL) & (b # NIL) DO
			IF ~Equals(a.value, b.value) THEN RETURN FALSE END;
			a := a.next; b := b.next;
		END;
		ASSERT(a = b);
		RETURN TRUE;
	ELSE
		RETURN FALSE
	END
END Equals;

PROCEDURE (self : List) Format*(VAR w : Text.Writer);
VAR 
	n : ListElement;
BEGIN
	w.Char("[");
	n := self.first;
	WHILE n # NIL DO
		FormatQ(w, n.value);
		n := n.next;
		IF n # NIL THEN w.Char(",") END;
	END;
	w.Char("]");
END Format;

PROCEDURE (l : List) IndexOf*(value : Object) : LONGINT;
VAR i : LONGINT; a : ListElement;
BEGIN
	i := 0;
	a := l.first;
	WHILE a # NIL DO
		IF Equals(value, a.value) THEN RETURN i END;
		INC(i);
		a := a.next;
	END;
	RETURN -1
END IndexOf;

PROCEDURE (self : List) Append*(o : Object);
VAR node : ListElement;
BEGIN
	NEW(node); node.value := o; node.next := NIL;
	IF self.length = 0 THEN
		self.first := node;
	ELSE
		self.last.next := node;
	END;
	self.last := node;
	INC(self.length);
END Append;

PROCEDURE (self : List) Concat*(value : List) : List;
VAR
	result : List;
	e : ListElement;
BEGIN
	result := NewList();
	e := self.first;
	WHILE e # NIL DO
		result.Append(e.value);
		e := e.next;
	END;
	e := value.first;
	WHILE e # NIL DO
		result.Append(e.value);
		e := e.next;
	END;
	RETURN result;
END Concat;

PROCEDURE (self : List) Prepend*(o : Object);
VAR node : ListElement;
BEGIN
	NEW(node); node.value := o; node.next := self.first;
	IF self.length = 0 THEN
		self.last := node;
	END;
	self.first := node;
	INC(self.length);
END Prepend;

PROCEDURE (self : List) Extend*(value : List);
VAR e : ListElement;
BEGIN
	e := value.first;
	WHILE e # NIL DO
		self.Append(e.value);
		e := e.next;
	END;
END Extend;

PROCEDURE (self : List) ToIntArray*() : IntArray;
VAR 
	i, count : LONGINT;
	it : ListElement;
	result : IntArray;
BEGIN
	count := 0;
	it := self.first;
	WHILE it # NIL DO
		IF it.value IS Integer THEN
			INC(count);
		END;
		it := it.next;
	END;
	NEW(result, count);
	it := self.first;
	i := 0;
	WHILE it # NIL DO
		IF it.value IS Integer THEN
			result[i] := it.value(Integer).value;
			INC(i);
		END;
		it := it.next;
	END;
	ASSERT(i = count);
	RETURN result;
END ToIntArray;

PROCEDURE (self : List) ToStringArray*() : StringArray;
VAR 
	i, count : LONGINT;
	it : ListElement;
	result : StringArray;
BEGIN
	count := 0;
	it := self.first;
	WHILE it # NIL DO
		IF it.value IS String THEN
			INC(count);
		END;
		it := it.next;
	END;
	NEW(result, count);
	it := self.first;
	i := 0;
	WHILE it # NIL DO
		IF it.value IS String THEN
			result[i] := it.value(String).value;
			INC(i);
		END;
		it := it.next;
	END;
	ASSERT(i = count);
	RETURN result;
END ToStringArray;

PROCEDURE (self : List) ToObjectArray*() : ObjectArray;
VAR 
	i : LONGINT;
	it : ListElement;
	result : ObjectArray;
BEGIN
	NEW(result, self.length);
	it := self.first;
	i := 0;
	WHILE it # NIL DO
		result[i] := it.value;
		INC(i);
		it := it.next;
	END;
	ASSERT(i = self.length);
	RETURN result;
END ToObjectArray;

PROCEDURE (self : List) ToArray() : Array;
VAR a : Array;
BEGIN
	NEW(a);
	a.length := self.length;
	a.value := self.ToObjectArray();
	RETURN a;
END ToArray;

PROCEDURE (self : List) Get*(index : LONGINT) : Object;
VAR it : ListElement;
BEGIN
	ASSERT((index >= 0) & (index < self.length));
	it := self.first;
	WHILE index > 0 DO
		it := it.next; DEC(index);
	END;
	RETURN it.value;
END Get;

PROCEDURE (self : List) Set*(index : LONGINT; value : Object);
VAR it : ListElement;
BEGIN
	ASSERT((index >= 0) & (index < self.length));
	it := self.first;
	WHILE index > 0 DO
		it := it.next; DEC(index);
	END;
	it.value := value;
END Set;

PROCEDURE (self : List) Join*(separator-: ARRAY OF CHAR) : String;
VAR
	b : Text.Buffer;
	it : ListElement;
BEGIN
	b.Init(32);
	it := self.first;
	WHILE it # NIL DO
		Format(b, it.value);
		it := it.next;
		IF it # NIL THEN b.String(separator) END
	END;
	RETURN StringFromBuffer(b);
END Join;

PROCEDURE (self : List) Map*(func : MapFunction) : List;
VAR 
	funcResult : Object;
	result : List;
	it : ListElement;
BEGIN
	result := NewList();
	it := self.first;
	WHILE it # NIL DO
		funcResult := func(it.value);
		IF funcResult # NIL THEN
			result.Append(funcResult);
		END;
		it := it.next;
	END;
END Map;

PROCEDURE (self : String) Split*(separator : CHAR) : List;
VAR
	l : List;
	begin, i : LONGINT;
BEGIN
	l := NewList();
	begin := 0;
	FOR i := 0 TO self.length-1 DO
		IF self.value[i] = separator THEN
			l.Append(self.Extract(begin, i-begin));
			begin := i+1;
		END;
	END;
	IF begin <= self.length THEN
		l.Append(self.Extract(begin, self.length-begin))
	END;
	RETURN l;
END Split;

PROCEDURE (self : List) CopyT*(deep : BOOLEAN) : List;
VAR
	e : ListElement;
	l : List;
	value : Object;
BEGIN
	l := NewList();
	e := self.first;
	WHILE e # NIL DO
		value := e.value;
		IF deep THEN value := DeepCopy(value) END;
		l.Append(value);
		e := e.next;
	END;
	RETURN l;
END CopyT;

PROCEDURE (self : List) Copy*() : Object;
BEGIN
	RETURN self.CopyT(FALSE);
END Copy;

PROCEDURE (self : List) DeepCopy*() : Object;
BEGIN
	RETURN self.CopyT(TRUE);
END DeepCopy;

(* ----- Record ----- *)

(* Mapping from String -> Object. Mappings are stored in a linked list. Not
very efficient, but easy to implement. Originally used a LISP-style association
list, but that doesn't well handle empty mappings. 
TODO: This would be better as a hash table. *)

PROCEDURE NewRecord*() : Record;
VAR r : Record;
BEGIN
	NEW(r);
	r.scope := NIL;
	r.last := NIL;
	r.length := 0;
	RETURN r;
END NewRecord;

(* Return the value associated with <key> *)

PROCEDURE (self : Record) Get*(key-: ARRAY OF CHAR) : Object;
VAR elem : RecordElement;
BEGIN
	elem := self.scope;
	WHILE (elem # NIL) & (elem.key.Compare(key) # 0) DO
		elem := elem.next;
	END;
	IF elem # NIL THEN
		RETURN elem.value
	END;
	RETURN NIL
END Get;

(* Set the <value> associated with <key>. If an existing association exists,
this will be replacecd *)

PROCEDURE (self : Record) Bind(key-: ARRAY OF CHAR; value : Object);
VAR elem : RecordElement;
BEGIN
	(* add new key/value binding, preserving order *)
	NEW(elem);
	elem.key := StringFromCharArray(key);
	elem.value := value;
	elem.next := NIL;
	IF self.scope = NIL THEN
		self.scope := elem;
	ELSE
		self.last.next := elem;
	END;
	self.last := elem;
	INC(self.length);
END Bind;

PROCEDURE (self : Record) Equals*(value : Object) : BOOLEAN;
VAR elem : RecordElement; 
BEGIN
	WITH value : Record DO
		IF value.length # self.length THEN RETURN FALSE END;
		elem := self.scope;
		WHILE elem # NIL DO
			IF ~Equals(elem.value, value.Get(elem.key.value^)) THEN RETURN FALSE END;
			elem := elem.next;
		END;
		RETURN TRUE;
	ELSE
		RETURN FALSE;
	END;
END Equals;

PROCEDURE (self : Record) Set*(key-: ARRAY OF CHAR; value : Object);
VAR elem : RecordElement;
BEGIN
	elem := self.scope;
	WHILE (elem # NIL) & (elem.key.Compare(key) # 0) DO
		elem := elem.next;
	END;
	IF elem = NIL THEN
		(* create new binding *)
		self.Bind(key, value);
	ELSE
		(* replace existing key binding with new value *)
		elem.value := value;
	END;
END Set;

PROCEDURE (self : Record) Map*(func : MapFunction) : Record;
VAR
	r : Record;
	e : RecordElement;
	value : Object;
BEGIN
	r := NewRecord();
	e := self.scope;
	WHILE e # NIL DO
		value := func(e.value);
		IF value # NIL THEN
			r.Bind(e.key.value^, value);
		END;
		e := e.next;
	END;
	RETURN r;
END Map;

PROCEDURE (self : Record) CopyT*(deep : BOOLEAN) : Record;
VAR
	r : Record;
	e : RecordElement;
	value : Object;
BEGIN
	r := NewRecord();
	e := self.scope;
	WHILE e # NIL DO
		value := e.value;
		IF deep THEN value := DeepCopy(value) END;
		r.Bind(e.key.value^, value);
		e := e.next;
	END;
	RETURN r;
END CopyT;

PROCEDURE (self : Record) Copy*() : Object;
BEGIN
	RETURN self.CopyT(FALSE);
END Copy;

PROCEDURE (self : Record) DeepCopy*() : Object;
BEGIN
	RETURN self.CopyT(TRUE);
END DeepCopy;

PROCEDURE (self : Record) Format*(VAR w : Text.Writer);
VAR 
	elem : RecordElement;
BEGIN
	w.Char("{");
	elem := self.scope;
	WHILE elem # NIL DO
		Format(w, elem.key);
		w.Char(":");
		FormatQ(w, elem.value);
		elem := elem.next;
		IF elem # NIL THEN w.Char(",") END;
	END;
	w.Char("}");
END Format;

(* ----- Array ------ *)

(* List of objects stored as an Object array *)

PROCEDURE NewArray(length : LONGINT) : Array;
VAR a : Array;
BEGIN
	NEW(a);
	a.length := length;
	NEW(a.value, length);
	RETURN a;
END NewArray;

PROCEDURE (self : Array) CopyT(deep : BOOLEAN) : Array;
VAR a : Array; i : LONGINT; value : Object;
BEGIN
	a := NewArray(self.length);
	FOR i := 0 TO self.length-1 DO
		value := self.value[i];
		IF deep THEN value := DeepCopy(value) END;
		a.value[i] := value;
	END;
END CopyT;

PROCEDURE (self : Array) Copy*() : Object;
BEGIN
	RETURN self.CopyT(FALSE);
END Copy;

PROCEDURE (self : Array) DeepCopy*() : Object;
BEGIN
	RETURN self.CopyT(FALSE);
END DeepCopy;

PROCEDURE (self : Array) Format*(VAR w : Text.Writer);
VAR
	i : LONGINT;
BEGIN
	w.Char("[");
	FOR i := 0 TO self.length-1 DO
		IF i > 0 THEN w.Char(",") END;
		FormatQ(w, self.value[i]);
	END;
	w.Char("]");
END Format;

PROCEDURE (self : Array) Equals*(value : Object) : BOOLEAN;
VAR i : LONGINT;
BEGIN
	WITH value : Array DO
		IF self.length # value.length THEN RETURN FALSE END;
		FOR i := 0 TO self.length-1 DO
			IF ~Equals(self.value[i], value.value[i]) THEN RETURN FALSE END;
		END;
		RETURN TRUE;
	ELSE
		RETURN FALSE
	END
END Equals;

PROCEDURE (self : Array) Get*(index : LONGINT) : Object;
BEGIN
	ASSERT((index >= 0) & (index < self.length));
	RETURN self.value[index];
END Get;

PROCEDURE (self : Array) Set*(index : LONGINT; value : Object) : Object;
BEGIN
	ASSERT((index >= 0) & (index < self.length));
	self.value[index] := value;
END Set;

(* ----- Miscellaneous ----- *)

PROCEDURE Split*(value-: ARRAY OF CHAR; separator : CHAR) : List;
VAR s : String;
BEGIN
	s := StringFromCharArray(value);
	RETURN s.Split(separator);
END Split;

(* ----- Convenience Constructors ----- *)

PROCEDURE list*() : List;
BEGIN
	RETURN NewList();
END list;

PROCEDURE record*() : Record;
BEGIN
	RETURN NewRecord();
END record;

PROCEDURE set*(value : BaseSet) : Set;
BEGIN
	RETURN NewSet(value);
END set;

PROCEDURE bool*(value : BOOLEAN) : Boolean;
BEGIN
	RETURN NewBoolean(value);
END bool;

PROCEDURE int*(value : BaseInteger) : Integer;
BEGIN
	RETURN NewInteger(value);
END int;

PROCEDURE real*(value : BaseReal) : Real;
BEGIN
	RETURN NewReal(value);
END real;

PROCEDURE array2*(a, b : Object) : Array;
VAR result : Array;
BEGIN
	result := NewArray(2);
	result.value[0] := a;
	result.value[1] := b;
	RETURN result;
END array2;

PROCEDURE array3*(a, b, c : Object) : Array;
VAR result : Array;
BEGIN
	result := NewArray(3);
	result.value[0] := a;
	result.value[1] := b;
	result.value[2] := c;
	RETURN result;
END array3;

PROCEDURE array4*(a, b, c, d : Object) : Array;
VAR result : Array;
BEGIN
	result := NewArray(3);
	result.value[0] := a;
	result.value[1] := b;
	result.value[2] := c;
	result.value[3] := d;
	RETURN result;
END array4;

(* construct string from CHAR ARRAY *)
PROCEDURE str*(value-: ARRAY OF CHAR) : String;
BEGIN
	RETURN StringFromCharArray(value);
END str;

(* construct string from CHAR ARRAY POINTER *)
PROCEDURE strp*(value : BaseString) : String;
BEGIN
	RETURN StringFromCharPointer(value);
END strp;

(* construct string from Text.Buffer *)
PROCEDURE strb*(VAR value : Text.Buffer) : String;
BEGIN
	RETURN StringFromBuffer(value);
END strb;

END arValue.

