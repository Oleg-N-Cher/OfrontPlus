/* Ofront 1.2 -xtspkae */
#include "SYSTEM.h"
#include "Args.h"
#include "Console.h"
#include "Kernel.h"
#include "Unix.h"

typedef
	struct Files_Handle *Files_File;

typedef
	struct Files_BufDesc {
		Files_File f;
		BOOLEAN chg;
		LONGINT org, size;
		BYTE data[4096];
	} Files_BufDesc;

typedef
	Files_BufDesc *Files_Buffer;

typedef
	CHAR Files_FileName[101];

typedef
	struct Files_Handle {
		Files_FileName workName, registerName;
		BOOLEAN tempFile;
		LONGINT dev, ino, mtime, fd, len, pos;
		Files_Buffer bufs[4];
		INTEGER swapper, state;
	} Files_Handle;

typedef
	struct Files_Rider {
		LONGINT res;
		BOOLEAN eof;
		Files_Buffer buf;
		LONGINT org, offset;
	} Files_Rider;

typedef
	struct Files_TimeDesc *Files_Time;

typedef
	struct Files_TimeDesc {
		LONGINT sec, min, hour, mday, mon, year, wday, isdst, zone, gmtoff;
	} Files_TimeDesc;


static LONGINT Files_fileTab[64];
static INTEGER Files_tempno;

export LONGINT *Files_Handle__typ;
export LONGINT *Files_BufDesc__typ;
export LONGINT *Files_Rider__typ;
export LONGINT *Files_TimeDesc__typ;

export Files_File Files_Base (Files_Rider *r, LONGINT *r__typ);
static Files_File Files_CacheEntry (LONGINT dev, LONGINT ino, LONGINT mtime);
export void Files_ChangeDirectory (CHAR *path, LONGINT path__len, INTEGER *res);
export void Files_Close (Files_File f);
static void Files_Create (Files_File f);
export void Files_Delete (CHAR *name, LONGINT name__len, INTEGER *res);
static void Files_Err (CHAR *s, LONGINT s__len, Files_File f, LONGINT errno);
static void Files_Finalize (SYSTEM_PTR o);
static void Files_FlipBytes (BYTE *src, LONGINT src__len, BYTE *dest, LONGINT dest__len);
static void Files_Flush (Files_Buffer buf);
export void Files_GetDate (Files_File f, LONGINT *t, LONGINT *d);
static void Files_GetTempName (CHAR *finalName, LONGINT finalName__len, CHAR *name, LONGINT name__len);
static BOOLEAN Files_HasDir (CHAR *name, LONGINT name__len);
static void Files_Init (void);
export LONGINT Files_Length (Files_File f);
static void Files_MakeFileName (CHAR *dir, LONGINT dir__len, CHAR *name, LONGINT name__len, CHAR *dest, LONGINT dest__len);
export Files_File Files_New (CHAR *name, LONGINT name__len);
export Files_File Files_Old (CHAR *name, LONGINT name__len);
export LONGINT Files_Pos (Files_Rider *r, LONGINT *r__typ);
export void Files_Purge (Files_File f);
export void Files_Read (Files_Rider *r, LONGINT *r__typ, BYTE *x);
export void Files_ReadBool (Files_Rider *R, LONGINT *R__typ, BOOLEAN *x);
export void Files_ReadBytes (Files_Rider *r, LONGINT *r__typ, BYTE *x, LONGINT x__len, LONGINT n);
export void Files_ReadInt (Files_Rider *R, LONGINT *R__typ, INTEGER *x);
export void Files_ReadLInt (Files_Rider *R, LONGINT *R__typ, LONGINT *x);
export void Files_ReadLReal (Files_Rider *R, LONGINT *R__typ, LONGREAL *x);
export void Files_ReadNum (Files_Rider *R, LONGINT *R__typ, LONGINT *x);
export void Files_ReadReal (Files_Rider *R, LONGINT *R__typ, REAL *x);
export void Files_ReadSet (Files_Rider *R, LONGINT *R__typ, SET *x);
export void Files_ReadString (Files_Rider *R, LONGINT *R__typ, CHAR *x, LONGINT x__len);
export void Files_Register (Files_File f);
export void Files_Rename (CHAR *old, LONGINT old__len, CHAR *new, LONGINT new__len, INTEGER *res);
static void Files_ScanPath (INTEGER *pos, CHAR *dir, LONGINT dir__len);
export void Files_Set (Files_Rider *r, LONGINT *r__typ, Files_File f, LONGINT pos);
export void Files_Write (Files_Rider *r, LONGINT *r__typ, BYTE x);
export void Files_WriteBool (Files_Rider *R, LONGINT *R__typ, BOOLEAN x);
export void Files_WriteBytes (Files_Rider *r, LONGINT *r__typ, BYTE *x, LONGINT x__len, LONGINT n);
export void Files_WriteInt (Files_Rider *R, LONGINT *R__typ, INTEGER x);
export void Files_WriteLInt (Files_Rider *R, LONGINT *R__typ, LONGINT x);
export void Files_WriteLReal (Files_Rider *R, LONGINT *R__typ, LONGREAL x);
export void Files_WriteNum (Files_Rider *R, LONGINT *R__typ, LONGINT x);
export void Files_WriteReal (Files_Rider *R, LONGINT *R__typ, REAL x);
export void Files_WriteSet (Files_Rider *R, LONGINT *R__typ, SET x);
export void Files_WriteString (Files_Rider *R, LONGINT *R__typ, CHAR *x, LONGINT x__len);

