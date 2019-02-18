/***************************************************************************\
|*+-----------------------------------------------------------------------+*|
|*|									  |*|
|*|  tt.c:	A version of Tetris to run on ordinary terminals,	  |*|
|*|		(ie., not needing a workstation, so should available	  |*|
|*|		to peasant Newwords+ users.				  |*|
|*|									  |*|
|*|  Author:	Mike Taylor (mirk@uk.co.ssl)				  |*|
|*|  Started:	Fri May 26 12:26:05 BST 1989				  |*|
|*|									  |*|
|*|		Oooh look, I've just invented a new, chunkier kind of	  |*|
|*|		comment-box.  I can't decide yet whether I like it.	  |*|
|*|									  |*|
|*+-----------------------------------------------------------------------+*|
\***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "tt.h"
#include "utils.h"
#include "screen.h"
#include "game.h"
#include "pieces.h"

/*-------------------------------------------------------------------------*/

extern time_t time ();
extern char *ctime ();
/*extern char *malloc ();*/
extern char *getenv ();
extern char *getlogin ();
extern struct passwd *getpwuid ();

static int get_key ();

/*-------------------------------------------------------------------------*/

char *LOCK_FILE = "/home/mike/lib/ttlock/ttlock";
char *SCORE_FILE = "/home/mike/lib/ttscores";

/* These four variables determine which game we're playing */
struct piece *pieces = pieces4;
int NO_PIECES = 7;		/* Value reset for t3, pentris, etc. */
int NO_SQUARES = 4;		/* Value reset for t3, pentris, etc. */
int GAME_WIDTH = 10;		/* Value reset for t3, pentris, etc. */
int GHOST_PIECES = 0;		/* Ghost pieces are not shown by default */
int SHOW_NEXT = 0;		/* Don't show the next piece by default */

int screen_depth;		/* To be calculated by termcap(3) */
int screen_width;		/* To be calculated by termcap(3) */
int so_gunk;			/* To be calculated by termcap(3) */
int in_curses = 0;		/* Set to 1 after initialisation */
int rot_backwards = 0;		/* If set non-zero, rotate clockwise */
int no_hiscores = 0;		/* Number of hi-scores in the table */
int no_shown = NO_SHOWN;	/* Number of hi-scores to list */
int game_level = 0;		/* Number of free pieces */
int game_mode = 0;		/* If non-zero, don't use all pieces */
int score;			/* Accumulated game score */
int no_pieces;			/* Number of pieces dropped so far */
int no_levels;			/* Number of levels filled & deleted */
char prog_name[LINELEN];	/* Will be the basename of argv[0] */
char user_name[NAMELEN];	/* From environment: TTNAME or NAME */
char user_code[CODELEN];	/* From getpwuid(getuid())->pw_name */
int board[GAME_DEPTH+4][MAX_GAME_WIDTH];
struct score_ent hi_scores[NO_HISCORES];

char tc_string[LINELEN];	/* Needed as static storage for the awful */
char *so_str;			/* ... tgetstr() function.  so_str and ... */
char *se_str;			/* ... se_str point into it. */
  
/***************************************************************************\
|*									   *|
|*  This function is called if a SIGHUP, SIGINT or SIGTERM is caught,	   *|
|*  and merely returns the user to standard terminal modes, (ie. exits	   *|
|*  from curses(3X) before die()ing.					   *|
|*									   *|
\***************************************************************************/

void signal_end (sig)
  int sig;
{
  if (in_curses)
    print_msg ("Aborted!");
  
  die (LE_OK, "");
}

/***************************************************************************\
|*									   *|
|*  The function get_scores() reads the contents of the global array	   *|
|*  hi_scores from the file named in the #definition of SCORE_FILE.	   *|
|*  It also sets no_hiscores to the number of scores in the table.	   *|
|*									   *|
\***************************************************************************/

