# Makefile for "tt" - Tetris for Terminals (ie. not just workstations)
# Created Wed Feb  6 16:33:20 GMT 1991 by Mike Taylor.

SRC = tt.c utils.c screen.c pieces3.c pieces4.c pieces5.c pieces7.c game.c
OBJ = $(SRC:.c=.o)
LINTFLAGS = -abh

# --- Choose one of these CFLAGS ---
CFLAGS = $(OPT)					# Sun BSD, Red Hat Linux
#CFLAGS = $(OPT) -DSYSV				# SCO System V
#CFLAGS = $(OPT) -DSYSV -DNO_NAP_SYSCALL	# Generic System V?
#CFLAGS = $(OPT) -DNO_USLEEP_SYSCALL		# Generic BSD?

# --- Choose one of these LDLIBS ---
LDLIBS = -lcurses -ltermcap 			# Sun and Generic BSD
#LDLIBS = -lcurses -lx				# SCO System V
#LBLIBS = -lcurses				# Generic System V?

tt: $(OBJ); $(CC) $(LDFLAGS) $(CFLAGS) $(OBJ) -o tt $(LDLIBS)
lint:; lint $(LINTFLAGS) $(SRC)
tags: TAGS
TAGS: $(SRC); etags *.c
clean:; rm -f $(OBJ) tt a.out core

game.o:  tt.h game.h screen.h pieces.h utils.h
pieces.o:  tt.h pieces.h
screen.o:  screen.h tt.h pieces.h utils.h game.h
tt.o:  tt.h utils.h screen.h game.h
utils.o:  tt.h utils.h screen.h
