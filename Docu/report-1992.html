<HTML>
<HEAD>
<TITLE>
 The Programming Language Oberon
</TITLE>
<LINK REV="made" HREF="mailto:oberon@mathematik.uni-ulm.de">
</HEAD>
<BODY>
<ADDRESS>Niklaus Wirth</ADDRESS>
<HR>
<H1> The Programming Language Oberon</H1>
<P>
<PRE>
		       <VAR>Make</VAR> <VAR>it</VAR> <VAR>as</VAR> <VAR>simple</VAR> <VAR>as</VAR> <VAR>possible</VAR>, <VAR>but</VAR> <VAR>not</VAR> <VAR>simpler</VAR>.
							   A. Einstein
</PRE>
<H2><A NAME="1.">1.</A> Introduction</H2>
<P>
Oberon is a general-purpose programming language that
evolved from Modula-2.
Its principal new feature is the concept of
<VAR>type extension</VAR>.
It permits the construction of new data types on the basis of
existing ones and provides relations between them.
<P>
This report is not intended as a programmer's tutorial.
It is intentionally kept concise.
Its function is to serve as a reference for
programmers, implementors and manual writers.
What remains unsaid is mostly left so intentionally,
either because it would require one to commit the
definition when a general commitment appears as unwise.
<H2><A NAME="2.">2.</A> Syntax</H2>
<P>
A language is an infinite set of sentences,
namely the sentences well formed according to its syntax.
In Oberon, these sentences are called compilation units.
Each unit is a finite sequence of symbols from a finite
vocabulary.
The vocabulary of Oberon consists of identifiers,
numbers, strings, operations, delimiters and comments.
They are called lexical symbols and are
composed of sequences of characters.
(Note the distinction between symbols and characters).
<P>
To describe the syntax, an extended Backus-Naur
Formalism called EBNF is used.
Brackets [ and ] denote optionality of the
enclosed sentential form,
and braces { and } denote its repetition (possibly 0 times).
Syntactic entities (non-terminal symbols)
are denoted by English words expressing their
intuitive meaning.
Symbols of the language vocabulary (terminal symbols)
are denoted by strings enclosed in quote marks or
words written in capital letters,
so-called reserved words.
Syntactic rules (productions) are marked by a bar
at the left margin of the line.
<H2><A NAME="3.">3.</A> Vocabulary and representation</H2>
<P>
The representation of symbols in terms of characters
is defined using the ASCII set.
Symbols are identifiers, numbers, strings, operations,
delimiters, and comments.
The following lexical rules must be observed.
Blanks and line breaks must not occur within symbols
(except in comments, and in the case of blanks,
in strings).
They are ignored unless they are essential to separate
two consecutive symbols.
Capital and lower-case letters are considered as
being distinct.
<P>
1. <VAR>Identifiers</VAR> are sequences of letters and digits.
The first character must be a letter.
<PRE>
$ ident = letter { letter | digit }.
</PRE>
Examples include
<PRE>
x scan Oberon GetSymbol firstLetter
</PRE>
<P>
2. <VAR>Numbers</VAR>
are (unsigned) integers or real numbers.
Integers are sequences of digits and may be followed
by a suffix letter.
The type is the minimal type to which the number
belongs (<A HREF="#6.1.">see Section 6.1</A>).
If no suffix is specified,
the representation is decimal.
The suffix <VAR>H</VAR> indicates hexadecimal representation.
<P>
A real number always contains a decimal point.
Optionally, it may also contain a decimal scale factor.
The letter <VAR>E</VAR> (or <VAR>D</VAR>)
is pronounced as 'times ten to the power of.'
A real number is of type REAL,
unless it has a scale factor containing the letter <VAR>D</VAR>,
in which case it is of type LONGREAL.
<PRE>
$ number = integer | real.
$ integer = digit {digit} | digit {hexDigit} ``H''.
$ real = digit {digit} ``.'' {digit} [ScaleFactor].
$ ScaleFactor = (``E'' | ``D'') [``+'' | ``-''] digit {digit}.
$ hexDigit = digit | ``A'' | ``B'' | ``C'' | ``D'' | ``E'' | ``F''.
$ digit = ``0'' | ``1'' | ``2'' | ``3'' | ``4'' | ``5'' | ``6'' | ``7'' | ``8'' | ``9''.
</PRE>
<P>
Examples include
<PRE>
1987
100H		= 256
12.3
4.567E8		= 456700000
0.57712566D-6	= 0.00000057712566
</PRE>
<P>
3. <VAR>Character constants</VAR> are either denoted
by a single character enclosed in quote marks
or by the ordinal number of the characters in
hexadecimal notation followed by the letter <VAR>X</VAR>.
<PRE>
$ CharConstant = ``&quot;'' character ``&quot;'' | digit {hexDigit} ``X''.
</PRE>
<P>
4. <VAR>Strings</VAR>
are sequences of characters enclosed in quote marks (&quot;).
A string cannot contain a quote mark.
The number of characters in a string is called
the length of the string.
Strings can be assigned to and compared with
arrays of characters
(see Sections 9.1 and 8.2.4).
<PRE>
$ string = ``&quot;''{character}``&quot;''.
</PRE>
<P>
Examples include
<PRE>
&quot;OBERON&quot;   &quot;Don't worry!&quot;
</PRE>
<P>
5. <VAR>Operators</VAR> and <VAR>delimiters</VAR>
are the special characters,
character pairs, or reserved words listed below.
These reserved words consist exclusively of capital
letters and cannot be used in the role of identifiers.
<PRE>
+	    :=		ARRAY	    IS		TO
-	    ^		BEGIN	    LOOP	TYPE
*	    =		CASE	    MOD		UNTIL
/	    #		CONST	    MODULE	VAR
~	    &lt;		DIV	    NIL		WHILE
&amp;	    &gt;		DO	    OF		WITH
.	    &lt;=		ELSE	    OR
,	    &gt;=		ELSIF	    POINTER
;	    ..		END	    PROCEDURE
|	    :		EXIT	    RECORD
(	    )		IF	    REPEAT
[	    ]		IMPORT	    RETURN
{	    }		IN	    THEN
</PRE>
<P>
6. <VAR>Comments</VAR>
may be inserted between any two symbols in a program.
They are arbitrary character sequences opened by
the bracket (* and closed by *).
Comments do not affect the meaning of a program.
<H2><A NAME="4.">4.</A> Declarations and scope rules</H2>
<P>
Every identifier occurring in a program must be introduced
by a declaration, unless it is a predefined identifier.
Declarations also serve to specify certain permanent
properties of an object,
such as whether it is a constant, a type,
a variable or a procedure.
<P>
The identifier is the used to refer to the associated object.
This is possible only in those parts of a program
that are within the scope of the declaration.
No identifier may denote more than one object within
a given scope.
The scope extends textually from the point of
the declaration to the end of the block (procedure or module)
to which the declaration belongs and hence to which the object
is local.
The scope rule has the following amendments:
<DL>
<DT>(1)<DD>
If a type <VAR>T</VAR> is defined as POINTER TO <VAR>T1</VAR>
(<A HREF="#6.4.">see Section 6.4</A>),
then the identifier <VAR>T1</VAR> can be declared textually following
the declaration of <VAR>T</VAR>,
but it must lie within the same scope.
<DT>(2)<DD>
Field identifiers of a record declaration (see 6.3)
are valid in field designators only.
</DL>
<P>
In its declaration, an identifier in the global scope
may be followed by an export mark (*) to indicate that
it is exported from its declaring module.
In this case,
the identifier may be used in other modules,
if they import the declaring module.
The identifier is then prefixed by
the identifier designating its module (<A HREF="#11.">see Section 11</A>).
The prefix and the identifier are separated by a period
and together are called a <VAR>qualified identifier</VAR>.
<PRE>
$ qualident = [ident ``.''] ident.
$ identdef = ident [``*''].
</PRE>
<P>
The following identifiers are predefined;
their meaning is defined in the indicated sections:
<PRE>
	      ABS	(<A HREF="#10.2.">10.2</A>)	 LEN	    (<A HREF="#10.2.">10.2</A>)
	      ASH	(<A HREF="#10.2.">10.2</A>)	 LONG	    (<A HREF="#10.2.">10.2</A>)
	      BOOLEAN	 (<A HREF="#6.1.">6.1</A>)	 LONGINT     (<A HREF="#6.1.">6.1</A>)
	      CAP	(<A HREF="#10.2.">10.2</A>)	 LONGREAL    (<A HREF="#6.1.">6.1</A>)
	      CHAR	 (<A HREF="#6.1.">6.1</A>)	 MAX	    (<A HREF="#10.2.">10.2</A>)
	      CHR	(<A HREF="#10.2.">10.2</A>)	 MIN	    (<A HREF="#10.2.">10.2</A>)
	      COPY	(<A HREF="#10.2.">10.2</A>)	 NEW	     (<A HREF="#6.4.">6.4</A>)
	      DEC	(<A HREF="#10.2.">10.2</A>)	 ODD	    (<A HREF="#10.2.">10.2</A>)
	      ENTIER	(<A HREF="#10.2.">10.2</A>)	 ORD	    (<A HREF="#10.2.">10.2</A>)
	      EXCL	(<A HREF="#10.2.">10.2</A>)	 REAL	     (<A HREF="#6.1.">6.1</A>)
	      FALSE	 (<A HREF="#6.1.">6.1</A>)	 SET	     (<A HREF="#6.1.">6.1</A>)
	      HALT	(<A HREF="#10.2.">10.2</A>)	 SHORT	    (<A HREF="#10.2.">10.2</A>)
	      INC	(<A HREF="#10.2.">10.2</A>)	 SHORTINT    (<A HREF="#6.1.">6.1</A>)
	      INCL	(<A HREF="#10.2.">10.2</A>)	 SIZE	    (<A HREF="#10.2.">10.2</A>)
	      INTEGER	 (<A HREF="#6.1.">6.1</A>)	 TRUE	     (<A HREF="#6.1.">6.1</A>)
