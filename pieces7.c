/***************************************************************************\
|*									   *|
|*  pieces.c:	A version of Tetris to run on ordinary terminals,	   *|
|*		(ie., not needing a workstation, so should available	   *|
|*		to peasant Newwords+ users.  This module contains the	   *|
|*		definitions of the pieces.				   *|
|*									   *|
|*  Author:	Mike Taylor (mirk@uk.ac.warwick.cs)			   *|
|*  Started:	Fri May 26 12:26:05 BST 1989				   *|
|*                                                                         *|
|*  Changed to "Tetris 'n' Three" by Pete 'Gandalf' Favelle 4/11/90        *|
|*									   *|
\***************************************************************************/

#include "tt.h"
#include "pieces.h"

/*-------------------------------------------------------------------------*/

struct piece pieces7[] = {
  { "[]", 4,			/* Square piece */
      {
	{{0,0}, {0,1}, {1,0}, {1,1}},
	{{0,0}, {0,1}, {1,0}, {1,1}},
	{{0,0}, {0,1}, {1,0}, {1,1}},
	{{0,0}, {0,1}, {1,0}, {1,1}}
      }
  },
  
  { "<>", 2,			/* Long piece */
      {
	{{0,0}, {1,0}, {2,0}, {3,0}},
	{{1,-1}, {1,0}, {1,1}, {1,2}},
	{{0,0}, {1,0}, {2,0}, {3,0}},
	{{1,-1}, {1,0}, {1,1}, {1,2}}
      }
  },
  
  { "()", 3,			/* L-shaped piece */
      {
	{{0,0}, {1,0}, {2,0}, {2,1}},
	{{0,1}, {1,-1}, {1,0}, {1,1}},
	{{0,-1}, {0,0}, {1,0}, {2,0}},
	{{1,-1}, {1,0}, {1,1}, {2,-1}}
      }
  },
  
  { "{}", 3,			/* Backwards L-shaped piece */
      {
	{{0,0}, {1,0}, {2,-1}, {2,0}},
	{{1,-1}, {1,0}, {1,1}, {2,1}},
	{{0,0}, {0,1}, {1,0}, {2,0}},
	{{0,-1}, {1,-1}, {1,0}, {1,1}}
      }
  },
  
  { "##", 1,			/* T-shaped piece */
      {
	{{1,-1}, {1,0}, {1,1}, {2,0}},
	{{0,0}, {1,0}, {1,1}, {2,0}},
	{{0,0}, {1,-1}, {1,0}, {1,1}},
	{{0,0}, {1,-1}, {1,0}, {2,0}}
      }
  },
  
  { "%%", 5,			/* S-shaped piece */
      {
	{{0,0}, {0,1}, {1,-1}, {1,0}},
	{{0,-1}, {1,-1}, {1,0}, {2,0}},
	{{0,0}, {0,1}, {1,-1}, {1,0}},
	{{0,-1}, {1,-1}, {1,0}, {2,0}}
      }
  },
  
  { "@@", 5,			/* Backwards S-shaped piece */
      {
	{{0,-1}, {0,0}, {1,0}, {1,1}},
	{{0,0}, {1,-1}, {1,0}, {2,-1}},
	{{0,-1}, {0,0}, {1,0}, {1,1}},
	{{0,0}, {1,-1}, {1,0}, {2,-1}}
      }
  },

  { "&&", 1,			/* Long piece */
      {
	{{0,0}, {0,-1}, {0,1}, {0,1}},
	{{0,0}, {-1,0}, {1,0}, {1,0}},
	{{0,0}, {0,-1}, {0,1}, {0,1}},
	{{0,0}, {-1,0}, {1,0}, {1,0}}
      }
  },

  { "88", 2,			/* L-shaped piece */
      {
	{{0,0}, {-1,0}, {-1,-1}, {-1,-1}},
	{{0,-1}, {-1,-1}, {-1,0}, {-1,0}},
	{{0,0}, {0,-1}, {-1,-1}, {-1,-1}},
	{{0,0}, {0,-1}, {-1,0}, {-1,0}}
      }
  },

  { "00", 3,			/* Three-shaped piece */
      {
        {{0,-1}, {-1,0}, {1,0}, {1,0}},
        {{1,0}, {0,-1}, {0,1}, {0,1}},
        {{-1,0}, {0,1}, {1,0}, {1,0}},
        {{-1,0}, {0,-1}, {0,1}, {0,1}}
      }
  },

  { "XX", 4,			/* Sorta inverse 'ARR' piece */
      {
	{{-1,1}, {0,0}, {0,-1}, {0,-1}},
        {{-1,-1}, {0,0}, {1,0}, {1,0}},
        {{0,0}, {0,1}, {1,-1}, {1,-1}},
        {{-1,0}, {0,0}, {1,1}, {1,1}}
      }
  },

  { "**", 4,			/* R shaped piece (use yer imagination ;-) */
      {
        {{0,-1}, {0,0}, {1,1}, {1,1}},
	{{-1,1}, {0,0}, {1,0}, {1,0}},
	{{-1,-1}, {0,0}, {0,1}, {0,1}},
	{{-1,0}, {0,0}, {1,-1}, {1,-1}}
      }
  },

  { "$$", 5,			/* Diagonal piece */
      {
        {{0,0}, {-1,-1}, {1,1}, {1,1}},
	{{0,0}, {-1,1}, {1,-1}, {1,-1}},
	{{0,0}, {-1,-1}, {1,1}, {1,1}},
	{{0,0}, {-1,1}, {1,-1}, {1,-1}}
      }
  },
};

/*-------------------------------------------------------------------------*/
