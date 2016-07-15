/*  Ofront 1.2 -xtspkae */
#include "SYSTEM.h"

typedef
	CHAR (*Unix_ADR)[1];

typedef
	struct Unix_Dirent {
		LONGINT ino, off;
		INTEGER reclen;
		CHAR name[256];
	} Unix_Dirent;

typedef
	SET Unix_FdSet[8];

typedef
	struct Unix_Hostent *Unix_HostEntry;

typedef
	struct Unix_Hostent {
		Unix_ADR name, aliases;
		SHORTINT addrtype, length;
		Unix_ADR addrlist;
	} Unix_Hostent;

typedef
	struct Unix_Iovec {
		LONGINT base, len;
	} Unix_Iovec;

typedef
	struct Unix_Timeval {
		INTEGER sec, usec;
	} Unix_Timeval;

typedef
	struct Unix_Itimerval {
		Unix_Timeval interval, value;
	} Unix_Itimerval;

typedef
	struct Unix_JmpBuf {
		INTEGER bx, si, di, bp, sp, pc, maskWasSaved;
		INTEGER savedMask[32];
	} Unix_JmpBuf;

typedef
	CHAR *Unix_Name;

typedef
	CHAR (*Unix_SizeT)[1];

typedef
	struct Unix_Pollfd {
		Unix_SizeT fd;
		SHORTINT events, revents;
	} Unix_Pollfd;

typedef
	struct Unix_Rusage {
		Unix_Timeval utime, stime;
		LONGINT maxrss, ixrss, idrss, isrss, minflt, majflt, nswap, inblock, oublock, msgsnd, msgrcv, nsignals, nvcsw, nivcsw;
	} Unix_Rusage;

typedef
	struct Unix_SigContext {
		char _prvt0[1];
	} Unix_SigContext;

typedef
	Unix_SigContext *Unix_SigCtxPtr;

typedef
	void (*Unix_SignalHandler)(LONGINT, LONGINT, Unix_SigCtxPtr);

typedef
	struct Unix_Sockaddr {
		SHORTINT family, port;
		INTEGER internetAddr;
		BYTE pad[8];
	} Unix_Sockaddr;

typedef
	LONGINT Unix_SocketPair[2];

typedef
	struct Unix_Status {
		INTEGER dev;
		SHORTINT ino, mode, nlink, uid, gid;
		INTEGER rdev, size;
		Unix_SizeT atime, mtime, ctime;
	} Unix_Status;

typedef
	struct Unix_Timezone {
		INTEGER minuteswest, dsttime;
	} Unix_Timezone;



export LONGINT *Unix_JmpBuf__typ;
export LONGINT *Unix_Status__typ;
export LONGINT *Unix_Timeval__typ;
export LONGINT *Unix_Timezone__typ;
export LONGINT *Unix_Itimerval__typ;
export LONGINT *Unix_SigContext__typ;
export LONGINT *Unix_Dirent__typ;
export LONGINT *Unix_Rusage__typ;
export LONGINT *Unix_Iovec__typ;
export LONGINT *Unix_Pollfd__typ;
export LONGINT *Unix_Sockaddr__typ;
export LONGINT *Unix_Hostent__typ;

export LONGINT Unix_Fstat (LONGINT fd, Unix_Status *statbuf, LONGINT *statbuf__typ);
export LONGINT Unix_Stat (CHAR *name, LONGINT name__len, Unix_Status *statbuf, LONGINT *statbuf__typ);
export LONGINT Unix_errno (void);

