MODULE MyDLL; IMPORT Console;

PROCEDURE Plus* (a, b: INTEGER): INTEGER;
BEGIN
  RETURN a + b
END Plus;

BEGIN
  Console.String("MyDLL initialized"); Console.Ln
END MyDLL.
