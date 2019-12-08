MODULE SQLite;
IMPORT SYSTEM;

CONST
  row*  = 100;
  done* = 101;

TYPE
  Sqlite3* = POINTER [1] TO ARRAY [1] 1 OF SHORTCHAR;
  Stmt* = POINTER [1] TO ARRAY [1] 1 OF SHORTCHAR;
  Ptr* = POINTER [1] TO ARRAY [1] 1 OF SHORTCHAR;

PROCEDURE -Open*
  (filename: ARRAY OF SHORTCHAR; VAR ppDb: Sqlite3): INTEGER
  "(int)sqlite3_open(filename, ppDb)";

PROCEDURE -Close*
  (pDb: Sqlite3): INTEGER
  "(int)sqlite3_close(pDb)";

PROCEDURE -Free*
  (x: Ptr)
  "sqlite3_free(x)";

PROCEDURE -Exec*
  (pDb: Sqlite3; sql: ARRAY OF SHORTCHAR; VAR err: Ptr): INTEGER
  "(int)sqlite3_exec(pDb, sql, (void *)0, (void *)0, err)";

PROCEDURE -PrepareV2*
  (pDb: Sqlite3; sql: ARRAY OF SHORTCHAR; VAR stmt: Stmt): INTEGER
  "(int)sqlite3_prepare_v2(pDb, sql, -1, stmt, (void *)0)";

PROCEDURE -Finalize*
  (stmt: Stmt): INTEGER
  "(int)sqlite3_finalize(stmt)";

PROCEDURE -Step*
  (stmt: Stmt): INTEGER
  "(int)sqlite3_step(stmt)";

PROCEDURE -AADefineColumnText*
  "extern char *sqlite3_column_text(void *stmt, int col);";

PROCEDURE -ColumnText*
  (stmt: Stmt; col: INTEGER): Ptr
  "(char *)sqlite3_column_text(stmt, col)";

PROCEDURE -ColumnInt*
  (stmt: Stmt; col: INTEGER): INTEGER
  "(int)sqlite3_column_int(stmt, col)";

PROCEDURE -Changes*
  (pDb: Sqlite3): INTEGER
  "(int)sqlite3_changes(pDb)";

PROCEDURE -InsertId*
  (pDb: Sqlite3): INTEGER
  "(int)sqlite3_last_insert_rowid(pDb)";

PROCEDURE -AADefineErrMsg*
  "extern char *sqlite3_errmsg(void *pDb);";

PROCEDURE -ErrMsg*
  (pDb: Sqlite3): Ptr
  "(char *)sqlite3_errmsg(pDb)";

END SQLite.