</PRE>
<H2><A NAME="5.">5.</A> Constant declarations</H2>
<P>
A constant declaration associates an identifier with a
constant value.
<PRE>
$ ConstantDeclaration = identdef ``='' ConstExpression.
$ ConstExpression = expression.
</PRE>
<P>
A constant expression can be evaluated by a mere
textual scan without actually executing the program.
Its operands are constants
(<A HREF="#8.">see Section 8</A>).
Examples of constant declarations include
<PRE>
N     = 100
limit = 2*N - 1
all   = {0..WordSize-1}
</PRE>
<H2><A NAME="6.">6.</A> Type declarations</H2>
<P>
A data type determines the set of values that variables
of that type may assume, and the operators that are applicable.
A type declaration is used to associate
an identifier with the type.
Such association may be with unstructured (basic) types,
or it may be with structured types, in which case it
defines the structure of variables of this type
and, by implication, the operator that are applicable
to the components.
There are two different structures,
namely arrays and records,
with different component selectors.
<PRE>
$ TypeDeclaration = identdef ``='' type.
$ type = qualident | ArrayType | RecordType | PointerType |
$        ProcedureType.
</PRE>
<P>
Examples include
<PRE>
Table      = ARRAY N OF REAL
Tree       = POINTER TO Node
Node       = RECORD key: INTEGER;
               left, right: Tree
             END
CenterNode = RECORD (Node)
               name: ARRAY 32 OF CHAR;
               subnode: Tree
             END
Function*  = PROCEDURE (x: INTEGER): INTEGER
</PRE>
<H3><A NAME="6.1.">6.1.</A> Basic types</H3>
<P>
The following basic types are denoted by predeclared
identifiers.
The associated operators are defined in <A HREF="#8.2.">Section 8.2</A>,
and the predeclared function procedures in <A HREF="#10.2.">Section 10.2</A>.
The values of a given basic type are as follows:
<PRE>
1. BOOLEAN     the truth values	TRUE and FALSE.
2. CHAR	       the characters of the extended ASCII set	(0X...0FFX).
3. SHORTINT    the integers between MIN(SHORTINT) and MAX(SHORTINT).
4. INTEGER     the integers between MIN(INTEGER) and MAX(INTEGER).
5. LONGINT     the integers between MIN(LONGINT) and MAX(LONGINT).
6. REAL	       real numbers between MIN(REAL) and MAX(REAL).
7. LONGREAL    real numbers between MIN(LONGREAL) and MAX(LONGREAL).
8. SET	       the sets	of integers between 0 and MAX(SET).
</PRE>
<P>
Types 3 to 5 are <VAR>integer</VAR> types,
6 and 7 are <VAR>real</VAR> types,
and together they are called <VAR>numeric</VAR> types.
They form a hierarchy;
the larger type <VAR>includes</VAR>
(the values of) the smaller type:
<PRE>
LONGREAL )= REAL )= LONGINT )= INTEGER )= SHORTINT
</PRE>
<H3><A NAME="6.2.">6.2.</A> Array types</H3>
<P>
An array is a structure consisting of a fixed number
of elements that are all of the same type,
called the <VAR>element type</VAR>.
The number of elements of an array is called its <VAR>length</VAR>.
The elements of the array are designated by indices,
which are integers between 0 and the length minus 1.
<PRE>
$ ArrayType = ARRAY length {``,'' length} OF type.
$ length = ConstExpression.
</PRE>
<P>
A declaration of the form
<PRE>
ARRAY N0, N1, ... , Nk OF T
</PRE>
is understood as an abbreviation of the declaration
<PRE>
ARRAY N0 OF
  ARRAY N1 OF
    ...
      ARRAY Nk OF T
