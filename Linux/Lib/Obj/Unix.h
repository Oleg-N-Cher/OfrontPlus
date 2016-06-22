/* Ofront 1.2 -xtspkael */

#ifndef Unix__h
#define Unix__h

#include "SYSTEM.h"

typedef
	struct Unix_Dirent {
		INTEGER _prvt0;
		char _prvt1[264];
	} Unix_Dirent;

typedef
	SET Unix_FdSet[8];

typedef
	struct Unix_Hostent *Unix_HostEntry;

typedef
	struct Unix_Hostent {
		LONGINT name, aliases, addrtype, length, addrlist;
	} Unix_Hostent;

typedef
	SYSTEM_PTR (*Unix_SizeT)[1];

typedef
	struct Unix_Iovec {
		LONGINT base, len;
	} Unix_Iovec;

typedef
	struct Unix_Timeval {
		LONGINT sec, usec;
	} Unix_Timeval;

typedef
	struct Unix_Itimerval {
		Unix_Timeval interval, value;
	} Unix_Itimerval;

typedef
	struct Unix_JmpBuf {
		LONGINT bx, si, di, bp, sp, pc, maskWasSaved;
		LONGINT savedMask[32];
	} Unix_JmpBuf;

typedef
	CHAR *Unix_Name;

typedef
	struct Unix_Pollfd {
		LONGINT fd;
		INTEGER events, revents;
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
		INTEGER family, port;
		LONGINT internetAddr;
		CHAR pad[8];
	} Unix_Sockaddr;

typedef
	LONGINT Unix_SocketPair[2];

typedef
	struct Unix_Status {
		INTEGER dev, devX;
		char _prvt0[4];
		Unix_SizeT ino;
		INTEGER mode, nlink, uid, gid, rdev, rdevX;
		char _prvt1[4];
		Unix_SizeT size, blksize, blocks, atime, unused1, mtime, unused2, ctime, unused3, unused4, unused5;
	} Unix_Status;

typedef
	struct Unix_Timezone {
		LONGINT minuteswest, dsttime;
	} Unix_Timezone;



import LONGINT *Unix_JmpBuf__typ;
import LONGINT *Unix_Status__typ;
import LONGINT *Unix_Timeval__typ;
import LONGINT *Unix_Timezone__typ;
import LONGINT *Unix_Itimerval__typ;
import LONGINT *Unix_SigContext__typ;
import LONGINT *Unix_Dirent__typ;
import LONGINT *Unix_Rusage__typ;
import LONGINT *Unix_Iovec__typ;
import LONGINT *Unix_Pollfd__typ;
import LONGINT *Unix_Sockaddr__typ;
import LONGINT *Unix_Hostent__typ;

import LONGINT Unix_Fstat (LONGINT fd, Unix_Status *statbuf, LONGINT *statbuf__typ);
import LONGINT Unix_Stat (CHAR *name, LONGINT name__len, Unix_Status *statbuf, LONGINT *statbuf__typ);
import LONGINT Unix_errno (void);
import void *Unix__init(void);

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
#define Unix_Fsync(fd)	fsync(fd)
#define Unix_Ftruncate(fd, length)	ftruncate(fd, length)
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

#endif
