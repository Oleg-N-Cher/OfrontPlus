/* voc 2.1.0 [2017/01/25] for gcc LP64 on centos xtpacF */

#ifndef SQLite__h
#define SQLite__h

#include "SYSTEM.h"




import void *SQLite__init(void);

extern char *sqlite3_column_text(void *stmt, int col);
extern char *sqlite3_errmsg(void *pDb);
#define SQLite_Changes(pDb)	(int)sqlite3_changes(pDb)
#define SQLite_Close(pDb)	(int)sqlite3_close(pDb)
#define SQLite_ColumnInt(stmt, col)	(int)sqlite3_column_int(stmt, col)
#define SQLite_ColumnText(stmt, col)	(char *)sqlite3_column_text(stmt, col)
#define SQLite_ErrMsg(pDb)	(char *)sqlite3_errmsg(pDb)
#define SQLite_Exec(pDb, sql, sql__len, err)	(int)sqlite3_exec(pDb, sql, (void *)0, (void *)0, err)
#define SQLite_Finalize(stmt)	(int)sqlite3_finalize(stmt)
#define SQLite_Free(x)	sqlite3_free(x)
#define SQLite_InsertId(pDb)	(int)sqlite3_last_insert_rowid(pDb)
#define SQLite_Open(filename, filename__len, ppDb)	(int)sqlite3_open(filename, ppDb)
#define SQLite_PrepareV2(pDb, sql, sql__len, stmt)	(int)sqlite3_prepare_v2(pDb, sql, -1, stmt, (void *)0)
#define SQLite_Step(stmt)	(int)sqlite3_step(stmt)

#endif // SQLite
