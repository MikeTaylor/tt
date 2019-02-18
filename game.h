/***************************************************************************\
|*									   *|
|*  game.h:	A version of Tetris to run on ordinary terminals,	   *|
|*		(ie., not needing a workstation, so should available	   *|
|*		to peasant Newwords+ users.  This is the module that	   *|
|*		actually plays the game, (ie. moves things down the	   *|
|*		screen, select(2)s on the keyboard, and so on)		   *|
|*									   *|
|*  Author:	Mike Taylor (mirk@uk.co.ssl)				   *|
|*  Started:	Fri May 26 12:26:05 BST 1989				   *|
|*									   *|
\***************************************************************************/

#define LEFT_KEY	','	/* Move piece left */
#define RIGHT_KEY	'/'	/* Move piece right */
#define ROTATE_KEY	'.'	/* Rotate piece anticlockwise */
#define DROP_KEY	' '	/* Drop piece to bottom of screen */
#define SUSP_KEY	's'	/* Suspend.  I'm sorry if its confusing */
#define QUIT_KEY	'q'	/* Quit.  I'm sorry if its confusing */
#define CHEAT_KEY	'Z'	/* Cheat (a bit) -- Might not help! */

#define REFRESH_KEY	'\014'	/* Control-L; refreshes screen */
				/* This is hard-coded in.  It can't */
				/* be changed by setting TTKEYS, since */
				/* the peasants couldn't handle all that */
				/* "setenv TTKEYS `echo | tr`" carp(anag) */

#define NO_MOVES 4		/* Number of moves allowed per fall */

/*-------------------------------------------------------------------------*/

extern char left_key;		/* Move piece left */
extern char right_key;		/* Move piece right */
extern char rotate_key;		/* Rotate piece anticlockwise */
extern char drop_key;		/* Drop piece to bottom of screen */
extern char susp_key;		/* Suspend.  I'm sorry if its confusing */
extern char quit_key;		/* Quit.  I'm sorry if its confusing */
extern char cheat_key;		/* Frogging identical comments :-P */

extern char erase_as[3];

/*-------------------------------------------------------------------------*/

extern void clear_board ();
extern void play_game ();

/*-------------------------------------------------------------------------*/
