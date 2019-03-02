(**************************************************************************

:Program.    Lists.mod
:Author.     Fridtjof Siebert, Hartmut Goebel [hG]
:Language.   Oberon
:Translator. AmigaOberon V2.00
:History.    V1.0, 17-Jun-90 Fridtjof Siebert
:History.    V1.1, 10-Jan-91 H.Goebel: AddSection...,GoForw./Backw.
:History.    V1.2, 28-Mar-91   [hG] SetMark, ..Area..->..Mark..
:History.    V1.3, 30 Sep 1991 [hG] + GetPred, GetSucc, Swap
:History.    V1.4, 17 Oct 1991 [hG] + IsElement
:History.    V1.4b 21 Oct 1991 [hG] - Bug in Swap
:Date.       21 Oct 1991 12:17:01

  ftp://ftp.uni-stuttgart.de/pub/systems/amiga/amok/amok059/Lists1.4e.lha

**************************************************************************)

MODULE EcoLists;

TYPE
  NodePtr* = POINTER TO Node;
  Node* = RECORD
            next,prev: NodePtr;
          END;
  List* = RECORD
            head : NodePtr;
            tail : NodePtr;
            remallowed: SHORTINT;
          END;
  Mark* = List;

  DoProc * = PROCEDURE(n: NodePtr);

(* Die DoProc darf Remove(), RemHead() und RemTail() nicht benutzen. *)


PROCEDURE Init*(VAR list: List);
BEGIN
  list.head := NIL;
  list.tail := NIL;
  list.remallowed := 0;
END Init;

(*------ Add ------------------------------*)

PROCEDURE AddHead*(VAR list: List; n: NodePtr);
BEGIN
  n.next := list.head;
  n.prev := NIL;
  IF n.next=NIL THEN list.tail   := n;
                ELSE n.next.prev := n END;
  list.head := n;
END AddHead;


PROCEDURE AddTail*(VAR list: List; n: NodePtr);
BEGIN
  n.prev := list.tail;
  n.next := NIL;
  IF n.prev=NIL THEN list.head   := n;
                ELSE n.prev.next := n END;
  list.tail := n;
END AddTail;


PROCEDURE AddBefore*(VAR list: List;
                         n,x: NodePtr);
(* fügt n vor x in die Liste ein *)

BEGIN
  n.prev := x.prev;
  n.next := x;
  x .prev := n;
  IF n.prev=NIL THEN list.head   := n
                ELSE n.prev.next := n END;
END AddBefore;


PROCEDURE AddBehind*(VAR list: List;
                         n,x: NodePtr);
(* fügt n hinter x in die Liste ein *)

BEGIN
  n.next := x.next;
  n.prev := x;
  x .next := n;
  IF n.next=NIL THEN list.tail   := n
                ELSE n.next.prev := n END;
END AddBehind;

(*------ Remove ---------------------------*)

PROCEDURE Remove*(VAR list: List; n: NodePtr);
BEGIN
  IF n#NIL THEN
    IF list.remallowed # 0 THEN HALT(20) END;
    IF n.next#NIL THEN n.next.prev := n.prev ELSE list.tail := n.prev END;
    IF n.prev#NIL THEN n.prev.next := n.next ELSE list.head := n.next END;
  END;
END Remove;


PROCEDURE RemHead*(VAR list: List): NodePtr;
VAR n: NodePtr;
BEGIN
  n := list.head; Remove(list,n); RETURN n;
END RemHead;


PROCEDURE RemTail*(VAR list: List): NodePtr;
VAR n: NodePtr;
BEGIN
  n := list.tail; Remove(list,n); RETURN n;
END RemTail;

(*------ Do Forward/Backward --------------*)

PROCEDURE DoForward*(VAR list: List; proc: DoProc);
VAR n: NodePtr;
BEGIN
  INC(list.remallowed);
  n := list.head; WHILE n#NIL DO proc(n); n := n.next END;
  DEC(list.remallowed);
END DoForward;


PROCEDURE DoBackward*(VAR list: List; proc: DoProc);
VAR n: NodePtr;
BEGIN
  INC(list.remallowed);
  n := list.tail; WHILE n#NIL DO proc(n); n := n.prev END;
  DEC(list.remallowed);
END DoBackward;

(*------ Elements -------------------------*)

PROCEDURE Next*(VAR n: NodePtr): BOOLEAN;
BEGIN
  n := n.next;
  RETURN n#NIL;
END Next;


PROCEDURE Previous*(VAR n: NodePtr): BOOLEAN;
BEGIN
  n := n.prev;
  RETURN n#NIL;
END Previous;


PROCEDURE Succ*(VAR n: NodePtr);
BEGIN
  n := n.next;
END Succ;


PROCEDURE Pred*(VAR n: NodePtr);
BEGIN
  n := n.prev;
END Pred;


PROCEDURE GetSucc*(n: NodePtr): NodePtr;
BEGIN
  RETURN n.next;
END GetSucc;


PROCEDURE GetPred*(n: NodePtr): NodePtr;
BEGIN
  RETURN n.prev;
