Ofront for Linux
================

It's Ofront for Linux modified by VEDAsoft Oberon Club.
In our work we need some features, and have added easily.

The project lives at:

  https://github.com/Oleg-N-Cher/Ofront-for-Linux

Support forum (English and Russian languages are welcome):

  http://zx.oberon2.ru/forum/viewforum.php?f=32


Thanks to Dr. Josef Templ for this great product!


Differences from the original ver. from Josef Templ, Software Templ OG
======================================================================

- Added supporting character '_' in identifiers. Use this feature with caution,
  and only when absolutely necessary (for WinApi constants, etc).
  Remember, this character is not present in the standards of Oberon/Oberon-2.

  
- Using identifiers concatenation '##' has been removed - old C compilers
  (like free Turbo C v2.01) does not support it.
  It needs to rewrite some macros in SYSTEM.h:

  Was:
    #define __IMPORT(name)	SYSTEM_INCREF(name##__init())

  Now:
    #define __IMPORT(name__init)	SYSTEM_INCREF(name__init())

  Was:
    #define __TDESC(t, m, n) \
    	static struct t##__desc {\
    		long tproc[m]; \
    		long tag, next, level, module; \
    		char name[24]; \
    		long *base[__MAXEXT]; \
    		char *rsrvd; \
    		long blksz, ptr[n+1]; \
    	} t##__desc

  Now:
      #define __TDESC(t__desc, m, n) \
    	static struct t__desc {\
    		long tproc[m]; \
    		long tag, next, level, module; \
    		char name[24]; \
    		long *base[__MAXEXT]; \
    		char *rsrvd; \
    		long blksz, ptr[n+1]; \
    	} t__desc

  WARNING: Work for removing all '##' identifiers concatenations is not done
     completely: macros __NEW(p, t), __DUPARR(v, t), __IS(tag, typ, level)
  __GUARDR(r, typ, level), __GUARDEQR(p, dyntyp, typ), __GUARDEQP(p, typ),
  __INITYP(t, t0, level), __INITBP(t, proc, num) still need to be modified.


- Add possibility of static linking to typebound procedures (a procedure name
  as additional parameter in macro __SEND (in 'SYSTEM.h')).


- Allow the SHORTINT type to has the size of 2 bytes (set this in Ofront.par).


Vers. from VEDAsoft uses the same distro name with suffix '+':

  ofront-1.2-linux2+.tar.gz
  ofront+.tar.gz


--
Oleg N. Cher
VEDAsoft Oberon Club
http://zx.oberon2.ru
