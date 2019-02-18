/***************************************************************************\
|*									   *|
|*  screen.c:	A version of Tetris to run on ordinary terminals,	   *|
|*		(ie., not needing a workstation, so should available	   *|
|*		to peasant Newwords+ users.  This module handles all	   *|
|*		the icky curses(3x) bits.				   *|
|*									   *|
|*  Author:	Mike Taylor (mirk@uk.co.ssl)				   *|
|*  Started:	Fri May 26 12:26:05 BST 1989				   *|
|*									   *|
\***************************************************************************/

#ifdef SYSV
#  undef SYSV	/* SysV curses actually defines SYSV */
#endif
#include <string.h>
#include <curses.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>

#include "screen.h"
#include "tt.h"
#include "pieces.h"
#include "utils.h"
#include "game.h"

char ghost_as[3] = "..";
char erase_as[3] = {BLANK_CHAR, BLANK_CHAR, 0};

/***************************************************************************\
|*									   *|
|*  The function myrefresh() calls the curses(3x) function refresh()	   *|
|*  after first moving the cursor out of harm's way at the bottom of	   *|
|*  the screen.								   *|
|*									   *|
\***************************************************************************/

void myrefresh ()
{
  int x;
  
  if ((x = screen_depth-2) < GAME_DEPTH+1)
    x = GAME_DEPTH+1;
  
  move (x, 0);
  refresh ();
}

/***************************************************************************\
|*									   *|
|*  The function hoopy_refresh() touches the screen, then refreshes it,	   *|
|*  so curses(3X) doesn't get any chance to phlegm about with only doing   *|
|*  the bits that it thinks are OK.  So this should fix up screens that	   *|
|*  have been interfered with by biff(1), mesg(1) etc.			   *|
|*									   *|
|*  Hmm, it seems that my original plan is insufficient to make curses	   *|
|*  get its act together, so I have to physically re-draw the screen.	   *|
|*  The function now also takes parameters describing any piece that is	   *|
|*  active, and if the piece-number is non-negative, then the piece is	   *|
|*  drawn (with draw_piece())						   *|
|*									   *|
\***************************************************************************/

void hoopy_refresh (piece_no, orient, x, y, next_piece_no, next_orient)
  int piece_no, orient, x, y, next_piece_no, next_orient;
{
  clear ();
  setup_screen ();
  draw_board ();
  update_scores ();
  if (piece_no > -1)
    draw_piece (piece_no, orient, x, y, PD_DRAW, 0);
  if (next_piece_no > -1 && SHOW_NEXT)
    draw_piece (next_piece_no, next_orient, NEXT_Y, NEXT_X, PD_DRAW, 0);
  myrefresh ();
}

/***************************************************************************\
|*									   *|
|*  The function print_msg() prints a short message centered on the	   *|
|*  floor of the playing area, with a space before and after it.	   *|
|*  If the message is NULL (ie. (char*)0), or null, (ie. ""), then	   *|
|*  no message is printed.						   *|
|*									   *|
\***************************************************************************/

void print_msg (line)
  char *line;
{
  int i;

  move (GAME_DEPTH, 2);
  for (i = 0; i < 2*GAME_WIDTH; i++)
    addch (FLOOR_CHAR);

  if ((line != NULL) && (*line != '\0'))
    mvaddstr (GAME_DEPTH, GAME_WIDTH+1-(strlen (line))/2,
	      form (" %s ", line));
  myrefresh ();
}

/***************************************************************************\
|*									   *|
|*  The function clear_area() fills the playing area with BLANK_CHARs.	   *|
|*  It is used to clear the screen before each game, and also clearing	   *|
|*  the screen while displaying pieces one at a time when in debug mode.   *|
|*									   *|
\***************************************************************************/