#define Files_IdxTrap()	__HALT(-1)
#define Files_getcwd(cwd, cwd__len)	getcwd(cwd, cwd__len)
#define Files_localtime(clock)	(Files_Time) localtime(clock)

static void Files_Err (CHAR *s, LONGINT s__len, Files_File f, LONGINT errno)
{
	__DUP(s, s__len, CHAR);
	Console_Ln();
	Console_String((CHAR*)"-- ", (LONGINT)4);
	Console_String(s, s__len);
	Console_String((CHAR*)": ", (LONGINT)3);
	if (f != NIL) {
		if (f->registerName[0] != 0x00) {
			Console_String(f->registerName, 101);
		} else {
			Console_String(f->workName, 101);
		}
	}
	if (errno != 0) {
		Console_String((CHAR*)" errno = ", (LONGINT)10);
		Console_Int(errno, 1);
	}
	Console_Ln();
	__HALT(99);
	__DEL(s);
}

static void Files_MakeFileName (CHAR *dir, LONGINT dir__len, CHAR *name, LONGINT name__len, CHAR *dest, LONGINT dest__len)
{
	INTEGER i, j;
	__DUP(dir, dir__len, CHAR);
	__DUP(name, name__len, CHAR);
	i = 0;
	j = 0;
	while (dir[__X(i, dir__len)] != 0x00) {
		dest[__X(i, dest__len)] = dir[__X(i, dir__len)];
		i += 1;
	}
	if (dest[__X(i - 1, dest__len)] != '/') {
		dest[__X(i, dest__len)] = '/';
		i += 1;
	}
	while (name[__X(j, name__len)] != 0x00) {
		dest[__X(i, dest__len)] = name[__X(j, name__len)];
		i += 1;
		j += 1;
	}
	dest[__X(i, dest__len)] = 0x00;
	__DEL(dir);
	__DEL(name);
}

static void Files_GetTempName (CHAR *finalName, LONGINT finalName__len, CHAR *name, LONGINT name__len)
{
	LONGINT n, i, j;
	__DUP(finalName, finalName__len, CHAR);
	Files_tempno += 1;
	n = Files_tempno;
	i = 0;
	if (finalName[0] != '/') {
		while (Kernel_CWD[__X(i, 256)] != 0x00) {
			name[__X(i, name__len)] = Kernel_CWD[__X(i, 256)];
			i += 1;
		}
		if (Kernel_CWD[__X(i - 1, 256)] != '/') {
			name[__X(i, name__len)] = '/';
			i += 1;
		}
	}
	j = 0;
	while (finalName[__X(j, finalName__len)] != 0x00) {
		name[__X(i, name__len)] = finalName[__X(j, finalName__len)];
		i += 1;
		j += 1;
	}
	i -= 1;
	while (name[__X(i, name__len)] != '/') {
		i -= 1;
	}
	name[__X(i + 1, name__len)] = '.';
	name[__X(i + 2, name__len)] = 't';
	name[__X(i + 3, name__len)] = 'm';
	name[__X(i + 4, name__len)] = 'p';
	name[__X(i + 5, name__len)] = '.';
	i += 6;
	while (n > 0) {
		name[__X(i, name__len)] = (CHAR)(__MOD(n, 10) + 48);
		n = __DIV(n, 10);
		i += 1;
	}
	name[__X(i, name__len)] = '.';
	i += 1;
	n = (int)Unix_Getpid();
	while (n > 0) {
		name[__X(i, name__len)] = (CHAR)(__MOD(n, 10) + 48);
		n = __DIV(n, 10);
		i += 1;
	}
	name[__X(i, name__len)] = 0x00;
	__DEL(finalName);
}

