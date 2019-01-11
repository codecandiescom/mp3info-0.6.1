/*
 * textfunc.h
 *
 * Original Author:
 *       Ricardo Cerqueira      <rmc@rccn.net>
 *
 * 
 * This software is covered by the GNU public license, which should be in a file named
 * LICENSE acompanying this.
 *
 */

#define CTRL_C		'\003'
#define TEXT_FIELD_LEN	30
#define INT_FIELD_LEN	4

struct id3tag {
	char title[31];
	char artist[31];
	char album[31];
	char year[5];
	char comment[31];
	unsigned char genre[1];
};

void CenterText(int line, char *text);
char *checkinput(int c, char *string, int length);
void pad(char *string, int length);
char *curs_addparam(WINDOW *win, int line, int length, char *buf);
void buildtag(char *buf, struct id3tag *tag);
void display_help();
void view_tag(char *filename,struct id3tag *tag);
void display_genres(void);
unsigned int get_genre (char *genre);


