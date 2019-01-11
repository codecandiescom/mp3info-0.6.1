# Makefile for MP3info
#	Ricardo Cerqueira, 1999
#


PROG =	mp3info
SRCS =	mp3info.c textfunc.c mp3curs.c
OBJS =  mp3info.o textfunc.o mp3curs.o
XSRC =	gmp3info.c
RM = /bin/rm
INSTALL = /usr/bin/install -c
STRIP = strip

prefix=/usr/local
bindir=${prefix}/bin


LIBS = -lncurses
CC = gcc
CFLAGS = -g -O2 -Wall

all: mp3info gmp3info

mp3info: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

gmp3info: $(XSRC)
	$(CC) $(XSRC) $(CFLAGS) -o $@ `gtk-config --cflags --libs`

clean: 
	$(RM) -f $(OBJS) $(XOBJ) mp3info gmp3info

install: 
	$(STRIP) mp3info
	$(INSTALL) mp3info $(bindir)/mp3info
	$(STRIP) gmp3info
	$(INSTALL) gmp3info $(bindir)/gmp3info
