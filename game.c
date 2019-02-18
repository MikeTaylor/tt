/***************************************************************************\
|*									   *|
|*  game.c:	A version of Tetris to run on ordinary terminals,	   *|
|*		(ie., not needing a workstation, so should available	   *|
|*		to peasant Newwords+ users.  This is the module that	   *|
|*		actually plays the game, (ie. moves things down the	   *|
|*		screen, select(2)s on the keyboard, and so on)		   *|
|*									   *|
|*  Author:	Mike Taylor (mirk@uk.co.ssl)				   *|
|*  Started:	Fri May 26 12:26:05 BST 1989				   *|
|*									   *|
\***************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>

#include "tt.h"
#include "game.h"
#include "screen.h"
#include "pieces.h"
#include "utils.h"

/*-------------------------------------------------------------------------*/

extern long int random ();

/*-------------------------------------------------------------------------*/

char left_key	= LEFT_KEY;	/* Move piece left */
char right_key	= RIGHT_KEY;	/* Move piece right */
char rotate_key = ROTATE_KEY;	/* Rotate piece anticlockwise */
char drop_key	= DROP_KEY;	/* Drop piece to bottom of screen */
char susp_key	= SUSP_KEY;	/* Suspend.  I'm sorry if its confusing */
char quit_key	= QUIT_KEY;	/* Quit.  I'm sorry if its confusing */
char cheat_key	= CHEAT_KEY;	/* Do whatever I eventaully make it do */

/***************************************************************************\
|*									   *|
|*  Oh good grief, what on earth is the point of putting a huge, wobbly	   *|
|*  box-comment in front of this titchy little self-explanatory function?  *|
|*  The name tells the whole story, it's perfectly strightforward, it's	   *|
|*  not a proposition from Witgenstein.	 I wouldn't bother commenting it   *|
|*  at all if it wasn't for the fact that I know I'd feel guilty in the	   *|
|*  morning.  I mean, be fair, you don't really want a program where all   *|
|*  the functions *except one* have box-comments explaining them, do you?  *|
|*  Ah well, here we go for completion's sake:				   *|
|*									   *|
|*  The function clear_board() takes no parameters and returns no value.   *|
|*  It clears the board.  The end.					   *|
|*									   *|
\***************************************************************************/

void clear_board ()
{
  int i, j;

  for (i = 0; i < GAME_DEPTH+4; i++)
    for (j = 0; j < GAME_WIDTH; j++)
      board[i][j] = PI_EMPTY;
}

/***************************************************************************\
|*									   *|
|*  The function play_game is the main loop in which the game of Tetris	   *|
|*  is implemented.  It takes no parameters, and returns no value.  The	   *|
|*  time at which it returns no value is the end of a game.  The main	   *|
|*  loop-component is a select(2) which polls the keyboard in real-time.   *|
|*  If you use a non-Berkeley UNIX(tm), you're well cheesed.		   *|
|*									   *|
|*  Actually, I have to admit I'm not proud of this one.  It must be one   *|
|*  of the messiest functions I've written in years, in terms of nested	   *|
|*  loops with ad-hoc exit conditions, re-used variables, general	   *|
|*  obfuscation and so on.  I wanna make it quite clear that I make no	   *|
|*  apologies for my use of "goto", which remains a highly desirable	   *|
|*  language feature, and is still the most elegant way of coding many	   *|
|*  things, but I gotta admit, overall this one is bit of a chicken.	   *|
|*									   *|
\***************************************************************************/

