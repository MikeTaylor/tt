/***************************************************************************\
|*									   *|
|*  utils.h:	A version of Tetris to run on ordinary terminals,	   *|
|*		(ie., not needing a workstation, so should available	   *|
|*		to peasant Newwords+ users.  This module supplies all	   *|
|*		the function that I will need, but which are not	   *|
|*		intrinsically a part of Tetris itself.			   *|
|*									   *|
|*  Author:	Mike Taylor (mirk@uk.co.ssl)				   *|
|*  Started:	Fri May 26 12:26:05 BST 1989				   *|
|*									   *|
\***************************************************************************/

extern char *basename ();
extern char *form (char *fmt, ...);
extern void die ();
extern void get_termcap ();
extern void flush_keyboard ();

/*-------------------------------------------------------------------------*/