#define Unix_Accept(socket, addr, addr__typ, addrlen)	accept(socket, addr, addrlen)
#define Unix_Bind(socket, name, namelen)	bind(socket, &(name), namelen)
#define Unix_Chdir(path, path__len)	chdir(path)
#define Unix_Chmod(path, path__len, mode)	chmod(path, mode)
#define Unix_Close(fd)	close(fd)
#define Unix_Connect(socket, name, namelen)	connect(socket, &(name), namelen)
#define Unix_Dup(fd)	dup(fd)
#define Unix_Dup2(fd1, fd2)	dup(fd1, fd2)
#define Unix_Exit(n)	exit(n)
#define Unix_Fchmod(fd, mode)	fchmod(fd, mode)
#define Unix_Fcntl(fd, cmd, arg)	fcntl(fd, cmd, arg)
#define Unix_Flock(fd, operation)	flock(fd, operation)
#define Unix_Fork()	fork()
#define Unix_Fsync(fd)	_commit(fd)
#define Unix_Ftruncate(fd, length)	_chsize(fd, length)
#define Unix_Getegid()	getegid()
#define Unix_Geteuid()	geteuid()
#define Unix_Getgid()	getgid()
#define Unix_Gethostbyname(name, name__len)	(Unix_HostEntry)gethostbyname(name)
#define Unix_Gethostname(name, name__len)	gethostname(name, name__len)
#define Unix_Getpid()	getpid()
#define Unix_Getsockname(socket, name, name__typ, namelen)	getsockname(socket, name, namelen)
#define Unix_Gettimeofday(tv, tv__typ, tz, tz__typ)	gettimeofday(tv, tz)
#define Unix_Getuid()	getuid()
#define Unix_Ioctl(fd, request, arg)	ioctl(fd, request, arg)
#define Unix_Kill(pid, sig)	kill(pid, sig)
#define Unix_Listen(socket, backlog)	listen(socket, backlog)
#define Unix_Lseek(fd, offset, origin)	lseek(fd, offset, origin)
#define Unix_Open(name, name__len, flag, mode)	open(name, flag, mode)
#define Unix_Read(fd, buf, nbyte)	read(fd, buf, nbyte)
#define Unix_ReadBlk(fd, buf, buf__len)	read(fd, buf, buf__len)
#define Unix_Readblk(fd, buf, buf__len, len)	read(fd, buf, len)
#define Unix_Recv(socket, bufadr, buflen, flags)	recv(socket, bufadr, buflen, flags)
#define Unix_Rename(old, old__len, new, new__len)	rename(old, new)
#define Unix_Select(width, readfds, writefds, exceptfds, timeout, timeout__typ)	select(width, readfds, writefds, exceptfds, timeout)
#define Unix_Send(socket, bufadr, buflen, flags)	send(socket, bufadr, buflen, flags)
#define Unix_Sigsetmask(mask)	sigsetmask(mask)
#define Unix_Socket(af, type, protocol)	socket(af, type, protocol)
#define Unix_Unlink(name, name__len)	unlink(name)
#define Unix_Wait(status)	wait(status)
#define Unix_Write(fd, buf, nbyte)	write(fd, buf, nbyte)
#define Unix_WriteBlk(fd, buf, buf__len)	write(fd, buf, buf__len)
#define Unix_err()	errno
#define Unix_fstat(fd, statbuf, statbuf__typ)	fstat(fd, (struct stat*)statbuf)
#include <errno.h>
#include <sys/stat.h>
#define Unix_stat(name, name__len, statbuf, statbuf__typ)	stat((const char*)name, (struct stat*)statbuf)

/*============================================================================*/

LONGINT Unix_errno (void)
{
	return Unix_err();
}

/*----------------------------------------------------------------------------*/
LONGINT Unix_Stat (CHAR *name, LONGINT name__len, Unix_Status *statbuf, LONGINT *statbuf__typ)
{
	LONGINT res;
	__DUP(name, name__len, CHAR);
	res = Unix_stat(name, name__len, &*statbuf, statbuf__typ);
	__DEL(name);
	return res;
}

/*----------------------------------------------------------------------------*/
LONGINT Unix_Fstat (LONGINT fd, Unix_Status *statbuf, LONGINT *statbuf__typ)
{
	LONGINT res;
	res = Unix_fstat(fd, &*statbuf, statbuf__typ);
	return res;
}

/*----------------------------------------------------------------------------*/
__TDESC(Unix_JmpBuf__desc, 1, 0) = {__TDFLDS("JmpBuf", 156), {-4}};
__TDESC(Unix_Status__desc, 1, 0) = {__TDFLDS("Status", 36), {-4}};
__TDESC(Unix_Timeval__desc, 1, 0) = {__TDFLDS("Timeval", 8), {-4}};
__TDESC(Unix_Timezone__desc, 1, 0) = {__TDFLDS("Timezone", 8), {-4}};
__TDESC(Unix_Itimerval__desc, 1, 0) = {__TDFLDS("Itimerval", 16), {-4}};
__TDESC(Unix_SigContext__desc, 1, 0) = {__TDFLDS("SigContext", 1), {-4}};
__TDESC(Unix_Dirent__desc, 1, 0) = {__TDFLDS("Dirent", 268), {-4}};
__TDESC(Unix_Rusage__desc, 1, 0) = {__TDFLDS("Rusage", 72), {-4}};
__TDESC(Unix_Iovec__desc, 1, 0) = {__TDFLDS("Iovec", 8), {-4}};
__TDESC(Unix_Pollfd__desc, 1, 0) = {__TDFLDS("Pollfd", 8), {-4}};
__TDESC(Unix_Sockaddr__desc, 1, 0) = {__TDFLDS("Sockaddr", 16), {-4}};
__TDESC(Unix_Hostent__desc, 1, 0) = {__TDFLDS("Hostent", 16), {-4}};

export void *Unix__init(void)
{
	__DEFMOD;
	__REGMOD("Unix", 0);
	__INITYP(Unix_JmpBuf, Unix_JmpBuf, 0);
	__INITYP(Unix_Status, Unix_Status, 0);
	__INITYP(Unix_Timeval, Unix_Timeval, 0);
	__INITYP(Unix_Timezone, Unix_Timezone, 0);
	__INITYP(Unix_Itimerval, Unix_Itimerval, 0);
	__INITYP(Unix_SigContext, Unix_SigContext, 0);
	__INITYP(Unix_Dirent, Unix_Dirent, 0);
	__INITYP(Unix_Rusage, Unix_Rusage, 0);
	__INITYP(Unix_Iovec, Unix_Iovec, 0);
	__INITYP(Unix_Pollfd, Unix_Pollfd, 0);
	__INITYP(Unix_Sockaddr, Unix_Sockaddr, 0);
	__INITYP(Unix_Hostent, Unix_Hostent, 0);
/* BEGIN */
	__ENDMOD;
}
