MODULE MyDLLuse; IMPORT MyDLL, Console;
BEGIN
  Console.String("4 + 3 = "); Console.Int(MyDLL.Plus(4, 3), 0);
  Console.Ln
END MyDLLuse.