void clear_area ()
{
  int i, j, status, fastflag = 0;
  fd_set fds;
  struct timeval tmout;
  static char buffer[LINELEN];

  for (i = 0; i < GAME_DEPTH; i++) {
    move (i, 2);
    for (j = 0; j < 2*GAME_WIDTH; j++)
      addch (BLANK_CHAR);
    
    if (fastflag == 0) {
      move (i+1, 2);
      for (j = 0; j < 2*GAME_WIDTH; j++)
	addch (FLOOR_CHAR);
      myrefresh ();

      FD_ZERO (&fds);
      FD_SET (0, &fds);
      tmout.tv_sec = 0L;	/* Future implementations of select(2) */
      tmout.tv_usec = 50000L; /* might change this value on return */
      if ((status = select (1, &fds, (fd_set*) NULL, (fd_set*) NULL,
			    &tmout)) == -1)
	if (errno != EINTR)
	  die (LE_SELECT, "select(2) failed in clear_area()");
      
      if (status != 0) {
	fastflag = 1;
	(void) read (0, buffer, LINELEN);
      }
    }
  }
  
  move (GAME_DEPTH, 2);
  for (j = 0; j < 2*GAME_WIDTH; j++)
    addch (FLOOR_CHAR);
  
  mvaddch (GAME_DEPTH, 0, CORNER_CHAR);
  addch (CORNER_CHAR);
  mvaddch (GAME_DEPTH, 2*GAME_WIDTH+2, CORNER_CHAR);
  addch (CORNER_CHAR);
  
  myrefresh ();
}

/***************************************************************************\
|*									   *|
|*  The function setup_screen should be called exactly once, near the	   *|
|*  beginning of execution.  It initialises curses(3x), and prints the	   *|
|*  game titles, the walls and the floor of the game area, and clears	   *|
|*  this area using clear_area() (perhaps unsurprisingly)		   *|
|*									   *|
|*  STOP PRESS: It no longer calls clear_area(), since play_game()	   *|
|*	does that fairly immediately after this function returns.	   *|
|*									   *|
|*  STOP PRESS^2: It now does do it again, since play_game() does the	   *|
|*	sneaky-but-slow clear that it fine at times, but cruddy for	   *|
|*	initialisation.							   *|
|*									   *|
\***************************************************************************/

void setup_screen ()
{
  int i;

  for (i = 0; i < GAME_DEPTH; i++) {
    mvaddch (i, 0, WALL_CHAR);
    addch (WALL_CHAR);
    mvaddch (i, 2*GAME_WIDTH+2, WALL_CHAR);
    addch (WALL_CHAR);
  }

  move (GAME_DEPTH, 2);
  for (i = 0; i < 2*GAME_WIDTH; i++)
    addch (FLOOR_CHAR);
  
  mvaddch (GAME_DEPTH, 0, CORNER_CHAR);
  addch (CORNER_CHAR);
  mvaddch (GAME_DEPTH, 2*GAME_WIDTH+2, CORNER_CHAR);
  addch (CORNER_CHAR);
  
  /* XXX -- Harv's SYSV port uses standout()/standend() here */
  mvaddstr (0, 2*GAME_WIDTH+6, form ("%sTETRIS FOR TERMINALS%*s%s",
				     so_str, so_gunk, "", se_str));
  mvaddstr (2, 2*GAME_WIDTH+6, "Written by Mike Taylor");
  mvaddstr (3, 2*GAME_WIDTH+6, "Email: mirk@uk.co.ssl");
  mvaddstr (4, 2*GAME_WIDTH+6, "Started: Fri May 26 12:26:05 BST 1989");
  mvaddstr (6, 2*GAME_WIDTH+6, form ("Game level: %d  Game mode: %d",
				     game_level, game_mode));
  mvaddstr (8, 2*GAME_WIDTH+6, "Score:");
  mvaddstr (9, 2*GAME_WIDTH+6, "Pieces:");
  mvaddstr (10, 2*GAME_WIDTH+6, "Levels:");
  if (SHOW_NEXT)
    mvaddstr (NEXT_Y-1, 2*GAME_WIDTH+28, "Next:");

  mvaddstr (12, 2*GAME_WIDTH+8, "Use keys:");
  mvaddstr (13, 2*GAME_WIDTH+8, "=========");
  mvaddstr (14, 2*GAME_WIDTH+8, form ("Move left:  '%c'", left_key));
  mvaddstr (15, 2*GAME_WIDTH+8, form ("Move right: '%c'", right_key));
  mvaddstr (16, 2*GAME_WIDTH+8, form ("Rotate:     '%c'", rotate_key));
  mvaddstr (17, 2*GAME_WIDTH+8, form ("Drop:       '%c'", drop_key));
  mvaddstr (18, 2*GAME_WIDTH+8, form ("Pause:      '%c'", susp_key));
  mvaddstr (19, 2*GAME_WIDTH+8, form ("Quit:       '%c'", quit_key));
  mvaddstr (20, 2*GAME_WIDTH+8, "Refresh:    '^L'");
}

