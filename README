Tetris for Terminals - "tt" - Written by Mike Taylor
====================================================

(1)  Specification
==================

	This is a game written in C for Berkeley and System V UNIX
	machines.  It was written on a Sun4 and has been extensively
	played and tested on both this machine and a Sun3, both
	running SunOS 4.0.1, a Berkeley 4.3bsd-derived UNIX, but it
	should port with minimum difficulties to any Berkeley UNIX.
	It has also been ported to SCO Unix System V/386 3.2.2, and
	should therefore be portable to other System V UNIX systems.
	One potential difficulty to look out for is incompatibility of
	the high-score file between different architectures, (eg.
	sun3 and sun4). I've done all I can to make it work, but there
	are no guarantees, and you might be better compiling multiple-
	architecture "tt"s with different high-score tables.

(2)  Compilation
================

	The Makefile as included should be pretty much applicable
	anywhere.  Simply edit the file "tt.c" to give the pathname of
	the file you want to use for the high-score table
	(SCORE_FILE), and unless you are using the LOCKF #definition
	(in tt.h), the file you want to use as a lock for this table
	(LOCK_FILE, which should be in a publicly writeable
	directory).  If your machine is running System V or a similar
	UNIX, then you should make sure that CFLAGS (see the Makefile)
	contains the flag -DSYSV.

	Note that if you're on a System V Unix, you should add
	-DNO_NAP_SYSCALL to CFLAGS if you do not have the nap(S)
	system call.  Try looking for it in the -lx library.  If you
	don't have the usleep() system call on BSD Unix, then add
	-DNO_USLEEP_SYSCALL to CFLAGS.

	Now type "make".  The program will be compiled, producing a
	binary called "tt".  This can then be moved to a bin directory
	if required.