static void Files_Create (Files_File f)
{
	Unix_Status stat;
	BOOLEAN done;
	LONGINT errno;
	CHAR err[32];
	if (f->fd == -1) {
		if (f->state == 1) {
			Files_GetTempName(f->registerName, 101, (void*)f->workName, 101);
			f->tempFile = 1;
		} else if (f->state == 2) {
			__MOVE(f->registerName, f->workName, 101);
			f->registerName[0] = 0x00;
			f->tempFile = 0;
		}
		errno = Unix_Unlink(f->workName, 101);
		f->fd = Unix_Open(f->workName, 101, 0x0242, 0x01b4);
		done = f->fd >= 0;
		errno = Unix_errno();
		if (!done && (errno == 23 || errno == 24) || done && f->fd >= 64) {
			if (done && f->fd >= 64) {
				errno = Unix_Close(f->fd);
			}
			Kernel_GC(1);
			f->fd = Unix_Open(f->workName, 101, 0x0242, 0x01b4);
			done = f->fd >= 0;
		}
		if (done) {
			if (f->fd >= 64) {
				errno = Unix_Close(f->fd);
				Files_Err((CHAR*)"too many files open", (LONGINT)20, f, 0);
			} else {
				Files_fileTab[__X(f->fd, 64)] = (LONGINT)f;
				Kernel_nofiles += 1;
				Kernel_RegisterObject((void*)f, Files_Finalize);
				f->state = 0;
				f->pos = 0;
				errno = Unix_Fstat(f->fd, &stat, Unix_Status__typ);
				f->dev = stat.dev;
				f->ino = stat.ino;
				f->mtime = stat.mtime;
			}
		} else {
			errno = Unix_errno();
			if (errno == 2) {
				__MOVE("no such directory", err, 18);
			} else if (errno == 23 || errno == 24) {
				__MOVE("too many files open", err, 20);
			} else {
				__MOVE("file not created", err, 17);
			}
			Files_Err(err, 32, f, errno);
		}
	}
}

static void Files_Flush (Files_Buffer buf)
{
	LONGINT res;
	Files_File f = NIL;
	Unix_Status stat;
	if (buf->chg) {
		f = buf->f;
		Files_Create(f);
		if (buf->org != f->pos) {
			res = Unix_Lseek(f->fd, buf->org, 0);
		}
		res = Unix_Write(f->fd, (LONGINT)buf->data, buf->size);
		if (res < 0) {
			Files_Err((CHAR*)"error in writing file", (LONGINT)22, f, Unix_errno());
		}
		f->pos = buf->org + buf->size;
		buf->chg = 0;
		res = Unix_Fstat(f->fd, &stat, Unix_Status__typ);
		f->mtime = stat.mtime;
	}
}

void Files_Close (Files_File f)
{
	LONGINT i, res;
	if (f->state != 1 || f->registerName[0] != 0x00) {
		Files_Create(f);
		i = 0;
		while (i < 4 && f->bufs[__X(i, 4)] != NIL) {
			Files_Flush(f->bufs[__X(i, 4)]);
			i += 1;
		}
		res = Unix_Fsync(f->fd);
		if (res < 0) {
			Files_Err((CHAR*)"error in writing file", (LONGINT)22, f, Unix_errno());
		}
	}
}

LONGINT Files_Length (Files_File f)
{
	return f->len;
}

Files_File Files_New (CHAR *name, LONGINT name__len)
{
	Files_File f = NIL;
	__DUP(name, name__len, CHAR);
	__NEW(f, Files_Handle);
	f->workName[0] = 0x00;
	__COPY(name, f->registerName, 101);
	f->fd = -1;
	f->state = 1;
	f->len = 0;
	f->pos = 0;
	f->swapper = -1;
	__DEL(name);
	return f;
}

static void Files_ScanPath (INTEGER *pos, CHAR *dir, LONGINT dir__len)
{
	INTEGER i;
	CHAR ch;
	CHAR home[256];
	i = 0;
	ch = Kernel_OBERON[__X(*pos, 1024)];
	while (ch == ' ' || ch == ':') {
		*pos += 1;
		ch = Kernel_OBERON[__X(*pos, 1024)];
	}
	if (ch == '~') {
		*pos += 1;
		ch = Kernel_OBERON[__X(*pos, 1024)];
		home[0] = 0x00;
		Args_GetEnv((CHAR*)"HOME", (LONGINT)5, (void*)home, 256);
		while (home[__X(i, 256)] != 0x00) {
			dir[__X(i, dir__len)] = home[__X(i, 256)];
			i += 1;
		}
		if (((ch != '/' && ch != 0x00) && ch != ':') && ch != ' ') {
			while (i > 0 && dir[__X(i - 1, dir__len)] != '/') {
				i -= 1;
			}
		}
	}
	while (ch != 0x00 && ch != ':') {
		dir[__X(i, dir__len)] = ch;
		i += 1;
		*pos += 1;
		ch = Kernel_OBERON[__X(*pos, 1024)];
	}
	while (i > 0 && dir[__X(i - 1, dir__len)] == ' ') {
		i -= 1;
	}
	dir[__X(i, dir__len)] = 0x00;
}

