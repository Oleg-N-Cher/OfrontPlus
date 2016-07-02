MODULE TestUnix;

IMPORT SYSTEM, Unix, Write := Console;

PROCEDURE -AAincludeSysStat "#include <sys/stat.h>";
PROCEDURE -statSIZE(): INTEGER "sizeof(struct stat)";

PROCEDURE -AAincludeSysTime "#include <sys/time.h>";
PROCEDURE -timevalSIZE(): INTEGER "sizeof(struct timeval)";
PROCEDURE -timezoneSIZE(): INTEGER "sizeof(struct timezone)";

PROCEDURE -AAincludeWinsock '#include "_winsock.h"';
PROCEDURE -fdsetSIZE(): INTEGER "sizeof(struct fd_set)";

BEGIN

  (* JmpBuf *)

  (* Status *)
  IF SIZE(Unix.Status) # statSIZE() THEN
    Write.String("SIZE(Unix.Status) = "); Write.Int(SIZE(Unix.Status), 0);
    Write.String("; sizeof(struct stat) = "); Write.Int(statSIZE(), 0);
    Write.Ln
  END;

  (* Timeval *)
  IF SIZE(Unix.Timeval) # timevalSIZE() THEN
    Write.String("SIZE(Unix.Timeval) = "); Write.Int(SIZE(Unix.Timeval), 0);
    Write.String("; sizeof(struct timeval) = "); Write.Int(timevalSIZE(), 0);
    Write.Ln
  END;

  (* Timezone *)
  IF SIZE(Unix.Timezone) # timezoneSIZE() THEN
    Write.String("SIZE(Unix.Timezone) = "); Write.Int(SIZE(Unix.Timezone), 0);
    Write.String("; sizeof(struct timezone) = "); Write.Int(timezoneSIZE(), 0);
    Write.Ln
  END;

  (* FdSet *)
  IF SIZE(Unix.FdSet) # fdsetSIZE() THEN
    Write.String("SIZE(Unix.FdSet) = "); Write.Int(SIZE(Unix.FdSet), 0);
    Write.String("; sizeof(struct fd_set) = "); Write.Int(fdsetSIZE(), 0);
    Write.Ln
  END;

  Write.Flush
END TestUnix.