/***************************************************************************\
|*									   *|
|*  The function setup_curses should be called exactly once, near the	   *|
|*  beginning of execution.  It initialises curses(3x), and notes that	   *|
|*  this has been done, by setting the global variable in_curses.	   *|
|*									   *|
\***************************************************************************/

void setup_curses ()
{
  initscr ();
  clear ();
#ifndef lint
  noecho ();
  crmode ();
#endif /*lint*/
  in_curses = 1;
}

/***************************************************************************\
|*									   *|
|*  The function update_scores() puts the sepecified values of score,	   *|
|*  no_pieces and no_levels on the screen in the specified positions.	   *|
|*									   *|
\***************************************************************************/

void update_scores ()
{
  int i;
  move (8, 2*GAME_WIDTH + 14);
  for (i=0; i<SCORE_WIDTH; i++) addch (BLANK_CHAR);
  addstr (form ("%d", score));

  move (9, 2*GAME_WIDTH + 14);
  for (i=0; i<SCORE_WIDTH; i++) addch (BLANK_CHAR);
  addstr (form ("%d", no_pieces));

  move (10, 2*GAME_WIDTH + 14);
  for (i=0; i<SCORE_WIDTH; i++) addch (BLANK_CHAR);
  addstr (form ("%d", no_levels));
}

/***************************************************************************\
|*									   *|
|*  The function draw_board() puts the current state of the global array   *|
|*  board[] ontop the curses(3x) screen, then refresh()es it.		   *|
|*									   *|
\***************************************************************************/

void draw_board ()
{
  int i, j;

  for (i = 0; i < GAME_DEPTH; i++)
    for (j = 0; j < GAME_WIDTH; j++)
      if (board[i+4][j] == PI_EMPTY) {
	mvaddch (i, 2*j+2, BLANK_CHAR);
	addch (BLANK_CHAR);
      }
      else
	mvaddstr (i, 2*j+2, pieces[board[i+4][j]].app);
}

/***************************************************************************\
|*									   *|
|*  The function draw_raw draws one of the tetris pieces on the screen in  *|
|*  a specified orientation and position and string.  It can thus be used  *|
|*  for drawing, erasing, or drawing a ghost of a piece, by passing the    *|
|*  appropriate arg as "str".						   *|
|*  This function is also called by draw_piece to do all of these things.  *|
|*  The form of the function is:					   *|
|*									   *|
|*      draw_raw (piece_no, orient, y, x, str)				   *|
|*									   *|
|*  All the arrguments are integers.  Common values of str are erase_as    *|
|*  (for erasing), ghost_as (for drawing the ghost), or			   *|
|*  pieces[piece_no].app (for drawing the piece itself.  Piece_no is	   *|
|*  between 0 and 6 inclusive, and specifies what sort of piece is	   *|
|*  required.  Orientation is between 0 and 3 inclusive, and states	   *|
|*  which way up the piece is to be drawn, and y and x express the	   *|
|*  position as an index into the GAME_DEPTH by GAME_WIDTH array	   *|
|*  that is the board.							   *|
|*									   *|
\***************************************************************************/
void draw_raw (piece_no, orient, y, x, str)
  int piece_no;
  int orient;
  int y;
  int x;
  char *str;
{
  int i;
  for (i = 0; i < NO_SQUARES; i++) {
    if (y+pieces[piece_no].index[orient][i][0] >= 0) {
      mvaddstr (y+pieces[piece_no].index[orient][i][0],
	        (2*(x+pieces[piece_no].index[orient][i][1]))+2,
	        str);
    }
  }
}