static BOOLEAN Files_HasDir (CHAR *name, LONGINT name__len)
{
	INTEGER i;
	CHAR ch;
	i = 0;
	ch = name[0];
	while (ch != 0x00 && ch != '/') {
		i += 1;
		ch = name[__X(i, name__len)];
	}
	return ch == '/';
}

static Files_File Files_CacheEntry (LONGINT dev, LONGINT ino, LONGINT mtime)
{
	Files_File f = NIL;
	INTEGER i;
	Unix_Status stat;
	LONGINT res;
	i = 0;
	while (i < 64) {
		f = (Files_File)Files_fileTab[__X(i, 64)];
		if ((f != NIL && ino == f->ino) && dev == f->dev) {
			if (mtime != f->mtime) {
				i = 0;
				while (i < 4) {
					if (f->bufs[__X(i, 4)] != NIL) {
						f->bufs[__X(i, 4)]->org = -1;
						f->bufs[__X(i, 4)] = NIL;
					}
					i += 1;
				}
				f->swapper = -1;
				f->mtime = mtime;
				res = Unix_Fstat(f->fd, &stat, Unix_Status__typ);
				f->len = stat.size;
			}
			return f;
		}
		i += 1;
	}
	return NIL;
}

Files_File Files_Old (CHAR *name, LONGINT name__len)
{
	Files_File f = NIL;
	LONGINT fd, res, errno;
	INTEGER pos;
	BOOLEAN done;
	CHAR dir[256], path[256];
	Unix_Status stat;
	__DUP(name, name__len, CHAR);
	if (name[0] != 0x00) {
		if (Files_HasDir((void*)name, name__len)) {
			dir[0] = 0x00;
			__COPY(name, path, 256);
		} else {
			pos = 0;
			Files_ScanPath(&pos, (void*)dir, 256);
			Files_MakeFileName(dir, 256, name, name__len, (void*)path, 256);
			Files_ScanPath(&pos, (void*)dir, 256);
		}
		for (;;) {
			fd = Unix_Open(path, 256, 0x02, 0x0);
			done = fd >= 0;
			errno = Unix_errno();
			if (!done && (errno == 23 || errno == 24) || done && fd >= 64) {
				if (done && fd >= 64) {
					res = Unix_Close(fd);
				}
				Kernel_GC(1);
				fd = Unix_Open(path, 256, 0x02, 0x0);
				done = fd >= 0;
				errno = Unix_errno();
				if (!done && (errno == 23 || errno == 24)) {
					Files_Err((CHAR*)"too many files open", (LONGINT)20, f, errno);
				}
			}
			if (!done && ((errno == 13 || errno == 30) || errno == 11)) {
				fd = Unix_Open(path, 256, 0x0, 0x0);
				done = fd >= 0;
				errno = Unix_errno();
			}
			if (!done && errno != 2) {
				Console_String((CHAR*)"warning Files.Old ", (LONGINT)19);
				Console_String(name, name__len);
				Console_String((CHAR*)" errno = ", (LONGINT)10);
				Console_Int(errno, 0);
				Console_Ln();
			}
			if (done) {
				res = Unix_Fstat(fd, &stat, Unix_Status__typ);
				f = Files_CacheEntry(stat.dev, stat.ino, stat.mtime);
				if (f != NIL) {
					res = Unix_Close(fd);
					__DEL(name);
					return f;
				} else if (fd >= 64) {
					res = Unix_Close(fd);
					Files_Err((CHAR*)"too many files open", (LONGINT)20, f, 0);
				} else {
					__NEW(f, Files_Handle);
					Files_fileTab[__X(fd, 64)] = (LONGINT)f;
					Kernel_nofiles += 1;
					Kernel_RegisterObject((void*)f, Files_Finalize);
					f->fd = fd;
					f->state = 0;
					f->len = stat.size;
					f->pos = 0;
					f->swapper = -1;
					__COPY(name, f->workName, 101);
					f->registerName[0] = 0x00;
					f->tempFile = 0;
					f->dev = stat.dev;
					f->ino = stat.ino;
					f->mtime = stat.mtime;
					__DEL(name);
					return f;
				}
			} else if (dir[0] == 0x00) {
				__DEL(name);
				return NIL;
			} else {
				Files_MakeFileName(dir, 256, name, name__len, (void*)path, 256);
				Files_ScanPath(&pos, (void*)dir, 256);
			}
		}
	} else {
		__DEL(name);
		return NIL;
	}
	__RETCHK;
}