</PRE>
<P>
Examples of array types include
<PRE>
ARRAY N OF INTEGER
ARRAY 10, 20 OF REAL
</PRE>
<H3><A NAME="6.3.">6.3.</A> Record types</H3>
<P>
A record type is a structure consisting of a fixed
number of elements of possibly different types.
The record type declaration specifies
for each element, called a <VAR>field</VAR>,
its type and an identifier that denotes the field.
The scope of these field identifiers is the
record definition itself,
but they are also visible within field designators
(<A HREF="#8.1.">see Section 8.1</A>) referring to elements of record variables.
<PRE>
$ RecordType = RECORD [``('' BaseType ``)''] FieldListSequence
$              END.
$ BaseType = qualident.
$ FieldListSequence = FieldList {``;'' FieldList}.
$ FieldList = [IdentList ``:'' type].
$ IdentList = identdef {``,'' identdef}.
</PRE>
<P>
If a record type is exported, field identifiers that are
to be visible outside the declaring module must
be marked.
They are called <VAR>public fields</VAR>;
unmarked fields are called <VAR>private fields</VAR>.
<P>
Record types are extensible; that is,
a record type can be defined as an extension of another
record type.
In the examples above,
<VAR>CenterNode</VAR> <VAR>(directly) extends</VAR> <VAR>Node</VAR>,
which is the <VAR>(direct) base type</VAR> of <VAR>CenterNode</VAR>.
More specifically, <VAR>CenterNode</VAR> extends <VAR>Node</VAR>
with the fields <VAR>name</VAR> and <VAR>subnode</VAR>.
<P>
<VAR>Definition</VAR>:
A type <VAR>T0</VAR> <VAR>extends</VAR> a type <VAR>T</VAR>
if it equals <VAR>T</VAR> or if it directly extends
an extension of <VAR>T</VAR>.
Conversely, a type <VAR>T</VAR> is a <VAR>base type</VAR>
of <VAR>T0</VAR> if it equals <VAR>T0</VAR> or if it is
the direct base type of a base type of <VAR>T0</VAR>.
<P>
Examples of record type include
<PRE>
RECORD day, month, year: INTEGER
END

RECORD
  name, firstname: ARRAY 32 OF CHAR;
  age: INTEGER;
  salary: REAL
END
</PRE>
<H3><A NAME="6.4.">6.4.</A> Pointer types</H3>
<P>
Variables of a pointer type <VAR>P</VAR> assume as values
pointers to variables of some type <VAR>T</VAR>.
The pointer type <VAR>P</VAR> is said to be <VAR>bound</VAR>
to <VAR>T</VAR>, and <VAR>T</VAR> is the <VAR>pointer base type</VAR>
of <VAR>P</VAR>.
<VAR>T</VAR> must be a record or array type.
Pointer types inherit the extension relation of their
base types.
If a type <VAR>T0</VAR> is an extension of <VAR>T</VAR>
and <VAR>P0</VAR> is a pointer type bound to <VAR>T0</VAR> then
<VAR>P0</VAR> is also an extension of <VAR>P</VAR>.
<PRE>
$ PointerType = POINTER TO type.
</PRE>
<P>
If <VAR>p</VAR> is a variable of type <VAR>P</VAR> = POINTER TO <VAR>T</VAR>
then a call of the predefined procedure NEW(<VAR>p</VAR>)
has the following effect (<A HREF="#10.2.">see Section 10.2</A>):
A variable of type <VAR>T</VAR> is allocated in free storage,
and a pointer to it is assigned to <VAR>p</VAR>.
This pointer <VAR>p</VAR> is of type <VAR>P</VAR>;
the <VAR>referenced</VAR> variable <VAR>p</VAR>^
is of type <VAR>T</VAR>.
Failure of allocation results in <VAR>p</VAR>
obtaining the value NIL.
Any pointer variable may be assigned the value <STRONG>NIL</STRONG>,
which points to no variable at all.
<H3><A NAME="6.5.">6.5.</A> Procedure types</H3>
<P>
Variables of a procedure type <VAR>T</VAR> have a procedure
(or NIL) as value.
If a procedure <VAR>P</VAR> is assigned to a procedure
variable of type <VAR>T</VAR>,
the (types of the) formal parameters of <VAR>P</VAR>
must be the same as those indicated in the
formal parameters of <VAR>T</VAR>.
The same holds for the result type in the case
of a function procedure (<A HREF="#10.1.">see Section 10.1</A>).
<VAR>P</VAR> must not be declared local to another procedure,
and neither can it be a predefined procedure.
<PRE>
$ ProcedureType = PROCEDURE [FormalParameters].
</PRE>
<H2><A NAME="7.">7.</A> Variable declarations</H2>
<P>
Variable declarations serve to introduce variables
and associate them with identifiers that must be unique
within the given scope.
They also serve to associate fixed data types
with the variables.
<PRE>
$ VariableDeclaration = IdentList ``:'' type.
</PRE>
<P>
Variables whose identifiers appear in the same list
are all of the same type.
Examples of variable declarations
(refer to the examples in <A HREF="#6.">Section 6</A>) include
<PRE>
i, j, k: INTEGER
x, y:    REAL
p, q:    BOOLEAN
s:       SET
f:       Function
a:       ARRAY 100 OF REAL
w:       ARRAY 16 OF
           RECORD ch: CHAR;
             count: INTEGER
           END
t:       Tree
</PRE>
<P>
Variables of a pointer type <VAR>T0</VAR>
and VAR-parameters of a record type <VAR>T0</VAR>
may assume values whose type <VAR>T1</VAR> is an extension
of their declared type <VAR>T0</VAR>.
<H2><A NAME="8.">8.</A> Expressions</H2>
<P>
Expressions are constructs denoting rules of computation
whereby constants and current values of variables are
combined to derive other values by the application of
operators and function procedures.
Expressions consist of operands and operators.
Parentheses may be used to express specific associations
of operators and operands.
<H3><A NAME="8.1.">8.1.</A> Operands</H3>
<P>
With the exception of sets and literal constants,
that is numbers and character strings,
operands are denoted by <VAR>designators</VAR>.
A designator consists of an identifier referring
to the constant, variable, or procedure to
be designated.
This identifier may possibly be qualified by
module identifiers (see Sections 4 and 11),
and it may be followed by selectors,
if the designated object is an element of a structure.
<P>
If <VAR>A</VAR> designates an array,
then <VAR>A</VAR>[<VAR>E</VAR>] denotes that element of <VAR>A</VAR>
whose index is the current value of the expression <VAR>E</VAR>.
Note that <VAR>E</VAR> must be of integer type.
A designator of the form
<VAR>A</VAR>[<VAR>E1</VAR>, <VAR>E2</VAR>, ... , <VAR>En</VAR>] stands
for <VAR>A</VAR>[<VAR>E1</VAR>][<VAR>E2</VAR>] ... [<VAR>En</VAR>].
If <VAR>p</VAR> designates a pointer variable,
<VAR>p</VAR>^ denotes the variable that is referenced by <VAR>p</VAR>.
If <VAR>r</VAR> designates a record
then <VAR>r.f</VAR> denotes the field <VAR>f</VAR> of <VAR>r</VAR>.
If <VAR>p</VAR> designates a pointer,
<VAR>p.f</VAR> denotes the field of the record <VAR>p</VAR>^
(that is, the dot implies dereferencing and <VAR>p.f</VAR>
stands for <VAR>p</VAR>^.<VAR>f</VAR>)
and <VAR>p</VAR>[<VAR>E</VAR>] denotes the element of
<VAR>p</VAR>^ with index <VAR>E</VAR>.
<P>
The <VAR>typeguard</VAR> <VAR>v</VAR>(<VAR>T0</VAR>) asserts that
<VAR>v</VAR> is of type <VAR>T0</VAR>;
that is, it aborts program execution if it is not of type <VAR>T0</VAR>.
The guard is applicable if
<DL>
<DT>(1)<DD>
<VAR>T0</VAR> is an extension of the declared type <VAR>T</VAR> of <VAR>v</VAR>,
and
<DT>(2)<DD>
<VAR>v</VAR> is a formal variable parameter of record type or
<VAR>v</VAR> is a pointer.
</DL>
<P>
<PRE>
$ designator = qualident {``.'' ident | ``['' ExpList ``]'' |
$              ``('' qualident ``)'' | ``^''}.
$ ExpList = expression {``,'' expression}.
</PRE>
<P>
If the designated object is a variable then
the designator refers to the variable's current value.
If the object is a procedure,
a designator without parameter list refers to that procedure.
If it is followed by (possibly empty)
parameter list,
the designator implies an activation of the procedure and
stands for the value resulting from its execution.
The (type of the) actual parameters must
correspond to the formal parameters as specified in the
procedure's declaration (<A HREF="#10.">see Section 10</A>).
<P>
Examples of designators (see the examples in <A HREF="#8.">Section 8</A>)
include
<PRE>
i			(INTEGER)
a[i]			(REAL)
w[3].ch			(CHAR)
t.key			(INTEGER)
t.left.right		(Tree)
t(CenterNode).subnode	(Tree)
</PRE>
<H3><A NAME="8.2.">8.2.</A> Operators</H3>
<P>
The syntax of expressions distinguishes between
four classes of operators with different precedences
(binding strengths).
The operator ~ has the highest precedence,
followed by multiplication operators,
addition operators and relations.
Operators of the same precedence
associate from left to right.
For example, x - y - z stands for (x - y) - z.
<PRE>
$ expression = SimpleExpression [relation SimpleExpression].
$ relation = ``='' | ``#'' | ``&lt;'' | &quot;&lt;=&quot; | ``&gt;'' | &quot;&gt;=&quot; | IN | IS.
$ SimpleExpression = [``+'' | ``-''] term {AddOperator term}.
$ AddOperator = ``+'' | ``-'' | OR.
$ term = factor {MulOperator factor}.
$ MulOperator = ``*'' | ``/'' | DIV | MOD | ``&amp;''.
$ factor = number | CharConstant | string | NIL | set |
$ 	 designator [ActualParameters] | ``('' expression ``)'' |
$ 	 ``~'' factor.
$ set = ``{'' [element {``,'' element}] ``}''.
$ element = expression [&quot;..&quot; expression].
$ ActualParameters = ``('' [ExpList] ``)''.
</PRE>
<P>
The available operators are listed in Sections 8.2.1-8.2.4.
In some instances, several different operations are designated
by the same operator symbol.
In these cases, the actual operation is identified by
the type of the operands.
<H4><A NAME="8.2.1.">8.2.1.</A> Logical operators</H4>
<P>
<PRE>
____________________________
Symbol	 Result
____________________________
  OR	 logical disjunction
  &amp;	 logical conjunction
  ~	 negation
