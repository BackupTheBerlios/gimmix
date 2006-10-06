# Makefile for gimmix

# Installation directory
PREFIX=/usr/local

ALLFILES=*.c

CONFUSE_LIB=`pkg-config libconfuse --libs`
GTK_CFLAGS=`pkg-config gtk+-2.0 --cflags`
GTK_LIB=`pkg-config gtk+-2.0 --libs`
GLADE_CFLAGS=`pkg-config libglade-2.0 --cflags`
GLADE_LIB=`pkg-config libglade-2.0 --libs`
MPD_CFLAGS=`pkg-config libmpd --cflags`
MPD_LIB=`pkg-config libmpd --libs`
GLIB_CFLAGS=`pkg-config --cflags glib-2.0 glib`
GLIB_LIB=`pkg-config --libs glib-2.0 glib`

CFLAGS=-march=i686 -pipe -O2 -Wall -fPIC $(GTK_CFLAGS) $(GLADE_CFLAGS) $(MPD_CFLAGS) $(GLIB_CFLAGS) -I.
LIBS=$(CONFUSE_LIB) $(GTK_LIB) $(GLADE_LIB) $(MPD_LIB) $(GLIB_LIB)

OBJS = config.o gimmixcore.o interface.o gimmix.o

CC=gcc

projname := gimmix

all: gimmix

gimmix: $(OBJS)
	$(CC) $(OBJS) -o gimmix $(LIBS)

clean:
	rm -f *.o core *.so* *.bak *~ src/*~ gimmix

config.o:			src/config.c
	$(CC) $(CFLAGS) -c src/config.c
#prefs.o:		src/prefs.c
#	$(CC) $(CFLAGS) -c src/prefs.c
gimmixcore.o:		src/gimmixcore.c
	$(CC) $(CFLAGS) -c src/gimmixcore.c
interface.o:		src/interface.c
	$(CC) $(CFLAGS) -c src/interface.c
#playlist.o:		src/playlist.c
#	$(CC) $(CFLAGS) -c src/playlist.c
gimmix.o:			src/gimmix.c
	$(CC) $(CFLAGS) -c src/gimmix.c
	
install:
	install -D gimmix $(PREFIX)/bin/gimmix
#	install -D -m644 gimmix.png $(PREFIX)/share/pixmaps/