void get_scores ()
{
  int fd;
  struct stat stat_buf;

  if ((fd = open (SCORE_FILE, O_RDONLY)) == -1) {
    if (errno != ENOENT)
      die (LE_OPEN, "couldn't open(2) high-score file for reading");
    else {
      no_hiscores = 0;
      return;
    }
  }

  if (fstat (fd, &stat_buf) == -1)
    die (LE_STAT, "couldn't stat(2) high-score file");
  no_hiscores = stat_buf.st_size/sizeof (struct score_ent);

  if (read (fd, (char*) hi_scores, (int) stat_buf.st_size) == -1) {
    (void) perror ("read()");
      die (LE_READ, "couldn't read(2) high-score file");
  }

  (void) close (fd);
}

/***************************************************************************\
|*									   *|
|*  The function print_scores() gets the table in from the disk_file,	   *|
|*  and prints it in a nice, human-readable format.			   *|
|*									   *|
\***************************************************************************/

void print_scores ()
{
  int i;
  
  get_scores ();
  if (no_hiscores == 0)
    (void) puts ("There are no high-scores (yet!)");
  else {
    (void) puts ("+------+---------------------+---------+-------+--------+--------+-----------+");
    (void) puts ("| Rank | Name                | Code    | Score | Pieces | Levels | Type/Mode |");
    (void) puts ("+------+---------------------+---------+-------+--------+--------+-----------+");
    for (i = 0; i < ((no_hiscores < no_shown) ? no_hiscores : no_shown); i++)
      (void) printf ("|%5d | %-*.*s| %-8.8s|%6ld |%7ld |%7ld |%4ld /%4ld |\n",
		     i+1, NAMELEN, NAMELEN, hi_scores[i].name,
		     hi_scores[i].code, hi_scores[i].score,
		     hi_scores[i].no_pieces, hi_scores[i].no_levels,
		     hi_scores[i].game_level, hi_scores[i].game_mode);
    (void) puts ("+------+---------------------+---------+-------+--------+--------+-----------+");
  }
}

/***************************************************************************\
|*									   *|
|*  The function new_highscore puts a lock on the high-score-file, and	   *|
|*  then (if successful), reads the high-scores, inserts the current	   *|
|*  score in the table if it's good enough, and writes it back if it	   *|
|*  has changed, finally removing the lock.  It returns the player's	   *|
|*  position in the table, or 0 if he is unplaced.  If we were unable	   *|
|*  to get the lock-file, it returns -1.				   *|
|*									   *|
|*  I'm about to do some mods allowing compilation to be done with	   *|
|*  the flag -DLOCKF to do the mutual exclusion using lockf(3) instead	   *|
|*  a lock-file.  This will be unneccesary on most systems, but here on	   *|
|*  the Warwick systems, some users have titchy quotas that cause them	   *|
|*  to be unable to create a lock-file, and thus to use the high-score	   *|
|*  table.  I'm using lockf(3) instead of flock(2) since it works across   *|
|*  machines.								   *|
|*									   *|
|*  Apologies for the cruddy way I've written this function.  It has had   *|
|*  bits added onto it in a very ad-hoc way, including the #ifdef'd bits   *|
|*  that determine what locking mechanism is used, and the result is,	   *|
|*  shall we say, sub-optimal elegance.	 Particularly nasty is that way	   *|
|*  that when the lockf(3)ing locking mechanism is used, we maintain	   *|
|*  two open file-descriptors at once on the same file, but it's the	   *|
|*  quickest and easiest way to use the existing get_scores() function,	   *|
|*  and it does at least work.	Since I don't anticipate anything else	   *|
|*  significant being added to the function, I'm going to leave it as it   *|
|*  is, and not tidy it up unless I feel *really* guilty in the morning.   *|
|*									   *|
\***************************************************************************/