/***************************************************************************\
|*									   *|
|*  draw_piece is the drawing function that should usually be called from  *|
|*  the outside.  It draws (or erases) the piece (and ghost), all	   *|
|*  regular-like.  It just makes the appropriate calls to draw_raw (after  *|
|*  figuring out where the ghost should be).				   *|
|*  The only situation to use draw_raw instead of draw_piece is if you     *|
|*  don't want the ghost to show up, such as for drawing the "Next"        *|
|*  piece.								   *|
|*  All the arrguments are integers.  Flag is either PD_DRAW or		   *|
|*  PD_ERASE, specifying the effect of the function.  The rest are as in   *|
|*  draw_raw.								   *|
|*									   *|
\***************************************************************************/
void draw_piece (piece_no, orient, y, x, flag)
  int piece_no;
  int orient;
  int y;
  int x;
  int flag;
{
  int j;
  
  if (GHOST_PIECES) 
    j = ghost_height(piece_no, orient, y, x);

  if (flag == PD_ERASE) {
    draw_raw (piece_no, orient, j, x, erase_as );
    draw_raw (piece_no, orient, y, x, erase_as );
  } /* if we're erasing */
  else {
    if (GHOST_PIECES) 
      draw_raw (piece_no, orient, j, x, ghost_as );
    draw_raw (piece_no, orient, y, x, pieces[piece_no].app );
  } /* else we are drawing */
}


/***************************************************************************\
|*									   *|
|*  The function place_piece takes the same parameters as draw_piece,	   *|
|*  except for the flag, and does not draw the piece, but places it	   *|
|*  on the board.  No checking is done to see if it will fit, since	   *|
|*  should already have been done by can_place().			   *|
|*									   *|
\***************************************************************************/

void place_piece (piece_no, orient, y, x)
  int piece_no;
  int orient;
  int y;
  int x;
{
  int i;

  for (i = 0; i < NO_SQUARES; i++)
    board[y+4+pieces[piece_no].index[orient][i][0]]
	 [x+pieces[piece_no].index[orient][i][1]] = piece_no;
}

/***************************************************************************\
|*									   *|
|*  The function can_place takes the same parameters as place_piece,	   *|
|*  It does not draw the piece, nor place it on the board, but returns	   *|
|*  an integer value -- 0 if the piece will not fit on the board, 1 if	   *|
|*  it will (with the specified orientation, position, etc.)		   *|
|*									   *|
\***************************************************************************/

#define HERE(x) pieces[piece_no].index[orient][i][x]

int can_place (piece_no, orient, y, x)
  int piece_no;
  int orient;
  int y;
  int x;
{
  int i;

  for (i = 0; i < NO_SQUARES; i++)
    if (((x+HERE(1) >= GAME_WIDTH) ||	/* Off right of screen or */
	 (x+HERE(1) < 0)) ||		/* Off left of screen */
	(y+HERE(0) >= GAME_DEPTH) ||	/* Off bottom of screen */
	(board[y+4+HERE(0)][x+HERE(1)] != PI_EMPTY))
					/* Board position not empty */
      return (0);
  
  return (1);
}


/***************************************************************************\
|*									   *|
|*  The function ghost_height takes piece_no, orient, y, and x, all	   *|
|*  integers describing the type, orientation, and location of a piece),   *|
|*  and returns the height at which we should draw the "ghost" piece.	   *|
|*  The ghost piece is the one that shows where it would be if we dropped  *|
|*  the piece right this instant.					   *|
|*									   *|
\***************************************************************************/
int ghost_height(int piece_no, int orient, int y, int x) 
{
    int j;
    for(j = y; j < GAME_DEPTH; j++) {
      if( !can_place(piece_no, orient, j, x) ) { break; }
    } /* for from here to the bottom of the board */

    j--;
    return j;
}


/*-------------------------------------------------------------------------*/
