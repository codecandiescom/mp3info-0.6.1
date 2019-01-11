/*
 *  mp3info.h
 *  
 * Original Author:
 *       Ricardo Cerqueira      <rmc@rccn.net>
 *    
 *  This software is covered by the GNU public license, which should be in a file named
 *  LICENSE acompanying this.
 *  
 *          
 */
 
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <curses.h>
#include <signal.h>
#include "textfunc.h"

#define VERSION "MP3info - v0.6.1"

void tagedit_curs(char *filename, int filenum, int fileoutof, struct id3tag *tag);
void text_genre(unsigned char *genre,char *buffer);