void Files_Purge (Files_File f)
{
	INTEGER i;
	Unix_Status stat;
	LONGINT res;
	i = 0;
	while (i < 4) {
		if (f->bufs[__X(i, 4)] != NIL) {
			f->bufs[__X(i, 4)]->org = -1;
			f->bufs[__X(i, 4)] = NIL;
		}
		i += 1;
	}
	if (f->fd != -1) {
		res = Unix_Ftruncate(f->fd, 0);
		res = Unix_Lseek(f->fd, 0, 0);
	}
	f->pos = 0;
	f->len = 0;
	f->swapper = -1;
	res = Unix_Fstat(f->fd, &stat, Unix_Status__typ);
	f->mtime = stat.mtime;
}

void Files_GetDate (Files_File f, LONGINT *t, LONGINT *d)
{
	Unix_Status stat;
	LONGINT clock, res;
	Files_Time time = NIL;
	Files_Create(f);
	res = Unix_Fstat(f->fd, &stat, Unix_Status__typ);
	time = Files_localtime(&stat.mtime);
	*t = (time->sec + __ASHL(time->min, 6)) + __ASHL(time->hour, 12);
	*d = (time->mday + __ASHL(time->mon + 1, 5)) + __ASHL(__MOD(time->year, 100), 9);
}

LONGINT Files_Pos (Files_Rider *r, LONGINT *r__typ)
{
	return (*r).org + (*r).offset;
}

void Files_Set (Files_Rider *r, LONGINT *r__typ, Files_File f, LONGINT pos)
{
	LONGINT org, offset, i, n, res;
	Files_Buffer buf = NIL;
	if (f != NIL) {
		if (pos > f->len) {
			pos = f->len;
		} else if (pos < 0) {
			pos = 0;
		}
		offset = __MASK(pos, -4096);
		org = pos - offset;
		i = 0;
		while ((i < 4 && f->bufs[__X(i, 4)] != NIL) && org != f->bufs[__X(i, 4)]->org) {
			i += 1;
		}
		if (i < 4) {
			if (f->bufs[__X(i, 4)] == NIL) {
				__NEW(buf, Files_BufDesc);
				buf->chg = 0;
				buf->org = -1;
				buf->f = f;
				f->bufs[__X(i, 4)] = buf;
			} else {
				buf = f->bufs[__X(i, 4)];
			}
		} else {
			f->swapper = __MASK(f->swapper + 1, -4);
			buf = f->bufs[__X(f->swapper, 4)];
			Files_Flush(buf);
		}
		if (buf->org != org) {
			if (org == f->len) {
				buf->size = 0;
			} else {
				Files_Create(f);
				if (f->pos != org) {
					res = Unix_Lseek(f->fd, org, 0);
				}
				n = Unix_ReadBlk(f->fd, (void*)buf->data, 4096);
				if (n < 0) {
					Files_Err((CHAR*)"read from file not done", (LONGINT)24, f, Unix_errno());
				}
				f->pos = org + n;
				buf->size = n;
			}
			buf->org = org;
			buf->chg = 0;
		}
	} else {
		buf = NIL;
		org = 0;
		offset = 0;
	}
	(*r).buf = buf;
	(*r).org = org;
	(*r).offset = offset;
	(*r).eof = 0;
	(*r).res = 0;
}

void Files_Read (Files_Rider *r, LONGINT *r__typ, BYTE *x)
{
	LONGINT offset;
	Files_Buffer buf = NIL;
	buf = (*r).buf;
	offset = (*r).offset;
	if ((*r).org != buf->org) {
		Files_Set(&*r, r__typ, buf->f, (*r).org + offset);
		buf = (*r).buf;
		offset = (*r).offset;
	}
	if (offset < buf->size) {
		*x = buf->data[__X(offset, 4096)];
		(*r).offset = offset + 1;
	} else if ((*r).org + offset < buf->f->len) {
		Files_Set(&*r, r__typ, (*r).buf->f, (*r).org + offset);
		*x = (*r).buf->data[0];
		(*r).offset = 1;
	} else {
		*x = 0x00;
		(*r).eof = 1;
	}
}