int new_highscore ()
{
  int i = 0, j = 0, k = 0;	/* Sundry re-usable loop-indices */
  int score_fd;			/* The fd through which we write new file */
  int lock_fd;			/* If LOCKF is defined, we use this as */
				/* an auxiliary fd on the SCORE_FILE, one */
				/* that stays open all the time, so we can */
				/* use lockf().	 Otherwise, it is the fd */
				/* that we open to the LOCK_FILE */

  (void) umask (0000);		/* 000 octal, just to make the point */

#ifdef LOCKF
  if ((lock_fd = open (SCORE_FILE, O_RDWR | O_CREAT, 0666)) == -1)
    die (LE_OPEN, "Couldn't open(2) score-file for lockf()");

  while (i++ < 5) {		/* Make up to five attempts */
    if (lockf (lock_fd, F_TLOCK, 0) != -1)
      break;			/* If we succeed, then carry on */	  
				/* Otherwise, if not due to exclusivity */
    if (errno != EAGAIN) {	/* then die with a system error */
      print_msg ("lockf(3) error!");
      (void) get_key ();
      return (-1);
    }
    
    print_msg ("Hi-score access:");
    sleep (1);			/* Back off and wait ... */
    print_msg ("");		/* Then try again */
  }  
#else /* LOCKF */
  while (i++ < 5) {		/* Make up to five attempts */
    if ((lock_fd = open (LOCK_FILE, O_CREAT | O_EXCL, 0666)) != -1)
      break;			/* If we succeed, then carry on */
				/* Otherwise, if not due to exclusivity */
    if (errno != EEXIST) {	/* then die with a system error */
      print_msg ("open(2) error!");
      (void) get_key ();
      return (-1);
    }
    
    print_msg ("Hi-score access:");
    sleep (1);			/* Back off and wait ... */
    print_msg ("");		/* Then try again */
  }  
#endif /* LOCKF */

  if (i > 5)			/* If we tried 5 times unsuccessfully, */
    return (-1);		/* Then give up and return -1 instead */

  get_scores ();
  for (i = 0; i < no_hiscores; i++) {
    if ((((!strcmp (user_code, hi_scores[i].code)) &&
	  (game_mode == hi_scores[i].game_mode)) &&
	 (game_level == hi_scores[i].game_level)) &&
	((score < hi_scores[i].score) ||
	 ((score == hi_scores[i].score) &&
	  ((no_pieces < hi_scores[i].no_pieces) ||
	   ((no_pieces == hi_scores[i].no_pieces) &&
	    (no_levels < hi_scores[i].no_levels)))))) {
      i = NO_HISCORES;		/* If the same user has a better score */
      break;			/* on the same level, then drop this one */
    }
    
    if ((game_mode < hi_scores[i].game_mode) ||
	((game_mode == hi_scores[i].game_mode) &&
	 ((score > hi_scores[i].score) ||
	  ((score == hi_scores[i].score) &&
	   ((no_pieces > hi_scores[i].no_pieces) ||
	    ((no_pieces == hi_scores[i].no_pieces) &&
	     ((no_levels > hi_scores[i].no_levels) ||
	      ((no_levels == hi_scores[i].no_levels) &&
	       ((game_level >= hi_scores[i].game_level))))))))))
                                /* Lisp :-) */
      break;			/* i is the new position of the player */
  }
  
  if (i == NO_HISCORES) {	/* If we looped off the end of the array */
    (void) close (lock_fd);	/* then the score isn't good enough: */
#ifndef LOCKF			/* Automagically removes advisory lockf() */
    (void) unlink (LOCK_FILE);
#endif /* LOCKF */
    return (0);
  }
				/* If there is a matching score lower down */
				/* the file, set j to it, (otherwise to i) */
  for (j = NO_HISCORES-1; j >= i; j--)
    if ((!strcmp (user_code, hi_scores[j].code)) &&
	(game_level == hi_scores[j].game_level))
      break;

				/* No duplicate score found, so just */
  if (j < i) {			/* shunt up all other scores. */
    for (j = NO_HISCORES-1; j > i; j--)
      bcopy ((char*) &hi_scores[j-1], (char*) &hi_scores[j],
	     sizeof (struct score_ent));
    if (no_hiscores < NO_HISCORES)
      no_hiscores++;
  }
  else {			/* j points at a duplicate score of the */
    for (k = j; k > i; k--) {	/* new one, so shift bits between them */
      bcopy ((char*) &hi_scores[k-1], (char*) &hi_scores[k],
	     sizeof (struct score_ent));
    }
  }
  
  (void) strcpy (hi_scores[i].name, user_name);
  (void) strcpy (hi_scores[i].code, user_code);
  hi_scores[i].score = score;
  hi_scores[i].no_pieces = no_pieces;
  hi_scores[i].no_levels = no_levels;
  hi_scores[i].game_level = game_level;
  hi_scores[i].game_mode = game_mode;

  if ((score_fd = open (SCORE_FILE, O_WRONLY | O_CREAT, 0666)) == -1) {
    perror ("open");
    die (LE_OPEN, "couldn't open(2) score-file for writing");
  }

  if (write (score_fd, (char*) hi_scores, no_hiscores*sizeof
	     (struct score_ent)) == -1) {
    perror ("write");
    die (LE_WRITE, "couldn't write(2) to score-file");
  }

  (void) close (score_fd);
  (void) close (lock_fd);
#ifndef LOCKF
  (void) unlink (LOCK_FILE);
#endif /* LOCKF */
  return (i+1);
}