END GetPred;


PROCEDURE Head*(VAR list: List): NodePtr;
BEGIN
  RETURN list.head;
END Head;


PROCEDURE Tail*(VAR list: List): NodePtr;
BEGIN
  RETURN list.tail;
END Tail;

(*------ Go Forward/Backward --------------*)

PROCEDURE GoForward*(list: List; VAR n: NodePtr; num: INTEGER);
BEGIN
  WHILE (num>0) & (n#NIL) DO
    n := n.next;
    DEC(num);
  END;
  IF n=NIL THEN n:=list.tail; END;
END GoForward;


PROCEDURE GoBackward*(list: List; VAR n: NodePtr; num: INTEGER);
BEGIN
  WHILE (num>0) & (n#NIL) DO
    n := n.prev;
    DEC(num);
  END;
  IF n=NIL THEN n:=list.head; END;
END GoBackward;

(*------ misc -----------------------------*)

PROCEDURE Empty*(VAR list: List): BOOLEAN;
BEGIN
  RETURN list.head=NIL
END Empty;


PROCEDURE IsElement*(VAR list: List; e: NodePtr): BOOLEAN;
VAR
  n: NodePtr;
BEGIN
  n := list.head;
  WHILE n # NIL DO
    IF n = e THEN RETURN TRUE; END;
    n := n.next;
  END;
  RETURN FALSE;
END IsElement;


PROCEDURE CountElements*(VAR list: List): INTEGER;
VAR
  i: INTEGER;
  n: NodePtr;
BEGIN
  i := 0;
  n := list.head;
  WHILE n#NIL DO n := n.next; INC(i) END;
  RETURN i;
END CountElements;


PROCEDURE Swap*(VAR list: List; a,b: NodePtr);
VAR
  c: NodePtr;
BEGIN
  c := a.next;
  IF b.next # a THEN  (* wird sonst an der gleichen Stelle wieder eingefügt *)
    Remove(list,a);
    AddBehind(list,a,b);
  END;
  IF c # b THEN       (* b war Succ(a) *)
    Remove(list,b);
    IF c = NIL THEN
      AddTail(list,b);
    ELSE
      AddBefore(list,b,c);
    END;
  END;
END Swap;

(*------ marks and things around ----------*)

PROCEDURE AddMarkBefore*(VAR list: List; mark: Mark; x: NodePtr);
(* fügt mark vor x in die Liste ein *)

BEGIN
  mark.head.prev := x.prev;
  mark.tail.next := x;
  x.prev := mark.tail;
  IF mark.head.prev=NIL THEN list.head := mark.head
                ELSE mark.head.prev.next := mark.head END;
  INC(mark.remallowed);
END AddMarkBefore;


PROCEDURE AddMarkBehind*(VAR list: List; mark: Mark; x: NodePtr);
(* fügt mark hinter x in die Liste ein *)

BEGIN
  mark.tail.next := x.next;
  mark.head.prev := x;
  x.next := mark.head;
  IF mark.tail.next=NIL THEN list.tail := mark.tail
                     ELSE mark.tail.next.prev := mark.tail END;
  INC(mark.remallowed);
END AddMarkBehind;


PROCEDURE AddMarkHead*(VAR list: List; mark: Mark);
BEGIN
  mark.tail.next := list.head;
  mark.head.prev := NIL;
  IF mark.tail.next=NIL THEN list.tail   := mark.tail;
                ELSE mark.tail.next.prev := mark.tail END;
  list.head := mark.head;
  INC(mark.remallowed);
END AddMarkHead;


PROCEDURE AddMarkTail*(VAR list: List; mark: Mark);
BEGIN
  mark.head.prev := list.tail;
  mark.tail.next := NIL;
  IF mark.head.prev=NIL THEN list.head   := mark.head;
                ELSE mark.head.prev.next := mark.head END;
  list.tail := mark.tail;
  INC(mark.remallowed);
END AddMarkTail;


PROCEDURE RemoveMark*(VAR list: List; mark: Mark);
BEGIN
  IF (mark.head#NIL) & (mark.tail#NIL)THEN
    IF list.remallowed # 0 THEN HALT(20) END;
    IF mark.tail.next#NIL THEN
      mark.tail.next.prev := mark.head.prev
    ELSE
      list.tail := mark.head.prev
    END;
    IF mark.head.prev#NIL THEN
      mark.head.prev.next := mark.tail.next
    ELSE
      list.head := mark.tail.next
    END;
  END;
  DEC(mark.remallowed);
END RemoveMark;


PROCEDURE SetMark*(VAR mark: Mark; h,t: NodePtr);
BEGIN
  IF (h=NIL) & (t=NIL) THEN
    mark.head := NIL; mark.tail := NIL;
  ELSE
    IF h#NIL THEN mark.head := h; END;
    IF t#NIL THEN mark.tail := t; END;
  END;
  IF mark.remallowed=0 THEN mark.remallowed := 1; END;
END SetMark;

END EcoLists.