void Files_ReadBytes (Files_Rider *r, LONGINT *r__typ, BYTE *x, LONGINT x__len, LONGINT n)
{
	LONGINT xpos, min, restInBuf, offset;
	Files_Buffer buf = NIL;
	if (n > x__len) {
		Files_IdxTrap();
	}
	xpos = 0;
	buf = (*r).buf;
	offset = (*r).offset;
	while (n > 0) {
		if ((*r).org != buf->org || offset >= 4096) {
			Files_Set(&*r, r__typ, buf->f, (*r).org + offset);
			buf = (*r).buf;
			offset = (*r).offset;
		}
		restInBuf = buf->size - offset;
		if (restInBuf == 0) {
			(*r).res = n;
			(*r).eof = 1;
			return;
		} else if (n > restInBuf) {
			min = restInBuf;
		} else {
			min = n;
		}
		__MOVE((LONGINT)buf->data + offset, (LONGINT)x + xpos, min);
		offset += min;
		(*r).offset = offset;
		xpos += min;
		n -= min;
	}
	(*r).res = 0;
	(*r).eof = 0;
}

Files_File Files_Base (Files_Rider *r, LONGINT *r__typ)
{
	return (*r).buf->f;
}

void Files_Write (Files_Rider *r, LONGINT *r__typ, BYTE x)
{
	Files_Buffer buf = NIL;
	LONGINT offset;
	buf = (*r).buf;
	offset = (*r).offset;
	if ((*r).org != buf->org || offset >= 4096) {
		Files_Set(&*r, r__typ, buf->f, (*r).org + offset);
		buf = (*r).buf;
		offset = (*r).offset;
	}
	buf->data[__X(offset, 4096)] = x;
	buf->chg = 1;
	if (offset == buf->size) {
		buf->size += 1;
		buf->f->len += 1;
	}
	(*r).offset = offset + 1;
	(*r).res = 0;
}

void Files_WriteBytes (Files_Rider *r, LONGINT *r__typ, BYTE *x, LONGINT x__len, LONGINT n)
{
	LONGINT xpos, min, restInBuf, offset;
	Files_Buffer buf = NIL;
	if (n > x__len) {
		Files_IdxTrap();
	}
	xpos = 0;
	buf = (*r).buf;
	offset = (*r).offset;
	while (n > 0) {
		if ((*r).org != buf->org || offset >= 4096) {
			Files_Set(&*r, r__typ, buf->f, (*r).org + offset);
			buf = (*r).buf;
			offset = (*r).offset;
		}
		restInBuf = 4096 - offset;
		if (n > restInBuf) {
			min = restInBuf;
		} else {
			min = n;
		}
		__MOVE((LONGINT)x + xpos, (LONGINT)buf->data + offset, min);
		offset += min;
		(*r).offset = offset;
		if (offset > buf->size) {
			buf->f->len += offset - buf->size;
			buf->size = offset;
		}
		xpos += min;
		n -= min;
		buf->chg = 1;
	}
	(*r).res = 0;
}

void Files_Delete (CHAR *name, LONGINT name__len, INTEGER *res)
{
	__DUP(name, name__len, CHAR);
	*res = (int)Unix_Unlink(name, name__len);
	*res = (int)Unix_errno();
	__DEL(name);
}

void Files_Rename (CHAR *old, LONGINT old__len, CHAR *new, LONGINT new__len, INTEGER *res)
{
	LONGINT fdold, fdnew, n, errno, r;
	Unix_Status ostat, nstat;
	CHAR buf[4096];
	__DUP(old, old__len, CHAR);
	__DUP(new, new__len, CHAR);
	r = Unix_Stat(old, old__len, &ostat, Unix_Status__typ);
	if (r >= 0) {
		r = Unix_Stat(new, new__len, &nstat, Unix_Status__typ);
		if (r >= 0 && (ostat.dev != nstat.dev || ostat.ino != nstat.ino)) {
			Files_Delete(new, new__len, &*res);
		}
		r = Unix_Rename(old, old__len, new, new__len);
		if (r < 0) {
			*res = (int)Unix_errno();
			if (*res == 18) {
				fdold = Unix_Open(old, old__len, 0x0, 0x0);
				if (fdold < 0) {
					*res = 2;
					__DEL(old);
					__DEL(new);
					return;
				}
				fdnew = Unix_Open(new, new__len, 0x0242, 0x01b4);
				if (fdnew < 0) {
					r = Unix_Close(fdold);
					*res = 3;
					__DEL(old);
					__DEL(new);
					return;
				}
				n = Unix_Read(fdold, (LONGINT)buf, 4096);
				while (n > 0) {
					r = Unix_Write(fdnew, (LONGINT)buf, n);
					if (r < 0) {
						errno = Unix_errno();
						r = Unix_Close(fdold);
						r = Unix_Close(fdnew);
						Files_Err((CHAR*)"cannot move file", (LONGINT)17, NIL, errno);
					}
					n = Unix_Read(fdold, (LONGINT)buf, 4096);
				}
				errno = Unix_errno();
				r = Unix_Close(fdold);
				r = Unix_Close(fdnew);
				if (n == 0) {
					r = Unix_Unlink(old, old__len);
					*res = 0;
				} else {
					Files_Err((CHAR*)"cannot move file", (LONGINT)17, NIL, errno);
				}
			} else {
				__DEL(old);
				__DEL(new);
				return;
			}
		}
		*res = 0;
	} else {
		*res = 2;
	}
	__DEL(old);
	__DEL(new);
}