____________________________
</PRE>
<P>
These operators apply to BOOLEAN operands and yield
a BOOLEAN result.
<PRE>
p OR q	 stands	for   &quot;if p then TRUE, else q&quot;
p &amp; q	 stands	for   &quot;if p then q, else FALSE&quot;
~ p	 stands	for   &quot;not p&quot;
</PRE>
<H4><A NAME="8.2.2.">8.2.2.</A> Arithmetic operators</H4>
<P>
<PRE>
_________________________
Symbol	 Result
_________________________
  +	 sum
  -	 difference
  *	 product
  /	 quotient
 DIV	 integer quotient
 MOD	 modulus
_________________________
</PRE>
<P>
The operators +, -, *, and / apply to operands of
numeric types.
The type of the result is that operand's type which includes
the other operand's type, except for division (/),
where the result is the real type which includes
both operand types.
When used as operators with a single operand,
- denotes sign inversion and + denotes the identity
operation.
<P>
The operators DIV and MOD apply to integer operands only.
They are related by the following formulas defined for
any dividend <VAR>x</VAR> and positive divisors <VAR>y</VAR>:
<PRE>
x = (x DIV y) * y + (x MOD y)
0 &lt;= (x MOD y) &lt; y
</PRE>
<H4><A NAME="8.2.3.">8.2.3.</A> Set operators</H4>
<P>
<PRE>
_________________________________
Symbol	 Result
_________________________________
  +	 union
  -	 difference
  *	 intersection
  /	 symmetric set difference
_________________________________
</PRE>
<P>
Sets are values of type SET.
Set operators apply to operands of this type.
The monadic minus sign denotes the complement of <VAR>x</VAR>;
that is, -<VAR>x</VAR> denotes the set of integers between
0 and MAX(SET) that are not elements of <VAR>x</VAR>.
<PRE>
x - y = x * (- y)
x / y = (x - y) + (y - x)
</PRE>
<H4><A NAME="8.2.4.">8.2.4.</A> Relations</H4>
<P>
<PRE>
_________________________
Symbol	 Relation
_________________________
  =	 equal
  #	 unequal
  &lt;	 less
  &lt;=	 less or equal
  &gt;	 greater
  &gt;=	 greater or equal
  IN	 set membership
  IS	 type test
