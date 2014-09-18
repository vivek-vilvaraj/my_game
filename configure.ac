
AC_INIT(asteroids3D, 0.5.1)
AC_CONFIG_HEADERS(config.h)
AC_PROG_INSTALL
AM_INIT_AUTOMAKE
AC_PROG_CC
AM_PROG_CC_C_O

AC_ARG_WITH([gamesdir], AS_HELP_STRING([--with-gamesdir=PATH],
	[Path to game binary directory [[EPREFIX/games]]]),
	[gamesdir="$withval"], [gamesdir='${exec_prefix}/games'])
AC_ARG_WITH([gamedatadir], AS_HELP_STRING([--with-gamedatadir=PATH],
	[Path to textures and sounds [[DATADIR/games]]]),
	[gamedatadir="$withval"], [gamedatadir='${exec_prefix}/games'])
AC_SUBST(gamesdir)
AC_SUBST(gamedatadir)

regular_CFLAGS="-D_LARGEFILE_SOURCE=1 -D_LARGE_FILES -D_FILE_OFFSET_BITS=64 \
	-D_REENTRANT -Wall -Waggregate-return -Wmissing-declarations \
	-Wmissing-prototypes -Wredundant-decls -Wshadow -Wstrict-prototypes \
	-Winline -pipe"
AC_SUBST(regular_CFLAGS)
AC_OUTPUT([Makefile])