void play_game ()
{
  int i;			/* Position of origin down board */
  int j;			/* Position of origin across board */
  int k;			/* Loop variable when i,j are invariant */
  int piece_no;			/* Type of piece currently falling */
  int orient;			/* Which way it is facing */
  static int next_piece_no=0;	/* Which piece is "next" */
  static int next_orient=0;	/* And at what orientation is it? */
  int pause_flag = 0;		/* We won't pause unless told to */
  int presses_left;		/* Futher moves possible this drop */
  int free_left = game_level;	/* Number of pieces to drop at start */
  fd_set read_fds;		/* Select must look at stdin */
  long int total_time = 200000; /* Allow 1/4 second before falling */
  struct timeval tmout;		/* Time before piece drops in select(2) */
  char ch;			/* Keystroke (command) */

  score = no_levels = no_pieces = 0;
  update_scores ();
  clear_board ();

  next_piece_no = (int) (random () % (NO_PIECES-(2*game_mode)));
  next_orient = (int) (random () % NO_ORIENTS);

  while (1) {
    piece_no = next_piece_no;
    orient   = next_orient;
    next_piece_no = (int) (random () % (NO_PIECES-(2*game_mode)));
    next_orient = (int) (random () % NO_ORIENTS);
    
    if (SHOW_NEXT) {
      draw_raw (piece_no, orient, NEXT_Y, NEXT_X, erase_as);
      draw_raw (next_piece_no, next_orient, NEXT_Y, NEXT_X, 
		pieces[next_piece_no].app);
    } /* if we are showing the next piece */


    i = -2;			/* Start falling from off-screen */
    if (free_left > 0) {
      int leftmost = GAME_WIDTH, rightmost = -GAME_WIDTH, boxlet, x;
      for (boxlet = 0; boxlet < NO_SQUARES; boxlet++) {
	x = (pieces[piece_no].index)[orient][boxlet][1];
	if (x < leftmost) leftmost = x;
	if (x > rightmost) rightmost = x;
      }
      j = (int) (random () % (GAME_WIDTH-(rightmost-leftmost)))-leftmost;
    } else
      j = GAME_WIDTH/2;
    
    if (!can_place (piece_no, orient, i, j)) {
      for (k = 0; k < 9; k++) { /* Crude but (hopefully) effective */
	draw_piece (piece_no, orient, i, j, PD_ERASE);
	myrefresh (); usleep (80000);
	draw_piece (piece_no, orient, i, j, PD_DRAW);
	myrefresh (); usleep (80000);
      }
      break;			/* End of game - piece won't fit */
    }

    if (free_left != 0) {	/* If there are pieces to be dropped, */
      if (free_left > 0)	/* And the number is positiive, */
	free_left--;		/* Then decrement it, otherwise */
      else			/* increment it, in any case, bring */
	free_left++;		/* it closer to zero if it gets to zero */
      if (free_left == 0)	/* set a flag so that the game will */
	pause_flag = 1;		/* pause.  Then go to the bit of code */
      goto DROP_PIECE;		/* that drops it. */
    }

    while (1) {
      presses_left = NO_MOVES;
      draw_piece (piece_no, orient, i, j, PD_DRAW);
      update_scores ();
      myrefresh ();

      while (1) {
	FD_ZERO (&read_fds);
	FD_SET (0, &read_fds);
	tmout.tv_sec = 0;
	tmout.tv_usec = total_time;
	switch (select (((presses_left > 0) && (i >= 0)), &read_fds,
			(fd_set*) NULL, (fd_set*) NULL, &tmout)) {
	case -1:
	  if (errno != EINTR)
	    die (LE_SELECT, "select(2) failed in play_game()");
	  else {		/* otherwise fall through, goto TMOUT */
	    print_msg ("Continue");
#if 0
	    flush_keyboard ();
#endif
	    (void) read (0, &ch, 1);
	    print_msg ("");
	  }
	case 0:
	  goto TMOUT;
	default:
	  if (read (0, &ch, 1) == -1)
	    die (LE_READ, "read(2) failed in play_game()");

	  if (ch == REFRESH_KEY)
	    hoopy_refresh (piece_no, orient, i, j, next_piece_no, next_orient);

	  if ((ch != left_key) && (ch != right_key) && (ch != rotate_key) &&
	      (ch != drop_key) && (ch != quit_key) && (ch != susp_key) &&
	      (ch != cheat_key))
	    break;

	  presses_left--;
	  if (ch == left_key) {
	    if (can_place (piece_no, orient, i, j-1)) {
	      draw_piece (piece_no, orient, i, j, PD_ERASE);
	      j--;
	      draw_piece (piece_no, orient, i, j, PD_DRAW);
	      myrefresh ();
	    }
	  }

	  else if (ch == right_key) {
	    if (can_place (piece_no, orient, i, j+1)) {
	      draw_piece (piece_no, orient, i, j, PD_ERASE);
	      j++;
	      draw_piece (piece_no, orient, i, j, PD_DRAW);
	      myrefresh ();
	    }
	  }

	  else if (ch == rotate_key) {
	    int new_or = ((orient+NO_ORIENTS+1-(2*rot_backwards))%
			  NO_ORIENTS);
	    if (can_place (piece_no, new_or, i, j)) {
	      draw_piece (piece_no, orient, i, j, PD_ERASE);
	      orient = new_or;
	      draw_piece (piece_no, orient, i, j, PD_DRAW);
	      myrefresh ();
	    }
	  }

	  else if (ch == drop_key) {
	  DROP_PIECE:
	    while (can_place (piece_no, orient, i+1, j)) {
	      draw_piece (piece_no, orient, i, j, PD_ERASE);
	      i++;
	      draw_piece (piece_no, orient, i, j, PD_DRAW);
	      myrefresh ();
	    }
	    goto TMOUT;
	  }

	  else if (ch == quit_key)
	    return;

	  else if (ch == cheat_key) {
	    print_msg ("CHEAT!");
	    total_time = 0L;
	  }

	  else if (ch == susp_key) {
	    clear_area ();
	    print_msg ("Paused");
	    (void) read (0, &ch, 1);
	    draw_board ();
	    print_msg ("");
	  }

	  break;
	} /* End of "switch(select())" ... ?!? */
      }

    TMOUT:
      if (!can_place (piece_no, orient, i+1, j)) {
	place_piece (piece_no, orient, i, j);
	score += pieces[piece_no].points;
	no_pieces++;
	update_scores ();
	myrefresh ();

	for (i = 0; i < GAME_DEPTH; i++) {
	  for (j = 0; j < GAME_WIDTH; j++)
	    if (board[i+4][j] == PI_EMPTY)
	      break;

	  if (j == GAME_WIDTH) {
	    no_levels++;
	    score += 10;
	    update_scores ();
	    for (k = i; k > 0; k--)
	      for (j = 0; j < GAME_WIDTH; j++)
		board[k+4][j] = board[k+3][j];
	    for (j = 0; j < GAME_WIDTH; j++)
	      board[4][j] = PI_EMPTY;
	    draw_board ();
	    myrefresh ();
	    i--;		/* Check the new row i */
	  }
	}

	if (pause_flag) {	/* If we are pausing after this drop ... */
	  pause_flag = 0;	/* Ensure we don't do so next time. */
	  print_msg ("Continue");
#if 0
	  flush_keyboard ();
#endif
	  (void) read (0, &ch, 1);
	  print_msg ("");
	}

	break;			/* End of fall - piece has hit floor */
      }
      
      draw_piece (piece_no, orient, i, j, PD_ERASE);
      i++;
    }

    myrefresh ();
    if (total_time != 0)
      total_time -= 100;
  }
}

/*-------------------------------------------------------------------------*/