_________________________
</PRE>
<P>
Relations are Booleans.
The ordering relations &lt;, &lt;=, &gt; and &gt;=
apply to the numeric types, CHAR and character arrays (strings).
The relations = and # also apply to the type BOOLEAN
and to set, pointer and procedure types.
<VAR>x</VAR> IN <VAR>s</VAR> stands for '<VAR>x</VAR> is an element of <VAR>s</VAR>.'
<VAR>x</VAR> must be of an integer type, and <VAR>s</VAR> of type SET.
<VAR>v</VAR> IS <VAR>T</VAR> stands for '<VAR>v</VAR> is of type <VAR>T</VAR>'
and is called a type test.
It is applicable if
<DL>
<DT>(1)<DD>
<VAR>T</VAR> is an extension of the declared type <VAR>T0</VAR>
of <VAR>v</VAR>, and
<DT>(2)<DD>
<VAR>v</VAR> is a variable parameter of record type or
<VAR>v</VAR> is a pointer.
</DL>
<P>
Assuming, for instance, that <VAR>T</VAR> is an extension
of <VAR>T0</VAR> and that <VAR>v</VAR> is a designator declared of
type <VAR>T0</VAR>, then the test '<VAR>v</VAR> IS <VAR>T</VAR>'
determines whether the actually designated variable is
(not only a <VAR>T0</VAR>, but also) a <VAR>T</VAR>.
The value of NIL IS <VAR>T</VAR> is undefined.
<P>
Examples of expressions (refer to the examples in <A HREF="#7.">Section 7</A>)
include
<PRE>
1987			(INTEGER)
i DIV 3			(INTEGER)
~p OR q			(BOOLEAN)
(i + j)	* (i - j)	(INTEGER)
s - {8,	9, 13}		(SET)
i + x			(REAL)
a[i + j] * a[i - j]	(REAL)
(0 &lt;= i) &amp; (i &lt;= 100)	(BOOLEAN)
t.key =	0		(BOOLEAN)
k IN {i..j-1}		(BOOLEAN)
t IS CenterNode		(BOOLEAN)
</PRE>
<H2><A NAME="9.">9.</A> Statements</H2>
<P>
Statements denote actions.
There are elementary and structured statements.
Elementary statements are not composed of any parts
that are themselves statements.
They are the assignment, the procedure call,
and the return and exit statements.
Structured statements are composed of
parts that are themselves statements.
They are used to express
sequencing and conditional, selective and repetitive execution.
A statement may also be empty,
in which case it denotes no action.
The empty statement is included in order
to relax punctuation rules in
statement sequences.
<PRE>
$ statement = [assignment | ProcedureCall | IfStatement |
$             CaseStatement | WhileStatement | RepeatStatement |
$             LoopStatement | WithStatement | EXIT |
$             RETURN [expression]].
</PRE>
<H3><A NAME="9.1.">9.1.</A> Assignments</H3>
<P>
The assignment serves to replace the current value of a variable by
a new value specified by an expression.
The assignment operator is written as &quot;:=&quot; and pronounced
as <VAR>becomes</VAR>.
<PRE>
$ assignment = designator &quot;:=&quot; expression.
</PRE>
<P>
The type of the expression must be included by the
type of the variable, or it must extend the type of the
variable.
The following exceptions hold:
<DL>
<DT>(1)<DD>
The constant NIL can be assigned to variables of
any pointer or procedure type.
<DT>(2)<DD>
Strings can be assigned to any variable whose
type is an array of characters,
provided the length of the string is
less than that of the array.
If a string <VAR>s</VAR> of length <VAR>n</VAR>
is assigned to an array <VAR>a</VAR>,
the result is <VAR>a</VAR>[<VAR>i</VAR>] = <VAR>si</VAR>
for <VAR>i</VAR> = 0, ..., <VAR>n</VAR>-1, and <VAR>a</VAR>[<VAR>n</VAR>] = 0X.
</DL>
<P>
Examples of assignments (see the examples in <A HREF="#7.">Section 7</A>)
include
<PRE>
i := 0
p := i = j
x := i + 1
k := log2(i + j)
f := log2
s := {2, 3, 5, 7, 11, 13}
a[i] := (x + y) * (x - y)
t.key := i
w[i + 1].ch := ``A''
</PRE>
<H3><A NAME="9.2.">9.2.</A> Procedure calls</H3>
<P>
A procedure call serves to activate a procedure.
The procedure call may contain a list of actual parameters
that are substituted in place of their corresponding
formal parameters defined in the procedure declaration
(<A HREF="#10.">see Section 10</A>).
The correspondence is established by the positions
of the parameters in the lists of actual and formal
parameters respectively.
There exist two kinds of parameters:
<VAR>variable</VAR> and <VAR>value parameters</VAR>.
<P>
In the case of variable parameters,
the actual parameter must be a designator denoting
a variable.
If it designates an element of a structured variable,
the selector is evaluated when the formal/actual parameter
substitution takes place;
that is, before the execution of the procedure.
If the parameter is a value parameter,
the corresponding actual parameter must be an expression.
This expression is evaluated prior to the
procedure activation,
and the resulting value is assigned to the formal
parameter, which now constitutes a local variable
(see also <A HREF="#10.1.">Section 10.1</A>).
<PRE>
$ ProcedureCall = designator [ActualParameters].
</PRE>
<P>
Examples of procedure calls include
<PRE>
ReadInt(i)              (<A HREF="#10.">see Section 10</A>)
WriteInt(j*2 + 1, 6)
INC(w[k].count)
</PRE>
<H3><A NAME="9.3.">9.3.</A> Statement sequences</H3>
<P>
Statement sequences denote the sequence of actions
specified by the component statements,
which are separated by semicolons.
<PRE>
$ StatementSequence = statement {``;'' statement}.
</PRE>
<H3><A NAME="9.4.">9.4.</A> If statements</H3>
<P>
<PRE>
$ IfStatement = IF expression THEN StatementSequence
$               {ELSIF expression THEN StatementSequence}
$               [ELSE StatementSequence]
$               END.
</PRE>
<P>
If statements specify the conditional execution of
guarded statements.
The Boolean expression preceding a statement is called its
<VAR>guard</VAR>.
The guards are evaluated in sequence of occurrence,
until one evaluates to TRUE,
whence its associated statement sequence is executed.
If no guard is satisfied,
the statement sequence following the symbol ELSE
is executed, if there is one.
<P>
An example is
<PRE>
IF (ch &gt;= ``A'') &amp; (ch &lt;= ``Z'') THEN ReadIdentifier
ELSIF (ch &gt;= ``0'') &amp; (ch &lt;= ``9'') THEN ReadNumber
ELSIF ch = 22X THEN ReadString
END
</PRE>
<H3><A NAME="9.5.">9.5.</A> Case statements</H3>
<P>
Case statements specify the selection and execution of
a statement sequence according to the value of an expression.
First the case expression is evaluated;
then the statement sequence is executed
whose case label list contains the obtained value.
The case expression and all labels must be of the same
type, which must be an integer type or CHAR.
Case labels are constants, and no value must occur
more than once.
If the value of the expression does not occur as a label
of any case, the statement sequence following
the symbol ELSE is selected,
if there is one.
Otherwise it is considered as an error.
<PRE>
$ CaseStatement = CASE expression OF case {``|'' case}
$                 [ELSE StatementSequence] END.
$ case = [CaseLabelList ``:'' StatementSequence].
$ CaseLabelList = CaseLabels {``,'' CaseLabels}.
$ CaseLabels = ConstExpression [&quot;..&quot; ConstExpression].
</PRE>
<P>
An example is
<PRE>
CASE ch OF
    ``A''..``Z'': ReadIdentifier
  | ``0''..``9'': ReadNumber
  | 22X: ReadString
ELSE SpecialCharacter
END
</PRE>
<H3><A NAME="9.6.">9.6.</A> While statements</H3>
<P>
While statements specify repetition.
If the Boolean expression (guard) yields TRUE,
the statement sequence is executed.
The expression evaluation and the statement
execution are repeated as long as the Boolean
expression yields TRUE.
<PRE>
$ WhileStatement = WHILE expression DO StatementSequence END.
</PRE>
<P>
Examples include
<PRE>
WHILE j &gt; 0 DO
  j := j DIV 2; i := i + 1
END

