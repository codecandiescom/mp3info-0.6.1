/*
 * mp3curs.c
 *
 * Curses functions for MP3info
 *
 * Original Author:
 *       Ricardo Cerqueira      <rmc@rccn.net>
 *
 * This software is covered by the GNU public license, which should be in a file named
 * LICENSE acompanying this.
 *
 */

#include "mp3info.h"

extern char *typegenre [128];


/*
 * Check if Break is given, and if so, clean term and exit, else pad the input.
 */

char *checkinput(int c, char *string, int length) {
        if (c == CTRL_C) { endwin(); exit(0); }
        return string;
}

/*
 * Get the cursor to the right place, and get input
 */

char *curs_addparam(WINDOW *win, int line, int length, char *buf) {
        int c;
        char string[TEXT_FIELD_LEN];

        wmove(win,line,16);
        c = wgetnstr(win,string,length);
        strcpy(buf,checkinput(c,string,length));
        return buf;
}

/*
 * Bring up the curses window, then get and parse input, and build the tag.
 */

void tagedit_curs(char *filename, int filenum, int fileoutof, struct id3tag *tag) {
WINDOW *mainwin;
   char line[50], *genre, genre_text[30];
   unsigned int genre_num;
   line[0]='\0';

   initscr(); cbreak(); noecho();
   nonl();
   mainwin = newwin(8,COLS,5,0);
   intrflush(mainwin, FALSE);
   keypad(mainwin, TRUE);
   CenterText(1,VERSION);
   CenterText(2,"Ricardo Cerqueira <rmc@rccn.net>, 2000");
#ifdef HAVE_TOUCHWIN
   touchwin(mainwin);
#endif
   box(mainwin, 0, 0);
   wmove(mainwin,1,1); 
   wprintw(mainwin,"Song Title:    %s",tag->title);
   wmove(mainwin,2,1); 
   wprintw(mainwin,"Artist Name:   %s",tag->artist);
   wmove(mainwin,3,1); 
   wprintw(mainwin,"Album Name:    %s",tag->album);
   wmove(mainwin,4,1); 
   wprintw(mainwin,"Year:          %s",tag->year);
   wmove(mainwin,5,1); 
   wprintw(mainwin,"Comment:       %s",tag->comment);
   wmove(mainwin,6,1);
   if(tag->genre[0] < 127) {
	genre=typegenre[tag->genre[0]];
   } else {
	genre="";
   }
   strcpy(genre_text,genre);
   wprintw(mainwin,"Genre:         %s",genre);
   wmove(mainwin,7,4);
   wprintw(mainwin," (%d/%d) %s ",filenum,fileoutof,filename);
   wmove(mainwin,7,COLS-22);
   wprintw(mainwin," Press ^C to quit ");
   refresh();
   echo();
   curs_addparam(mainwin,1,30,line);
   strncpy(tag->title,line,strlen(line));
   curs_addparam(mainwin,2,30,line);
   strncpy(tag->artist,line,strlen(line));
   curs_addparam(mainwin,3,30,line);
   strncpy(tag->album,line,strlen(line));
   curs_addparam(mainwin,4,4,line);
   strncpy(tag->year,line,strlen(line));
   curs_addparam(mainwin,5,30,line);
   strncpy(tag->comment,line,strlen(line));
   curs_addparam(mainwin,6,30,line);
   strncpy(genre_text,line,strlen(line) + (strlen(line) < strlen (genre_text) ? 0 : 1));
   endwin();
   if((genre_num=get_genre(genre_text)) < 256) {
	tag->genre[0] = (unsigned char) genre_num;
   } else {
	printf("ERROR - '%s' is not a valid genre name or number.\nUse '-G' for a list of valid genres.\n",genre_text);
   }

}

