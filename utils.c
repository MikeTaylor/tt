/***************************************************************************\
|*									   *|
|*  utils.c:	A version of Tetris to run on ordinary terminals,	   *|
|*		(ie., not needing a workstation, so should available	   *|
|*		to peasant Newwords+ users.  This module supplies all	   *|
|*		the function that I will need, but which are not	   *|
|*		intrinsically a part of Tetris itself.			   *|
|*									   *|
|*  Author:	Mike Taylor (mirk@uk.co.ssl)				   *|
|*  Started:	Fri May 26 12:26:05 BST 1989				   *|
|*									   *|
|*  Modifications:							   *|
|*									   *|
|*  7th Feb 1991	harveyt@scol.UUCP	M001			   *|
|*	- Checked SYSV version for SCO Unix, flush_keyboard can be	   *|
|*	  simplified to an ioctl(), and checked usleep() worked.	   *|
|*	  It didn't - no clear solution yet.				   *|
|*  1st May 1991	harveyt@scol.UUCP	M002			   *|
|*	- Rewrote usleep() to use nap()					   *|
|*	- Added sysv_select() which emulates BSD select	using poll()	   *|
|*									   *|
\***************************************************************************/

#include <string.h>
#include <errno.h>
#ifdef SYSV
#  undef SYSV	/* SysV curses actually defines SYSV */
		/* We'll use poll() instead of select */
#  include <stropts.h>
#  include <poll.h>
#endif
#include <curses.h>
#include <term.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>						/* M001 */

#include "tt.h"
#include "utils.h"
#include "screen.h"

/*-------------------------------------------------------------------------*/

extern char *getenv ();

/***************************************************************************\
|*									   *|
|*  The function basename() acts like the UNIX(tm) command of the same	   *|
|*  name.  It takes as its argument a string, and returns the last path-   *|
|*  component of that string, (ie, the section after the last '/'), or	   *|
|*  the whole string if it has no '/'.					   *|
|*									   *|
\***************************************************************************/

char *basename (name)
  char *name;
{
  char *slash;
  if ((slash = strrchr (name, '/')) != 0)
    return (slash+1);
  else
    return (name);
}

/***************************************************************************\
|*									   *|
|*  This is my home-made varargs-based version of C++'s neat function of   *|
|*  the same name.  It saves all that tedious mucking about with static	   *|
|*  buffers which you sprintf into, then forget all about, by basically	   *|
|*  being a sprintf with its own static storage.  It takes as arguments	   *|
|*  exactly the same things as printf(3), and returns a pointer to the	   *|
|*  resultant string.							   *|
|*									   *|
\***************************************************************************/

/*VARARGS*/
char *form (char *fmt, ...)
{
  va_list ap;
  static char result[LINELEN];
  
  va_start (ap, fmt);
  (void) vsprintf (result, fmt, ap);
  va_end (ap);
  return (result);
}

/***************************************************************************\
|*									   *|
|*  This is yer bog-standard "print a message and quit" function, except   *|
|*  that it checks to see if we are in curses(3x), and if so, takes us	   *|
|*  out before doing its stuff.	 The arguments are an integer, the exit	   *|
|*  status, and a string containing an error report, to which will be	   *|
|*  prepended the program name when it is printed.  If the status code	   *|
|*  is LE_OK, (ie. nothing went wrong), no message is printed.		   *|
|*									   *|
\***************************************************************************/

void die (status, line)
  int status;
  char *line;
{
  if (in_curses) {
    myrefresh ();
#ifndef lint
    nocrmode ();
    echo ();
#endif /*lint*/
    endwin ();
  }

  if (status != LE_OK)
    (void) printf ("%s: %s\n", prog_name, line);

#ifndef LOCKF
  (void) unlink (LOCK_FILE);	/* Just in case :-) */
#endif /* LOCKF */
  exit (status);
}

/***************************************************************************\
|*									   *|
|*  The function get_termcap extracts the information we need from the	   *|
|*  UNIX terminal capability database.	It has been (correctly) pointed	   *|
|*  out that most of what I do in this function is already done for me	   *|
|*  by curses(3x), but (A) curses behaves stupidly on terminals with	   *|
|*  termcap's "sg" non-zero, (ie. refuses to enter standout mode, so	   *|
|*  you *have* to do it manually), and (B) I wanted to prove a point to	   *|
|*  Sunny and others, who said it was impossible.  Tgets() sucks :-P	   *|
|*									   *|
\***************************************************************************/

