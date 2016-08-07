/* Ofront+ 0.9 -xtspkae */

#ifndef Files__h
#define Files__h

#include "SYSTEM.h"
#include "Platform.h"

typedef
	struct Files_FileDesc *Files_File;

typedef
	struct Files_FileDesc {
		char _prvt0[224];
		Platform_FileHandle fd;
		char _prvt1[64];
	} Files_FileDesc;

typedef
	struct Files_Rider {
		INTEGER res;
		BOOLEAN eof;
		LONGINT _prvt0;
		char _prvt1[12];
	} Files_Rider;



import LONGINT *Files_FileDesc__typ;
import LONGINT *Files_Rider__typ;

import Files_File Files_Base (Files_Rider *r, LONGINT *r__typ);
import void Files_ChangeDirectory (CHAR *path, INTEGER path__len, INTEGER *res);
import void Files_Close (Files_File f);
import void Files_Delete (CHAR *name, INTEGER name__len, INTEGER *res);
import void Files_GetDate (Files_File f, INTEGER *t, INTEGER *d);
import void Files_GetName (Files_File f, CHAR *name, INTEGER name__len);
import LONGINT Files_Length (Files_File f);
import Files_File Files_New (CHAR *name, INTEGER name__len);
import Files_File Files_Old (CHAR *name, INTEGER name__len);
import LONGINT Files_Pos (Files_Rider *r, LONGINT *r__typ);
import void Files_Purge (Files_File f);
import void Files_ReadBool (Files_Rider *R, LONGINT *R__typ, BOOLEAN *x);
import void Files_ReadByte (Files_Rider *r, LONGINT *r__typ, BYTE *x);
import void Files_ReadBytes (Files_Rider *r, LONGINT *r__typ, BYTE *x, INTEGER x__len, INTEGER n);
import void Files_ReadChar (Files_Rider *r, LONGINT *r__typ, CHAR *x);
import void Files_ReadInt (Files_Rider *R, LONGINT *R__typ, INTEGER *x);
import void Files_ReadLInt (Files_Rider *R, LONGINT *R__typ, LONGINT *x);
import void Files_ReadLReal (Files_Rider *R, LONGINT *R__typ, LONGREAL *x);
import void Files_ReadLine (Files_Rider *R, LONGINT *R__typ, CHAR *x, INTEGER x__len);
import void Files_ReadNum (Files_Rider *R, LONGINT *R__typ, LONGINT *x);
import void Files_ReadReal (Files_Rider *R, LONGINT *R__typ, REAL *x);
import void Files_ReadSInt (Files_Rider *R, LONGINT *R__typ, SHORTINT *x);
import void Files_ReadSet (Files_Rider *R, LONGINT *R__typ, SET *x);
import void Files_ReadString (Files_Rider *R, LONGINT *R__typ, CHAR *x, INTEGER x__len);
import void Files_Register (Files_File f);
import void Files_Rename (CHAR *old, INTEGER old__len, CHAR *new, INTEGER new__len, INTEGER *res);
import void Files_Set (Files_Rider *r, LONGINT *r__typ, Files_File f, LONGINT pos);
import void Files_SetSearchPath (CHAR *path, INTEGER path__len);
import void Files_WriteBool (Files_Rider *R, LONGINT *R__typ, BOOLEAN x);
import void Files_WriteByte (Files_Rider *r, LONGINT *r__typ, BYTE x);
import void Files_WriteBytes (Files_Rider *r, LONGINT *r__typ, BYTE *x, INTEGER x__len, INTEGER n);
import void Files_WriteChar (Files_Rider *R, LONGINT *R__typ, CHAR x);
import void Files_WriteInt (Files_Rider *R, LONGINT *R__typ, INTEGER x);
import void Files_WriteLInt (Files_Rider *R, LONGINT *R__typ, LONGINT x);
import void Files_WriteLReal (Files_Rider *R, LONGINT *R__typ, LONGREAL x);
import void Files_WriteNum (Files_Rider *R, LONGINT *R__typ, LONGINT x);
import void Files_WriteReal (Files_Rider *R, LONGINT *R__typ, REAL x);
import void Files_WriteSInt (Files_Rider *R, LONGINT *R__typ, SHORTINT x);
import void Files_WriteSet (Files_Rider *R, LONGINT *R__typ, SET x);
import void Files_WriteString (Files_Rider *R, LONGINT *R__typ, CHAR *x, INTEGER x__len);
import void *Files__init(void);


#endif