/***************************************************************************\
|*									   *|
|*  The function get_key() reads a character from the keyboard, and	   *|
|*  performs some simple processing on it.  If it's an 's', it lists	   *|
|*  the high-score table.  Otherwise, it returns 1 for a 'q' or 'n',	   *|
|*  and 0 for anything else.						   *|
|*									   *|
\***************************************************************************/

static int get_key ()
{
  char ch;

  (void) read (0, &ch, 1);
  if ((ch == 's') || (ch == 'S')) {
    print_scores ();
    (void) read (0, &ch, 1);
    hoopy_refresh (-1, 0, 0, 0, -1, 0);
    print_msg ("Press a key:");
    (void) read (0, &ch, 1);
  }	 

  print_msg ("");
  return ((ch == 'n') || (ch == 'N') || (ch == 'q') || (ch == 'Q'));
}

/***************************************************************************\
|*									   *|
|*  The main() function handles initialisation, gets keys and names from   *|
|*  the environent, parses command-line arguments and so on.  It then	   *|
|*  goes into the main loop of calling play_game(), and asking if the	   *|
|*  player wants another game, and so on.				   *|
|*									   *|
\***************************************************************************/

int main (argc, argv)
  int argc;
  char **argv;
{
  int i;
  char *cp;			/* Temporary pointer for getenv() */
  time_t ignore_me;		/* Storage for time for random seed. */
  struct passwd *pw_ptr;	/* Used with getuid() to find usercode */

  (void) srandom ((int) time (&ignore_me));
  (void) strcpy (prog_name, basename (argv[0]));

  if ((i = getuid ()) == -1)
    die (LE_GETUID, "couldn't getuid(2)");
  if ((pw_ptr = getpwuid (i)) == NULL)
    die (LE_GETPW, "couldn't get password entry");
  (void) strncpy (user_code, pw_ptr->pw_name, CODELEN-1);
  user_code[CODELEN-1] = '\0';

  if ((cp = getenv ("TTNAME")) == NULL)
    if ((cp = getenv ("NAME")) == NULL)
      cp = user_code;
  (void) strncpy (user_name, cp, NAMELEN-1);
  user_name[NAMELEN-1] = '\0';

  if ((cp = getenv ("TTKEYS")) != NULL) {
    if (*cp != '\0')
      left_key = *(cp++);
    if (*cp != '\0')
      right_key = *(cp++);
    if (*cp != '\0')
      rotate_key = *(cp++);
    if (*cp != '\0')
      drop_key = *(cp++);
    if (*cp != '\0')
      susp_key = *(cp++);
    if (*cp != '\0')
      quit_key = *(cp++);
    if (*cp != '\0')
      cheat_key = *(cp++);
  }

  for (i = 1; i < argc; i++)
    switch (argv[i][0]) {
    case '-':
      switch (argv[i][1]) {
      case 's':
	if (argv[i][2] != '\0') 
	  if (((no_shown = atoi (argv[i]+2)) < 1) ||
	      (no_shown > NO_HISCORES)) {
	    static char tmp[LINELEN]; /* To stop recursive form() */
	    (void) sprintf (tmp, "Number of scores must be between 1 and %d",
			    NO_HISCORES);
	    die (LE_LEVEL, tmp);
	  }
	print_scores ();	/* Flag '-s': print scores, then exit. */
	exit (LE_OK);		/* Not in curses => No need to call die() */
      case 'b':
	rot_backwards = 1;
	break;
      case '3':
	/* Set up for "t3" */
	pieces = pieces3;
	NO_PIECES = 6;
	NO_SQUARES = 3;
	GAME_WIDTH = 10;
	break;
      case '4':
	/* Set up for default game, tetris */
	pieces = pieces4;
	NO_PIECES = 7;
	NO_SQUARES = 4;
	GAME_WIDTH = 10;
	break;
      case '5':
	/* Set up for "t5" aka. "pentris" */
	pieces = pieces5;
	NO_PIECES = 18;
	NO_SQUARES = 5;
	GAME_WIDTH = 15;
	break;
      case '7':
	/* Set up for "tnt", which is tt(4) + t3 == t7, ish. */
	pieces = pieces7;
	NO_PIECES = 13;
	NO_SQUARES = 4;
	GAME_WIDTH = 10;
	break;
      case 'l':
	if (argv[i][2] == '\0')
	  goto USAGE_ERROR;
	else
	  if (((game_level = atoi (argv[i]+2)) < -10) || (game_level > 20))
	    die (LE_LEVEL, "Game-level must be between -10 and 20");
	break;
      case 'm':
	if (argv[i][2] == '\0')
	  goto USAGE_ERROR;
	else
	  if (((game_mode = atoi (argv[i]+2)) < 0) || (game_mode > 8 ))
	    die (LE_MODE, "Game-mode must be betweeen 0 and 8.");
	break;
      case 'g':
	GHOST_PIECES = 1;
	break;
      case 'N':
	SHOW_NEXT = 1;
	break;
      default:
	  goto USAGE_ERROR;
      }
      break;
    default:
    USAGE_ERROR:
      die (LE_USAGE,
	   form ("Usage: %s [-s] [-b] [-g] [-N] [-3|4|5|7] [-l<num>] [-m<num>]",
		 prog_name));
    }

  get_termcap ();
  if (screen_depth < GAME_DEPTH+1)
    die (LE_SCREEN, "screen is not deep enough");
  if (screen_width < (2*GAME_WIDTH)+6+STAT_WIDTH)
    die (LE_SCREEN, "screen is not wide enough");

  if ((signal (SIGHUP,	signal_end)  == SIG_ERR) ||
      (signal (SIGINT,	signal_end)  == SIG_ERR) ||
      (signal (SIGTERM, signal_end)  == SIG_ERR))
    die (LE_SIGNAL, "couldn't set up signal-handling");
  
  setup_curses ();
  setup_screen ();
  clear_board ();
  print_msg ("Press a key:");
  if (get_key ())
    die (LE_OK, "");
  
  while (1) {
    play_game ();

    if ((i = new_highscore ()) > 0) {
      static char tmp[LINELEN]; /* To stop recursive form() phelgming */
      (void) sprintf (tmp, "Score ranks #%d", i);
      print_msg (tmp);
    }
    if (i < 0)
      print_msg ("Save-score failed!");
    if (i != 0) {
#if 0
      flush_keyboard ();
#endif
      if (get_key ())
	break;
    }
    
    print_msg ("Again?");
#if 0
    flush_keyboard ();
#endif
    if (get_key ())
      break;
    
    clear_area ();
  }
  
  die (LE_OK, "");
  return 0;
}

/*-------------------------------------------------------------------------*/
