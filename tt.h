/***************************************************************************\
|*									   *|
|*  tt.h:	A version of Tetris to run on ordinary terminals,	   *|
|*		(ie., not needing a workstation, so should available	   *|
|*		to peasant Newwords+ users.				   *|
|*									   *|
|*  Author:	Mike Taylor (mirk@uk.co.ssl)				   *|
|*  Started:	Fri May 26 12:26:05 BST 1989				   *|
|*									   *|
\***************************************************************************/
/***************************************************************************\
|*									   *|
|*  The following macro, if defined, causes the exclusive high-score	   *|
|*  file-locking to be done using lockf(3) instead of the O_EXCL flag	   *|
|*  on open(2).	 This means that no temporary file need be created, and	   *|
|*  thus, that no write-permission is needed on a lockfile-directory.	   *|
|*  This should probably be used in preference to open(O_EXCL) when it	   *|
|*  exists and is bugless.  WARNING: on some machines, buggy versions	   *|
|*  of lockf(3) can crash the machine, (eg. Sun3/4s running SunOS 4.0.1)   *|
|*									   *|
\***************************************************************************/

#undef LOCKF			/* Use lockf(3) instead of open(O_EXCL) */

/***************************************************************************\
|*									   *|
|*  File in which high-scores will be stored.  I'm not gonna muck	   *|
|*  about encrypting them or anything, so you can either make tt	   *|
|*  setuid, or just run the risk of stupid people editing the high-	   *|
|*  score table by hand.						   *|
|*									   *|
\***************************************************************************/

extern char *LOCK_FILE;
extern char *SCORE_FILE;

/*-------------------------------------------------------------------------*/

#define NO_HISCORES	100	/* Size of high-score table */
#define NO_SHOWN	10	/* Default number to print */
#define LINELEN		160	/* Maximum length of a line of text */
#define NAMELEN		20	/* Multiple of four for struct-padding */
#define CODELEN		12	/* Multiple of four for struct-padding */

#define MAX_GAME_WIDTH	15	/* Max. supported value of GAME_WIDTH */
#define GAME_DEPTH	20	/* Number of squares down board */
#define STAT_WIDTH	38	/* Number of characters for messages */
#define NO_ORIENTS	4	/* Number of orientations possible */

#define PI_EMPTY	-1	/* Position of board is empty */

/***************************************************************************\
|*									   *|
|*  The following #defines, (the LE_* ones) are local errors, in the	   *|
|*  sense of being local to this program, as distinct from the system	   *|
|*  errors, (ENOENT, EINTR and friends).  They should be used as exit	   *|
|*  statuses, as the first argument to die(), defined in utils.c	   *|
|*									   *|
\***************************************************************************/

#define LE_OK		0	/* No problems, me ole' fruit-bat */
#define LE_USAGE	1	/* Command-line syntax was wrong */
#define LE_ENV		2	/* Couldn't get environment variable */
#define LE_TERMCAP	3	/* Couldn't get a termcap entry */
#define LE_SCREEN	4	/* Screen was too small for game */
#define LE_SIGNAL	5	/* Signal(3) call failed */
#define LE_SELECT	6	/* Select(2) call failed */
#define LE_READ		7	/* Read(2) call failed */
#define LE_MALLOC	8	/* Couldn't allocate memory */
#define LE_LEVEL	9	/* User wanted to play on a silly level */
#define LE_OPEN		10	/* Open(2) call failed */
#define LE_STAT		11	/* {f,l,}stat(2) call failed */
#define LE_WRITE	12	/* write(2) call failed */
#define LE_GETUID	13	/* getuid(2) call failed */
#define LE_GETPW	14	/* getpwuid(3) call failed */
#define LE_MODE		15	/* User wanted to play in a silly mode */

/***************************************************************************\
|*									   *|
|*  This is the structure of which the high-score table is composed.	   *|
|*  I'm gonna make everything a muliple of four bytes in an attempt	   *|
|*  to maintain file-compatibility between sun3s and 4s (and maybe	   *|
|*  other things, who knows?)  This is also why all the integers are	   *|
|*  long -- this helps to ensure that they will be four bytes long on	   *|
|*  most architectures, (ie. including some that have 2-byte integers)	   *|
|*  thus making struct-compatibility more likely.			   *|
|*									   *|
\***************************************************************************/

struct score_ent {
  char name[NAMELEN];		/* Name of player (from environment) */
  char code[CODELEN];		/* Code of player (from getpwuid()) */
  long int score;		/* Total score (5 for an "S" etc.) */
  long int no_pieces;		/* Number of pieces dropped in */
  long int no_levels;		/* Number of levels completed */
  long int game_level;		/* Number of free pieces at start */
  long int game_mode;		/* if they used all the pieces */
};