void get_termcap ()
{
  static char bp[1024];		/* Seems odd to hardwire the 1024, but ... */
  char *term_name;		/* ... the manual entry told me to do it! */
  int status;			/* Success/failure of tgetent here */
  char *tmp_ptr = tc_string;	/* Pointer to be advanced by tgetstr() */

  if ((term_name = getenv ("TERM")) == NULL)
    die (LE_ENV, "couldn't get TERM from environment");

  if ((status = tgetent (bp, term_name)) == -1)
    die (LE_TERMCAP, "couldn't open TERMCAP file");

  if (status == 0)
    die (LE_TERMCAP, "couldn't find your terminal in TERMCAP file");

  if ((screen_depth = tgetnum ("li")) == -1)
    die (LE_TERMCAP, "couldn't get screen-depth from termcap");

  if ((screen_width = tgetnum ("co")) == -1)
    die (LE_TERMCAP, "couldn't get screen-width from termcap");

  if ((so_gunk = tgetnum ("sg")) == -1)
    so_gunk = 0;		/* Default value */

  so_str = tmp_ptr;
  if (tgetstr ("so", &tmp_ptr) == NULL)
    so_str = "";

  se_str = tmp_ptr;
  if (tgetstr ("se", &tmp_ptr) == NULL)
    se_str = "";
}

/*-------------------------------------------------------------------------*/
/* BEGIN M002 */

/***************************************************************************\
|*									   *|
|*  The function flush_keybord() checks to see whether there are any	   *|
|*  keystrokes waiting to be read, and if so reads them and throws	   *|
|*  them away.	Surprising how complex this needs to be, but the only	   *|
|*  alternative I can think of is to ioctl() stdin to be non-blocking,	   *|
|*  read() until it returns 0, then ioctl() it back, which is scarcely	   *|
|*  an improvement.							   *|
|*									   *|
|*  Update: Thu May 16 15:22:41 PDT 1991 (harveyt@santa-cruz.co.uk)	   *|
|*  Rewritten as a one line ioctl() call - which is much more portable,	   *|
|*  we hope.								   *|
|*									   *|
\***************************************************************************/

void flush_keyboard ()
{
#ifdef SYSV
  /* M001 - We can do a luverly ioctl() for this */
  (void) ioctl (fileno(stdin), TCFLSH, 0);
#else
  /* We have to do it the hard way on SunOS 4.1.1 -- and all BSD's? */
  fd_set fds;			/* Will contain only stdin */
  struct timeval tmout;		/* Will contain a zero timeout (poll) */
  static char buffer[LINELEN];	/* Will be used as the buffer to read(2) */
  int status;			/* Contains select(2)'s return value */

  while (1) {
    FD_ZERO (&fds);
    FD_SET (0, &fds);
    tmout.tv_sec = 0L;
    tmout.tv_usec = 0L;
    if (((status = select (1, &fds, (fd_set*) NULL, (fd_set*) NULL,
			   &tmout)) == -1) && (errno != EINTR))
      die (LE_SELECT, "select(2) failed in flush_keybord()");
    if ((status == 0) || (status == -1))
      break;			/* Can only be -1 if errno == EINTR */
    (void) read (0, buffer, LINELEN);
  }
#endif /*SYSV*/
}

/*-------------------------------------------------------------------------*/

#ifndef SYSV /* implies BSD... */
#  ifdef NO_USLEEP_SYSCALL

/*
 * Use this if your on BSD and don't have usleep() - mostly for
 * non Sun's.
 */
void usleep (us)
unsigned us;
{
	struct timeval tmout;

	tmout.tv_sec = us / 1000000;
	tmout.tv_usec = us % 1000000;
	(void) select (0, (fd_set *)0, (fd_set *)0, (fd_set *)0, tmout);
}

#  endif /* NO_USLEEP_SYSCALL */
#else /* SYSV */

/*
 * If you don't have nap - you might try coding this using
 * poll().  I'll put tentaive code in,
 * although it didn't seem to work too well for me.
 * Another solution would be to use curses napms() call,
 * although I haven't bothered trying this one.
 */
void usleep (us)
unsigned us;
{
#  ifndef NO_NAP_SYSCALL
	(void) nap (us / 1000);
#  else	
	(void) poll ((struct pollfd *)0, 0, us/1000);
#  endif /* NO_NAP_SYSCALL */
}

/*
 * Assumptions:  Only ever select with nfds == 0, or rfd == 0, or
 *		 rfd on file descriptor 0 (stdin).
 *		 Timeout NULL implies blocking select.
 */
int sysv_select (nfds, rfd, wfd, xfd, tmout)
int nfds;
fd_set *rfd, *wfd, *xfd;
struct timeval *tmout;
{
	struct pollfd pollrfd;
	int ret, time;
	unsigned long pollnfds = ((nfds == 0 || rfd == 0) ? 0L : 1L);
	
	if (tmout)
		time = (tmout->tv_sec * 1000) + (tmout->tv_usec / 1000);
	else
		time = -1;
		
	pollrfd.fd = 0;
	pollrfd.events = POLLIN;
	pollrfd.revents = 0;

	ret = poll (&pollrfd, pollnfds, time);

	if (pollrfd.revents & POLLIN)
		FD_SET (0,rfd);
	return (ret);
}
#endif /* SYSV */

/* END M002 */
/*--------------------------------------------------------------------------*/
