/***************************************************************************\
|*									   *|
|*  pieces.h:	A version of Tetris to run on ordinary terminals,	   *|
|*		(ie., not needing a workstation, so should available	   *|
|*		to peasant Newwords+ users.  This module contains the	   *|
|*		definitions of the pieces.				   *|
|*									   *|
|*  Author:	Mike Taylor (mirk@uk.co.ssl)				   *|
|*  Started:	Fri May 26 12:26:05 BST 1989				   *|
|*									   *|
\***************************************************************************/

struct piece {
  char app[3];
  int points;
  /* `5' below is the maximum number of squares in pieces of any set */
  int index[NO_ORIENTS][5][2];
};

/*-------------------------------------------------------------------------*/

extern struct piece pieces3[];	/* Pieces for "t3" (Harv's idea) */
extern struct piece pieces4[];	/* Pieces for vanilla tetris */
extern struct piece pieces5[];	/* Pieces for "pentris" (Kenton's idea) */
extern struct piece pieces7[];	/* Pieces for "tnt" (Pete Favelle's idea) */
extern struct piece *pieces;

/*-------------------------------------------------------------------------*/