WHILE (t # NIL) &amp; (t.key # i) DO
  t := t.left
END
</PRE>
<H3><A NAME="9.7.">9.7.</A> Repeat statements</H3>
<P>
A repeat statement specifies the repeated execution
of a statement sequence until a condition is satisfied.
The statement sequence is executed at least once.
<PRE>
$ RepeatStatement = REPEAT StatementSequence UNTIL expression.
</PRE>
<H3><A NAME="9.8.">9.8.</A> Loop statements</H3>
<P>
A loop statement specifies the repeated execution of statement
sequence.
It is terminated by the execution of any exit statement
within that sequence (<A HREF="#9.9.">see Section 9.9</A>).
<PRE>
$ LoopStatement = LOOP StatementSequence END.
</PRE>
<P>
An example is
<PRE>
LOOP
  IF t1 = NIL THEN EXIT END;
  IF k &lt; t1.key THEN t2 := t1.left; p := TRUE
  ELSIF k &gt; t1.key THEN t2 := t1.right; p := FALSE
  ELSE EXIT
  END;
  t1 := t2
END
</PRE>
<P>
Although while and repeat statements can be expressed
by loop statements containing a single exit statement,
the use of while and repeat statements is recommended
in the most frequently occurring situations,
where termination depends on a single condition
determined either at the beginning or the end of the
repeated statement sequence.
The loop statement is useful to express cases with
several termination conditions and points.
<H3><A NAME="9.9.">9.9.</A> Return and exit statements</H3>
<P>
A return statement consists of the symbol RETURN,
possibly followed by an expression.
It indicates the termination of a procedure,
and the expression specifies the result of a
function procedure.
Its type must be identical to the result type specified
in the procedure heading (<A HREF="#10.">see Section 10</A>).
<P>
Function procedure require the presence of
a return statement indicating the result value.
There may be several,
although only one will be executed.
In proper procedures,
a return statement is implied by
the end of the procedure body.
An explicit return statement therefore appears
as an additional (probably exceptional)
termination point.
<P>
An exit statement consists of the symbol EXIT.
It specifies termination of the enclosing loop statement
and continuation with the statement
following that loop statement.
Exit statements are contextually,
although not syntactically,
bound to the loop statement that contains them.
<H3><A NAME="9.10.">9.10.</A> With statements</H3>
<P>
If a pointer variable or a variable parameter with record
structure is of a type <VAR>T0</VAR>,
it may be designated in the heading of a with clause
together with a type <VAR>T</VAR> that is an extension
of <VAR>T0</VAR>.
Then the variable is guarded within the with statement
as if it had been declared of type <VAR>T</VAR>.
The with statement assumes a role similar to the
type guard,
extending the guard over an entire statement sequence.
It may be regarded as a <VAR>regional type guard</VAR>.
<PRE>
$ WithStatement = WITH qualident ``:'' qualident DO
$                 StatementSequence END.
</PRE>
<P>
An example is
<PRE>
WITH t: CenterNode DO name := t.name; L := t.subnode END;
</PRE>
<H2><A NAME="10.">10.</A> Procedure declarations</H2>
<P>
Procedure declarations consist of a <VAR>procedure heading</VAR>
and a <VAR>procedure body</VAR>.
The heading specifies the procedure identifier,
the <VAR>formal parameters</VAR> and the result type (if any).
The body contains declarations and statements.
The procedure identifier is repeated at the end
of the procedure declaration.
<P>
There are two kinds of procedures,
namely <VAR>proper procedures</VAR> and
<VAR>function procedures</VAR>.
The latter are activated by a function designator
as a constituent of an expression,
and yield a result that is an operand in the expression.
Proper procedures are activated by a procedure call.
The function procedure is distinguished in the declaration
by indication of the type of its result following the parameter list.
Its body must contain a RETURN statement that
defines the result of the function procedure.
<P>
All constants, variables, types and procedures
declared within a procedure body are local to the procedure.
The values of local variables are undefined upon entry
to the procedure.
Since procedures may be declared as local objects too,
procedure declarations may be nested.
<P>
In addition to its formal parameters
and locally declared objects,
the objects declared in the environment
of the procedure are also visible in the procedure
(with the exception of those objects that have
the same names as an object declared locally).
<P>
The use of the procedure identifier in a call
within its declaration
implies recursive activation of the procedure.
<PRE>
$ ProcedureDeclaration =
$            ProcedureHeading ``;'' ProcedureBody ident.
$ ProcedureHeading =
$            PROCEDURE [``*''] identdef [FormalParameters].
$ ProcedureBody =
$            DeclarationSequence [BEGIN StatementSequence] END.
$ ForwardDeclaration =
$            PROCEDURE ``^'' identdef [FormalParameters].
$ DeclarationSequence =
$            {CONST {ConstantDeclaration ``;''} |
$            TYPE {TypeDeclaration ``;''} |
$            VAR {VariableDeclaration ``;''}}
$            {ProcedureDeclaration ``;'' | ForwardDeclaration ``;''}.
</PRE>
<P>
A <VAR>forward declaration</VAR> serves to allow forward
references to a procedure that appears later in the text
in full.
The actual declaration -- which specifies the body --
must indicate the same parameters and result type (if any)
as the forward declaration, and it must be within the
same scope.
<P>
An asterisk following the symbol PROCEDURE is a hint
to the compiler, and specifies that the procedure is to
be usable as a parameter and assignable to variables.
(Depending on the implementation, the hint may be
optional or required.)
<H3><A NAME="10.1.">10.1.</A> Formal parameters</H3>
<P>
Formal parameters are identifiers that denote actual
parameters specified in the procedure call.
The correspondence between formal and actual
parameters is established when the procedure is called.
There are two kinds of parameters,
namely <VAR>value</VAR> and <VAR>variable parameters</VAR>.
The kind is indicated in the formal parameter list.
Value parameters stand for local
variables to which the result of the evaluation
of the corresponding actual parameter is assigned as initial value.
Variable parameters correspond to actual parameters
that are variables,
and they stand for these variables.
Variable parameters are indicated by
the symbol VAR, value parameters by its absence.
A function procedure without parameters must
have an empty parameter list.
It must be called by a function designator whose actual
parameter list is empty too.
<P>
Formal parameters are local to the procedure;
that is, their scope is the program text
that constitutes the procedure declaration.
<PRE>
$ FormalParameters = ``('' [FPSection {``;'' FPSection}] ``)''
$                    [``:'' qualident].
$ FPSection = [VAR] ident {``,'' ident} ``:'' FormalType.
$ FormalType = {ARRAY OF} qualident | ProcedureType.
</PRE>
<P>
The type of each formal parameter is specified
in the parameter list.
For variable parameters,
it must be identical to the corresponding actual
parameter's type, except in the case of a record,
where it must be a base type of the corresponding
actual parameter's type.
For value parameters, the rule of assignment holds
(<A HREF="#9.1.">see Section 9.1</A>).
If the formal parameter's type is specified as
<PRE>
ARRAY OF T
</PRE>
the parameter is said to be an <VAR>open array parameter</VAR>,
and the corresponding actual parameter may be
any array with element type <VAR>T</VAR>.
<P>
If a formal parameter specifies a procedure type
then the corresponding actual parameter must be
either a procedure declared globally or a variable
(or parameter) of that procedure type.
It cannot be a predefined procedure.
The result type of a procedure can be neither
a record nor an array.
<P>
Examples of procedure declarations include
<PRE>
PROCEDURE ReadInt(VAR x: INTEGER);
  VAR i: INTEGER; ch: CHAR;
BEGIN i := 0; Read(ch);
  WHILE (``0'' &lt;= ch) &amp; (ch &lt;= ``9'') DO
    i := 10*i + (ORD(ch) - ORD(``0'')); Read(ch)
  END;
  x := i
END ReadInt

PROCEDURE WriteInt(x: INTEGER); (* 0 &lt;= x &lt; 10.0E5 *)
  VAR
    i: INTEGER;
    buf: ARRAY 5 OF INTEGER;
BEGIN i := 0;
  REPEAT buf[i] := x MOD 10; x := x DIV 10; INC(i) UNTIL x = 0;
  REPEAT DEC(i); Write(CHR(buf[i] + ORD(``0''))) UNTIL i = 0
END WriteInt


PROCEDURE log2(x: INTEGER): INTEGER;
  VAR y: INTEGER; (* assume x &gt; 0 *)
BEGIN y := 0;
  WHILE x &gt; 1 DO x := x DIV 2; INC(y) END;
  RETURN y
END log2
</PRE>
<H3><A NAME="10.2.">10.2.</A> Predefined procedures</H3>
<P>
The following tables list the predefined procedures.
Some are <VAR>generic procedures</VAR>;
that is, they apply to several types of operands.
<VAR>v</VAR> stands for a variable, <VAR>x</VAR>
and <VAR>n</VAR> for expressions, and <VAR>T</VAR> for a type.
<P>
Function procedures:
<PRE>
______________________________________________________________________________
<VAR>Name</VAR>	 |  <VAR>Argument</VAR> <VAR>type</VAR>     |	 <VAR>Result</VAR>	<VAR>type</VAR> |	<VAR>Function</VAR>
_________|____________________|______________|________________________________
ABS(<VAR>x</VAR>)	 |  numeric type      |	 type of <VAR>x</VAR>   |	absolute value
_________|____________________|______________|________________________________
ODD(<VAR>x</VAR>)	 |  integer type      |	 BOOLEAN     |	<VAR>x</VAR> MOD 2	= 1
_________|____________________|______________|________________________________
CAP(<VAR>x</VAR>)	 |  CHAR	      |	 CHAR	     |	corresponding capital letter
	 |		      |		     |
_________|____________________|______________|________________________________
	 |		      |		     |	     <VAR>n</VAR>
ASH(<VAR>x</VAR>, <VAR>n</VAR>)|  <VAR>x</VAR>, <VAR>n</VAR>: integer type|	 LONGINT     |	<VAR>x</VAR> * 2 ,	arithmetic shift
_________|____________________|______________|________________________________
LEN(<VAR>v</VAR>, <VAR>n</VAR>)|  <VAR>v</VAR>: array	      |	 LONGINT     |	the length of <VAR>v</VAR>	in dimension <VAR>n</VAR>
	 |  <VAR>n</VAR>: integer type   |		     |
_________|____________________|______________|________________________________
LEN(<VAR>v</VAR>)	 |  array type	      |	 LONGINT     |	LEN(<VAR>v</VAR>, 0)
_________|____________________|______________|________________________________
MAX(<VAR>T</VAR>)	 |  T =	basic type    |	 <VAR>T</VAR>	     |	maximum	value of type <VAR>T</VAR>
	 |  T =	SET	      |	 INTEGER     |	maximum	element	of sets
_________|____________________|______________|________________________________
MIN(<VAR>T</VAR>)	 |  T =	basic type    |	 <VAR>T</VAR>	     |	minimum	value of type <VAR>T</VAR>
	 |  T =	SET	      |	 INTEGER     |	0
_________|____________________|______________|________________________________
SIZE(<VAR>T</VAR>)	 |  T =	any type      |	 integer type|	number of bytes	required by <VAR>T</VAR>
_________|____________________|______________|________________________________
</PRE>
<P>
Type conversion procedures:
<PRE>
____________________________________________________________________________
<VAR>Name</VAR>	 |  <VAR>Argument</VAR> <VAR>type</VAR>|  <VAR>Result</VAR> <VAR>type</VAR>|  <VAR>Function</VAR>
_________|_______________|_____________|____________________________________
ORD(<VAR>x</VAR>)	 |  CHAR	 |  INTEGER    |  ordinal number of <VAR>x</VAR>
_________|_______________|_____________|____________________________________
CHR(<VAR>x</VAR>)	 |  integer type |  CHAR       |  character with ordinal number	<VAR>x</VAR>
_________|_______________|_____________|____________________________________
SHORT(<VAR>x</VAR>) |  LONGINT	 |  INTEGER    |  identity
	 |  INTEGER	 |  SHORTINT   |
	 |  LONGREAL	 |  REAL       |  (truncation possible)
_________|_______________|_____________|____________________________________
LONG(<VAR>x</VAR>)	 |  SHORTINT	 |  INTEGER    |  identity
	 |  INTEGER	 |  LONGINT    |
	 |  REAL	 |  LONGREAL   |
_________|_______________|_____________|____________________________________
ENTIER(<VAR>x</VAR>)|  real type	 |  LONGINT    |  largest integer not greater than <VAR>x</VAR>
	 |		 |	       |
_________|_______________|_____________|____________________________________
</PRE>
Note that ENTIER(<VAR>i</VAR>/<VAR>j</VAR>) = <VAR>i</VAR> DIV <VAR>j</VAR>
<P>
Proper procedures:
<PRE>
_____________________________________________________________________
<VAR>Name</VAR>	  |  <VAR>Argument</VAR> <VAR>types</VAR>	       |  <VAR>Function</VAR>
__________|____________________________|_____________________________
INC(<VAR>v</VAR>)	  |  integer type	       |  <VAR>v</VAR> := <VAR>v</VAR> + 1
__________|____________________________|_____________________________
INC(<VAR>v</VAR>, <VAR>x</VAR>) |  integer type	       |  <VAR>v</VAR> := <VAR>v</VAR> + <VAR>x</VAR>
__________|____________________________|_____________________________
DEC(<VAR>v</VAR>)	  |  integer type	       |  <VAR>v</VAR> := <VAR>v</VAR> - 1
__________|____________________________|_____________________________
DEC(<VAR>v</VAR>, <VAR>x</VAR>) |  integer type	       |  <VAR>v</VAR> := <VAR>v</VAR> - <VAR>x</VAR>
__________|____________________________|_____________________________
INCL(<VAR>v</VAR>,	<VAR>x</VAR>)|  <VAR>v</VAR>:	SET; <VAR>x</VAR>:	integer	type   |  <VAR>v</VAR> := <VAR>v</VAR> + {<VAR>x</VAR>}
__________|____________________________|_____________________________
EXCL(<VAR>v</VAR>,	<VAR>x</VAR>)|  <VAR>v</VAR>:	SET; <VAR>x</VAR>:	integer	type   |  <VAR>v</VAR> := <VAR>v</VAR> - {<VAR>x</VAR>}
__________|____________________________|_____________________________
COPY(<VAR>x</VAR>,	<VAR>v</VAR>)|  <VAR>x</VAR>:	character array, string|  <VAR>v</VAR> := <VAR>x</VAR>
	  |  <VAR>v</VAR>:	character array	       |
__________|____________________________|_____________________________
NEW(<VAR>v</VAR>)	  |  pointer type	       |  allocate <VAR>v</VAR>^
__________|____________________________|_____________________________
HALT(<VAR>x</VAR>)	  |  integer constant	       |  terminate program execution
__________|____________________________|_____________________________
</PRE>
In HALT(<VAR>x</VAR>), <VAR>x</VAR> is a parameter
whose interpretation is left to the underlying
system implementation.
<H2><A NAME="11.">11.</A> Modules</H2>
<P>
A module is a collection of declarations of constants,
types, variables, and procedures, and a sequence of
statements for the purpose of assigning initial values
to the variables.
A module typically constitutes a text that
is compilable as a unit.
<PRE>
$ module = MODULE ident ``;'' [ImportList] DeclarationSequence
$          [BEGIN StatementSequence] END ident ``.''.
$ ImportList = IMPORT import {``,'' import} ``;''.
$ import = ident [&quot;:=&quot; ident].
</PRE>
<P>
The import list specifies the modules of which the module
is a client.
If an identifier <VAR>x</VAR> is exported from a module <VAR>M</VAR>,
and if <VAR>M</VAR> is listed in a module's import list,
then <VAR>x</VAR> is referred to as <VAR>M.x</VAR>.
If the form '<VAR>M</VAR> := <VAR>M1</VAR>' is used
in the import list, that object declared within <VAR>M1</VAR>
is referenced as <VAR>M.x</VAR>.
<P>
Identifiers that are to be visible in client modules,
that is outside the declaring module,
must be marked by an export mark in their declaration.
<P>
The statement sequence following the symbol BEGIN
is executed when the module is added to the system (loaded).
Individual (parameterless) procedures can thereafter
be activated from the system,
and these procedures serve as commands.
<P>
An example is
<PRE>
MODULE Out;
(* exported procedures: Write, WriteInt, WriteLn *)
IMPORT Texts, Oberon;
VAR W: Texts.Writer;

PROCEDURE Write*(ch: CHAR);
BEGIN Texts.Write(W, ch)
END Write;

PROCEDURE WriteInt*(x, n: LONGINT);
  VAR i: INTEGER; a: ARRAY 16 OF CHAR;
BEGIN i := 0;
  IF x &lt; 0 THEN Texts.Write(W, ``-''); x := -x END;
  REPEAT
    a[i] := CHR(x MOD 10 + ORD(``0'')); x := x DIV 10; INC(i);
  UNTIL x = 0;
  REPEAT Texts.Write(W, `` ''); DEC(n) UNTIL n &lt;= i;
  REPEAT DEC(i); Texts.Write(W, a[i]) UNTIL i = 0
END WriteInt;

PROCEDURE WriteLn*;
BEGIN Texts.WriteLn(W); Texts.Append(Oberon.Log, W.buf)
END WriteLn;

BEGIN Texts.OpenWriter(W)
END Out.
</PRE>
<H2><A NAME="12.">12.</A> The Module SYSTEM</H2>
<P>
The module SYSTEM contains definitions that are
necessary to program <VAR>low-level</VAR> operations
referring directly to resources particular to
a given computer and/or implementation.
These include, for example, facilities
for accessing devices that are controlled by the computer,
and facilities to break the data type compatibility rules
otherwise imposed by the language definition.
It is recommended that their use be restricted to
specific <VAR>low-level</VAR> modules.
Such modules are inherently non-portable,
but are easily recognized due to the identifier SYSTEM
appearing in their import lists.
The subsequent definitions are those that hold for
the NS32000 implementation,
but they are applicable to most modern computers.
Individual implementations may differ and include
definitions that are particular to the specific,
underlying computer.
<P>
Module SYSTEM exports the data type BYTE.
No representation of values is specified.
Instead, certain compatibility rules with other types
are given:
<DL>
<DT>(1)<DD>
The type BYTE is compatible with CHAR and SHORTINT.
<DT>(2)<DD>
If a formal variable parameter is of type ARRAY OF BYTE
then the corresponding actual parameter may be of any type.
</DL>
<P>
The procedures contained in module SYSTEM
are listed in the following tables.
They correspond to single instructions compiled
as in-line code.
For details, the reader is referred to the processor
manual.
<VAR>v</VAR> stands for a variable,
<VAR>x</VAR>, <VAR>y</VAR>, <VAR>a</VAR>, and <VAR>n</VAR> for expressions,
and <VAR>T</VAR> for a type.
<P>
Function procedures:
<PRE>
_____________________________________________________________________________
<VAR>Name</VAR>	 |  <VAR>Argument</VAR> <VAR>types</VAR>	  |  <VAR>Result</VAR> <VAR>type</VAR>|  <VAR>Function</VAR>
_________|________________________|_____________|____________________________
ADR(<VAR>v</VAR>)	 |  any			  |  LONGINT	|  address of variable <VAR>v</VAR>
_________|________________________|_____________|____________________________
BIT(<VAR>a</VAR>, <VAR>n</VAR>)|  <VAR>a</VAR>: LONGINT		  |  BOOLEAN	|  bit <VAR>n</VAR> of <VAR>Mem</VAR>[<VAR>a</VAR>]
	 |  <VAR>n</VAR>: integer type	  |		|
_________|________________________|_____________|____________________________
CC(<VAR>n</VAR>)	 |  integer constant	  |  BOOLEAN	|  Condition <VAR>n</VAR>
_________|________________________|_____________|____________________________
LSH(<VAR>x</VAR>, <VAR>n</VAR>)|  <VAR>x</VAR>: integer type of SET|  type of <VAR>x</VAR>	|  logical shift
	 |  <VAR>n</VAR>: integer type	  |		|
_________|________________________|_____________|____________________________
ROT(<VAR>x</VAR>, <VAR>n</VAR>)|  <VAR>x</VAR>: integer type or SET|  type of <VAR>x</VAR>	|  rotation
	 |  <VAR>n</VAR>: integer type	  |		|
_________|________________________|_____________|____________________________
VAL(<VAR>T</VAR>, <VAR>x</VAR>)|  <VAR>T</VAR>, <VAR>x</VAR>: any type	  |  <VAR>T</VAR>		|  <VAR>x</VAR> interpreted as of type <VAR>T</VAR>
_________<STRONG>|</STRONG>________________________<STRONG>|</STRONG>_____________<STRONG>|</STRONG>____________________________
</PRE>
<P>
Proper procedures:
<PRE>
________________________________________________________________________
<VAR>Name</VAR>	     |	<VAR>Argument</VAR> <VAR>types</VAR>	   |  <VAR>Function</VAR>
_____________|_____________________|____________________________________
GET(<VAR>a</VAR>, <VAR>v</VAR>)    |	<VAR>a</VAR>: LONGINT	   |  <VAR>v</VAR>	:= <VAR>Mem</VAR>[<VAR>a</VAR>]
	     |	<VAR>v</VAR>: any basic type  |
_____________|_____________________|____________________________________
PUT(<VAR>a</VAR>, <VAR>x</VAR>)    |	<VAR>a</VAR>: LONGINT	   |  <VAR>Mem</VAR>[<VAR>a</VAR>] :=	<VAR>x</VAR>
	     |	<VAR>x</VAR>: any basic type  |
_____________|_____________________|____________________________________
MOVE(<VAR>s</VAR>,	<VAR>d</VAR>, <VAR>n</VAR>)|	<VAR>s</VAR>, <VAR>d</VAR>: LONGINT	   |  <VAR>Mem</VAR>[<VAR>d</VAR>]...<VAR>Mem</VAR>[<VAR>d</VAR>+<VAR>n</VAR>-1] :=
	     |	<VAR>n</VAR>: integer type	   |  <VAR>Mem</VAR>[<VAR>s</VAR>]...<VAR>Mem</VAR>[<VAR>s</VAR>+<VAR>n</VAR>-1]
_____________|_____________________|____________________________________
NEW(<VAR>v</VAR>, <VAR>n</VAR>)    |	<VAR>v</VAR>: any pointer type|  allocate storage block of	<VAR>n</VAR> bytes,
	     |	<VAR>n</VAR>: integer type	   |  assign its address to <VAR>v</VAR>
_____________<STRONG>|</STRONG>_____________________<STRONG>|</STRONG>____________________________________
</PRE>
<H3>REFERENCE</H3>
<P>
This report was taken (with some minor corrections) out of
<BR>
Martin Reiser, Niklaus Wirth,
&quot;Programming in Oberon&quot;, Addison-Wesley, New York, 1992
<BR>
with the permission of Niklaus Wirth.
<H1>REVISION</H1>
Edited by: borchert<BR>
Last Change: 1995/04/03<BR>
Revision: 1.1<BR>
Converted to HTML: 1995/08/06
<BR>
<HR>