/*-------------------------------------------------------------------------*/

extern int NO_PIECES;		/* Number of different pieces */
extern int NO_SQUARES;		/* Max. number of squares per piece */
extern int GAME_WIDTH;		/* Number of squares across board */
extern int GHOST_PIECES;	/* Whether to show ghost pieces */
extern int SHOW_NEXT;		/* Whether to show the next piece */
extern int screen_depth;	/* To be calculated by termcap(3) */
extern int screen_width;	/* To be calculated by termcap(3) */
extern int so_gunk;		/* To be calculated by termcap(3) */
extern int in_curses;		/* Set to 1 after initialisation */
extern int rot_backwards;	/* If set non-zero, rotate clockwise */
extern int no_hiscores;		/* Number of hi-scores in the table */
extern int game_level;		/* Number of free pieces */
extern int game_mode;		/* If non-zero, don't use all pieces */
extern int score;		/* Accumulated game score */
extern int no_pieces;		/* Number of pieces dropped so far */
extern int no_levels;		/* Number of levels filled & deleted */
extern char prog_name[LINELEN]; /* Will be the basename of argv[0] */
extern char user_name[NAMELEN]; /* From environment: TTNAME or NAME */
extern char user_code[CODELEN]; /* From getpwuid(getuid())->pw_name */
extern int board[GAME_DEPTH+4][MAX_GAME_WIDTH];
extern struct score_ent hi_scores[NO_HISCORES];

extern char tc_string[LINELEN]; /* Static, filled by termcap(3) */
extern char *so_str;		/* Points at things found by termcap(3) */
extern char *se_str;		/* Points at things found by termcap(3) */

/*-------------------------------------------------------------------------*/

extern void signal_die ();	/* When CTRL-C etc. is pressed */

/****************************************************************************\
|*
|*	Portability stuff, added Thu Aug  2 19:00:53 BST 1990
|*	This should allow "tt" to run on System V and early BSDs.
|*
|*	I've also changed the function update_file() to new_highscore(),
|*	and the variable rotate_backwards to rot_backwards, in order to
|*	avoid name-clashes with update_scores() and rotate_key when using
|*	eight-significant-character-including-prepended-underscore
|*	compilers ... sigh ...
|*
|*	More stuff added Wed Apr  3 12:36:05 BST 1991
|*	Included grot like using "tmout" sinstead of "timeout" because some
|*	SysV curses packages #define this identifier!  :-P
|*
|*	Update: Thu May 16 15:18:29 PDT 1991 (harveyt@santa-cruz.co.uk)
|*	Added various gubbins for *real* System V support, notably emulate
|*	BSD's select with System V's poll().  Also support Sun specific
|*	usleep() (which I've noticed isn't a BSD standard syscall), ostensibly
|*	by using nap(S).  Seems to work on SCO Unix System V/386 3.2 anyway.
|*
\****************************************************************************/

#ifndef BADSIG			/* Return value of failed signal() call */
#ifdef SYSV
#define BADSIG (void (*)()) -1
#else
#define BADSIG (int (*)()) -1
#endif
#endif /*BADSIG*/		/* system call returns this. */

#ifndef FD_SET			/* Eg., Berkeley 4.2bsd */
#define FD_SETSIZE	32	/* Must be atleast this amount */
typedef long fd_set;
#define FD_SET(fd,fsp) (*fsp) |= (1 << (fd))
#define FD_CLR(fd,fsp) (*fsp) &= ~ (1 << (fd))
#define FD_ISSET(fd,fsp) ((*fsp) & (1 << (fd)) != 0)
#define FD_ZERO(fsp) (*fsp) = 0
#endif /*FD_SET*/

#ifdef SYSV
	/* Various inconsistencies between SYSV and others...  */
#define bcopy(m1,m2,n) memmove ((m2),(m1),(n))
#define bzero(m,l) memset ((m),0,(l))
#define random rand
#define srandom srand
#define cbreak crmode
#define nocbreak nocrmode
	/* 
	 * Here we'll use our own version of select - even if the system
	 * provides one (under SCO Unix it does, however the granularity
	 * of the timeout is seemingly random or broken). poll() seems
	 * to do the trick very neatly (if oddly).
	 */ 
#define select sysv_select
#endif /*SYSV*/

/*--------------------------------------------------------------------------*/

/* This is probably still the most portable way ... */
extern int read(), close(), sleep(), unlink(), write(), getuid();
extern void usleep();