void Files_Register (Files_File f)
{
	INTEGER idx, errno;
	Files_File f1 = NIL;
	CHAR file[104];
	if (f->state == 1 && f->registerName[0] != 0x00) {
		f->state = 2;
	}
	Files_Close(f);
	if (f->registerName[0] != 0x00) {
		Files_Rename(f->workName, 101, f->registerName, 101, &errno);
		if (errno != 0) {
			__COPY(f->registerName, file, 104);
			__HALT(99);
		}
		__MOVE(f->registerName, f->workName, 101);
		f->registerName[0] = 0x00;
		f->tempFile = 0;
	}
}

void Files_ChangeDirectory (CHAR *path, LONGINT path__len, INTEGER *res)
{
	__DUP(path, path__len, CHAR);
	*res = (int)Unix_Chdir(path, path__len);
	Files_getcwd((void*)Kernel_CWD, 256);
	__DEL(path);
}

static void Files_FlipBytes (BYTE *src, LONGINT src__len, BYTE *dest, LONGINT dest__len)
{
	LONGINT i, j;
	if (!Kernel_littleEndian) {
		i = src__len;
		j = 0;
		while (i > 0) {
			i -= 1;
			dest[__X(j, dest__len)] = src[__X(i, src__len)];
			j += 1;
		}
	} else {
		__MOVE((LONGINT)src, (LONGINT)dest, src__len);
	}
}

void Files_ReadBool (Files_Rider *R, LONGINT *R__typ, BOOLEAN *x)
{
	Files_Read(&*R, R__typ, (CHAR*)(void*)&*x);
}

void Files_ReadInt (Files_Rider *R, LONGINT *R__typ, INTEGER *x)
{
	CHAR b[2];
	Files_ReadBytes(&*R, R__typ, (void*)b, 2, 2);
	*x = (int)b[0] + __ASHL((int)b[1], 8);
}

void Files_ReadLInt (Files_Rider *R, LONGINT *R__typ, LONGINT *x)
{
	CHAR b[4];
	Files_ReadBytes(&*R, R__typ, (void*)b, 4, 4);
	*x = ((LONGINT)((int)b[0] + __ASHL((int)b[1], 8)) + __ASHL((LONGINT)b[2], 16)) + __ASHL((LONGINT)b[3], 24);
}

void Files_ReadSet (Files_Rider *R, LONGINT *R__typ, SET *x)
{
	CHAR b[4];
	Files_ReadBytes(&*R, R__typ, (void*)b, 4, 4);
	*x = (SET)(((LONGINT)((int)b[0] + __ASHL((int)b[1], 8)) + __ASHL((LONGINT)b[2], 16)) + __ASHL((LONGINT)b[3], 24));
}

void Files_ReadReal (Files_Rider *R, LONGINT *R__typ, REAL *x)
{
	CHAR b[4];
	Files_ReadBytes(&*R, R__typ, (void*)b, 4, 4);
	Files_FlipBytes((void*)b, 4, (void*)&*x, 4);
}

void Files_ReadLReal (Files_Rider *R, LONGINT *R__typ, LONGREAL *x)
{
	CHAR b[8];
	Files_ReadBytes(&*R, R__typ, (void*)b, 8, 8);
	Files_FlipBytes((void*)b, 8, (void*)&*x, 8);
}

void Files_ReadString (Files_Rider *R, LONGINT *R__typ, CHAR *x, LONGINT x__len)
{
	INTEGER i;
	CHAR ch;
	i = 0;
	do {
		Files_Read(&*R, R__typ, (void*)&ch);
		x[__X(i, x__len)] = ch;
		i += 1;
	} while (!(ch == 0x00));
}

void Files_ReadNum (Files_Rider *R, LONGINT *R__typ, LONGINT *x)
{
	SHORTINT s;
	CHAR ch;
	LONGINT n;
	s = 0;
	n = 0;
	Files_Read(&*R, R__typ, (void*)&ch);
	while ((int)ch >= 128) {
		n += __ASH((LONGINT)((int)ch - 128), s);
		s += 7;
		Files_Read(&*R, R__typ, (void*)&ch);
	}
	n += __ASH((LONGINT)(__MASK((int)ch, -64) - __ASHL(__ASHR((int)ch, 6), 6)), s);
	*x = n;
}

