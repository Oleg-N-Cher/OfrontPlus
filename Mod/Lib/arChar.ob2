(* Functions for common character classes *)

MODULE arChar;

PROCEDURE IsLetter*(ch : CHAR) : BOOLEAN;
BEGIN
	RETURN ((ch >= 'A') & (ch <= 'Z')) OR ((ch >= 'a') & (ch <= 'z'))
END IsLetter;

PROCEDURE IsDigit*(ch : CHAR) : BOOLEAN;
BEGIN
	RETURN (ch >= '0') & (ch <= '9');
END IsDigit;

PROCEDURE IsAlphaNumeric*(ch : CHAR) : BOOLEAN;
BEGIN
	RETURN IsLetter(ch) OR IsDigit(ch)
END IsAlphaNumeric;

PROCEDURE IsSpace*(ch : CHAR) : BOOLEAN;
BEGIN
	RETURN (ch = ' ') OR (ch = 09X) OR (ch = 0AX) OR (ch = 0DX);
END IsSpace;

END arChar.