(3) Acknowledgements
====================

	The game Tetris was apparently designed by "a Russian
	Researcher".  Sadly, history, or at least the version of it
	that I heard, does not record his name, and it seems that the
	poor frog-head not only made no money from his brilliant idea,
	but also didn't get any fame.  'S'sad.	Still, I credit him
	with the original idea, whoever he may be.

	The inspiration for this program came from two recently-posted
	versions of Tetris, both of which run on Sun workstations
	only.  There are a few of these around here, but many people
	have no access to them.	 I wrote this version so that everyone
	could play.  It's the communist in me :-)

	The program design, planning (hah!), coding, and so on for
	this version was all done entirely by me, (Mike Taylor), and
	very smug I am about it, too.  The only exception to this is
	that the game-levels were accidentally co-invented by James
	"root@weed" Beckett, by pressing the space-bar too many times.

	Play-testing, which has been extensive even now, less than a
	week after I started writing, has been by many people, but
	special mention goes to Paul Metcalfe, Kenton Oatley, Harvey
	"Max" Thomson and Paul "Freddy" Capper.	 No mention *at all*,
	not even this one, goes to Mike "Sunny" Lessacher, who claims
	not to like Tetris.  ("Not like Tetris?	 What does that
	mean?" :-)

	All documentation is also by me.

Update: Thu Jul  5 16:04:21 BST 1990

	I've added a bug-fix supplied by Boris Lucre Goldowsky
	(boris@edu.rochester.psych.prodigal), so that the program now
	correctly chooses where to drop pieces at the beginning of
	games on high levels -- thanks, Boris!

Update: Thu May 16 15:16:15 PDT 1991

	Harvey Thompson (harveyt@santa-cruz.co.uk) really did all the
	work for System V portability (fingers crossed that nap(S) is
	actually a standard system call).

Update: Thu Jun  1 08:34:20 BST 2006 (Yes, fifteen years later)

	Ryan Jud Hughes (rjhughes@umich.edu) contributed code and
	documentationfor two nice new features: -g displays "ghost"
	pieces that show where the current piece would fall if
	dropped, and -N shows the next piece in the spare right-side
	area.

(4) The Legal Position
======================

	Look, I'm sorry, I *know* this bit is dull, but it has to be
	done, and it saves time and trouble for everyone if we just
	get on with it.	 here we go:

		The program "tt", its visual appearance, its code, its
		documentation, etc., are the intellectual property of
		Mike Taylor.  The program may be freely distributed,
		copied, modified, re-posted or whatever PROVIDED that
		the authorship and ownership remains clear, and that
		no-one makes any money from it without me knowing (and
		taking a hefty cut!)  Whoops, that's spoilt the
		official feel to it.  Never mind, you get the idea.
		Do what you want provided it isn't sneaky.

(5) The Game
============

	Tetris is one of those simple-but-compulsive games that you
	persistently find yourself wanting to play "just once more".
	I know, I know what you're thinking, "We've all heard that
	before!", but it just happens to be true on this occasion.
	The object of the game is simply to prevent a stack of blocks
	from building up to the top of the play-area, (to the left of
	the screen).  You do this by rotating and shifting the blocks
	as they fall, in such a way as to make them fit together as
	well as possible at the bottom.	 If you manage to complete a
	whole line, from right to left, then that row will disappear,
	and all the rows above will fall down into its place.  it is
	possible to get more than one row at once, and it is a
	wonderful feeling to get four at once!

	There are five types of block, (seven if you count mirror
	images), each of which is made up of four squares stuck
	together, (hence "Tetris", I assume).  These pieces score
	different numbers of points based on how difficult or
	otherwise it is to fit them into the place required.  The
	pieces are:

		  ######	"T-shape"		1 point.
		    ##

		<><><><>	"Long one"		2 points.

		()	{}
		()	{}	"L-shapes"		3 points.
		()()  {}{}

		  [][]		"Square"		4 points.
		  [][]

		%%	@@	"S-shapes"		5 points.
		%%%%  @@@@
		  %%  @@

	Once a piece has been positioned where you want it, it can be
	dropped into place immediately.	 There are no extra points for
	this, 'cos I think it would spoil the purity of the scoring
	system; it just speeds up the game.

	Hehehe, that reminds me, as the game progresses, it very very
	gradually speeds up, until, at scores of around 3000, it gets
	very difficult indeed ...  I mean *very* difficult!

	The keys that control the pieces, and other special keys
	(those used to Quit or Pause the game, or Refresh the screen)
	are shown on the screen.  They can be re-defined (except the
	Refresh key), as explained in the manual page.

(6)  Basic Hints
================

	Obviously, the aim is to keep the stack low, so at all times,
	you should be on the lookout for ways to fit the currently-
	falling piece onto those that have already fallen in such a
	way as to complete rows.  Removing rows is the key to success.

	However, if a lower section of the screen gets hopelessly
	hole-ridden, it is sometimes best to forget about it, and
	concentrate on building complete rows higher up, as this can
	often be the best way to make the holes lower down become
	available again.  A good player can take over from a game 2/3
	full of badly-packed pieces, and eventually wrestle it right
	back down to ground level again.

	Finally, there is a tendency among beginners to leave long,
	thin gaps down the sides of the screen, praying for a "Long
	one" to slide down it -- it is, of course, at precisely these
	times that a succession of S-shapes, all of the same
	handedness, comes pouring down the screen.  The moral is
	two-fold: (1) Beware of allowing such a situation to build up
	in the first place, it often isn't necessary, and (2), learn
	how to remove layers from higher up the screen with non-long
	pieces, so your screen doesn't fill up while you wait for that
	magical "Long one".

(7)  What to do if you don't like it
====================================

	Email me at the following address: mirk@uk.co.ssl
	In fact, email me if you *do* like it.	Email me with
	bouquets, brickbats, bugs, baboons, bachelors, blueberries,
	boathouses, er ... um, sorry, I seem to have got a bit carried
	away.  Anyway, the point is, let me know what you think of
	"tt", and it might just influence the next version.  Also, I
	am pretty good at replying to mail!

	That's it -- have fun!
______________________________________________________________________________
Mike Taylor - {Christ,M{athemat,us}ic}ian ...  Email to: mirk@uk.co.ssl
     "Imagine the universe perfect and whole and beautiful.  Then be sure
	of one thing: God has imagined it quite a bit better than you"