void Files_WriteBool (Files_Rider *R, LONGINT *R__typ, BOOLEAN x)
{
	Files_Write(&*R, R__typ, __VAL(CHAR, x));
}

void Files_WriteInt (Files_Rider *R, LONGINT *R__typ, INTEGER x)
{
	CHAR b[2];
	b[0] = (CHAR)x;
	b[1] = (CHAR)__ASHR(x, 8);
	Files_WriteBytes(&*R, R__typ, (void*)b, 2, 2);
}

void Files_WriteLInt (Files_Rider *R, LONGINT *R__typ, LONGINT x)
{
	CHAR b[4];
	b[0] = (CHAR)x;
	b[1] = (CHAR)__ASHR(x, 8);
	b[2] = (CHAR)__ASHR(x, 16);
	b[3] = (CHAR)__ASHR(x, 24);
	Files_WriteBytes(&*R, R__typ, (void*)b, 4, 4);
}

void Files_WriteSet (Files_Rider *R, LONGINT *R__typ, SET x)
{
	CHAR b[4];
	LONGINT i;
	i = (LONGINT)x;
	b[0] = (CHAR)i;
	b[1] = (CHAR)__ASHR(i, 8);
	b[2] = (CHAR)__ASHR(i, 16);
	b[3] = (CHAR)__ASHR(i, 24);
	Files_WriteBytes(&*R, R__typ, (void*)b, 4, 4);
}

void Files_WriteReal (Files_Rider *R, LONGINT *R__typ, REAL x)
{
	CHAR b[4];
	Files_FlipBytes((void*)&x, 4, (void*)b, 4);
	Files_WriteBytes(&*R, R__typ, (void*)b, 4, 4);
}

void Files_WriteLReal (Files_Rider *R, LONGINT *R__typ, LONGREAL x)
{
	CHAR b[8];
	Files_FlipBytes((void*)&x, 8, (void*)b, 8);
	Files_WriteBytes(&*R, R__typ, (void*)b, 8, 8);
}

void Files_WriteString (Files_Rider *R, LONGINT *R__typ, CHAR *x, LONGINT x__len)
{
	INTEGER i;
	i = 0;
	while (x[__X(i, x__len)] != 0x00) {
		i += 1;
	}
	Files_WriteBytes(&*R, R__typ, (void*)x, x__len * 1, i + 1);
}

void Files_WriteNum (Files_Rider *R, LONGINT *R__typ, LONGINT x)
{
	while (x < -64 || x > 63) {
		Files_Write(&*R, R__typ, (CHAR)(__MASK(x, -128) + 128));
		x = __ASHR(x, 7);
	}
	Files_Write(&*R, R__typ, (CHAR)__MASK(x, -128));
}

static void Files_Finalize (SYSTEM_PTR o)
{
	Files_File f = NIL;
	LONGINT res;
	f = (Files_File)o;
	if (f->fd >= 0) {
		Files_fileTab[__X(f->fd, 64)] = 0;
		res = Unix_Close(f->fd);
		f->fd = -1;
		Kernel_nofiles -= 1;
		if (f->tempFile) {
			res = Unix_Unlink(f->workName, 101);
		}
	}
}

static void Files_Init (void)
{
	LONGINT i;
	i = 0;
	while (i < 64) {
		Files_fileTab[__X(i, 64)] = 0;
		i += 1;
	}
	Files_tempno = -1;
	Kernel_nofiles = 0;
}

__TDESC(Files_Handle__desc, 1, 4) = {__TDFLDS("Handle", 248), {228, 232, 236, 240, -20}};
__TDESC(Files_BufDesc__desc, 1, 1) = {__TDFLDS("BufDesc", 4112), {0, -8}};
__TDESC(Files_Rider__desc, 1, 1) = {__TDFLDS("Rider", 20), {8, -8}};
__TDESC(Files_TimeDesc__desc, 1, 0) = {__TDFLDS("TimeDesc", 40), {-4}};

export void *Files__init(void)
{
	__DEFMOD;
	__IMPORT(Args__init);
	__IMPORT(Console__init);
	__IMPORT(Kernel__init);
	__IMPORT(Unix__init);
	__REGMOD("Files", 0);
	__INITYP(Files_Handle, Files_Handle, 0);
	__INITYP(Files_BufDesc, Files_BufDesc, 0);
	__INITYP(Files_Rider, Files_Rider, 0);
	__INITYP(Files_TimeDesc, Files_TimeDesc, 0);
/* BEGIN */
	Files_Init();
	__ENDMOD;
}
