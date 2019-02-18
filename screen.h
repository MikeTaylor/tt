/***************************************************************************\
|*									   *|
|*  screen.h:	A version of Tetris to run on ordinary terminals,	   *|
|*		(ie., not needing a workstation, so should available	   *|
|*		to peasant Newwords+ users.  This module handles all	   *|
|*		the icky curses(3x) bits.				   *|
|*									   *|
|*  Author:	Mike Taylor (mirk@uk.co.ssl)				   *|
|*  Started:	Fri May 26 12:26:05 BST 1989				   *|
|*									   *|
\***************************************************************************/

#define WALL_CHAR	'|'	/* Character used for sides of pay-area */
#define FLOOR_CHAR	'='	/* Character used for base-line */
#define CORNER_CHAR	'+'	/* Character used bottom left and right */
#define BLANK_CHAR	' '	/* Character used elsewhere */

#define PD_DRAW		0	/* Code to draw piece */
#define PD_ERASE	1	/* Code to erase piece */

extern char ghost_as[3];	/* With which characters to draw the ghost 
				 * piece (defined in screen.c) */
extern char erase_as[3];	/* With which characters to erase a piece
				 * (in screen.c) */

#define SCORE_WIDTH	7	/* How many digits for your score? */
#define NEXT_Y		9	/* Where to draw the "next" box */
#define NEXT_X		2*GAME_WIDTH+6

/*-------------------------------------------------------------------------*/

extern void myrefresh ();
extern void hoopy_refresh ();
extern void print_msg ();
extern void clear_area ();
extern void setup_screen ();
extern void setup_curses ();
extern void update_scores ();
extern void draw_board ();
extern void draw_piece_and_ghost();
extern void draw_piece ();
extern void place_piece ();
extern int can_place ();

/*-------------------------------------------------------------------------*/
