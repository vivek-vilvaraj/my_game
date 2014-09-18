# -*- Makefile -*-

AUTOMAKE_OPTIONS    = foreign subdir-objects
AM_CFLAGS           = $(regular_CFLAGS) -DDATADIR='"$(gamedatadir)"'

games_PROGRAMS      = asteroids3D
asteroids3D_SOURCES = src/asteroid.c src/calc_frame.c src/collision.c \
                      src/debug.c src/display.c src/dust.c src/explosion.c \
                      src/hud.c src/keyb.c src/light.c src/main.c src/move.c \
                      src/oglutils.c src/pause.c src/rcfile.c src/score.c \
                      src/shield.c src/sound.c src/split.c src/sun.c \
                      src/target.c src/texture.c src/thrust.c src/torpedo.c \
                      src/vecops.c src/view.c src/wind_part.c
asteroids3D_LDADD   = -lGL -lGLU -lglut -lm -lpthread

gamedata_DATA       = $(wildcard src/*.ppm src/*.ub)

.PHONY: tarball
tarball:
	rm -Rf /tmp/${PACKAGE_NAME}-${PACKAGE_VERSION};
	pushd ${top_srcdir} && git-archive --prefix=${PACKAGE_NAME}-${PACKAGE_VERSION}/ HEAD | tar -C /tmp -x && popd;
	pushd /tmp/${PACKAGE_NAME}-${PACKAGE_VERSION} && ./autogen.sh && popd;
	tar -C /tmp -cjf ${PACKAGE_NAME}-${PACKAGE_VERSION}.tar.bz2 --owner=root --group=root ${PACKAGE_NAME}-${PACKAGE_VERSION}/;
	rm -Rf /tmp/${PACKAGE_NAME}-${PACKAGE_VERSION};
