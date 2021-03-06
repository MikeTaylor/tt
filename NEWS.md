# Change history for tt

## Release 16.0 (IN PROGRESS)

* Better default paths for `LOCK_FILE` and `SCORE_FILE`.
* Rework top-level `README` into Markdown.
* Rework this `NEWS` file into Markdown.
* Bug-fix: sometimes pieces were being erased in the location where a ghost-piece _would_ fall if `-g` was on. Thanks to Madhu <enometh@meer.net>.

## Release 15.1 (Thu Jun 18 16:23:14 BST 2009)

* Replace C++-style `//`-comments with C-style `/*`-comments.
* Replace `false` with 0.
* I think this is now once more good C.

## Release 15.0 (Mon Jun  5 11:55:51 BST 2006)

* New `-g` option shows "ghost" pieces indicating where the current piece would fall if dropped.  Contributed by Ryan Jud Hughes <rjhughes@umich.edu>
* New `-N` option shows what the next piece will be.  Also contributed by Ryan Jud Hughes <rjhughes@umich.edu>

## Release 14.1 (Tue Oct 18 18:14:56 BST 2005)

* The `form()` function in `form.c` (declared in `form.h`) now uses ANSI-style `<stdarg.h>` instead of old-style `<varargs.h>`, as the latter is no longer supported on modern Linux systems such as Ubuntu "Hoary Hedgehog".
* `util.c` now includes `<term.h>` to avoid a compiler warning when `tgetstr()` is used undeclared.
* `screen.c` now includes `<string.h>` to avoid a compiler warning when `strlen()` is used undeclared.
* New material on the end of the `Testimonial` file.

## Release 14.0 (Wed May 28 15:24:45 BST 2003)

* Pretty much as written in 1989.

