/*
 * textfunc.c
 * 
 * Auxilliary text mode functions for MP3info
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

int alphagenreindex[] = {
123,74,73,34,99,40,20,26,90,116,41,85,
96,89,0,107,65,88,104,102,97,61,1,32,
112,57,2,58,125,3,50,22,4,55,122,120,
98,52,48,124,25,54,84,81,115,80,119,5,
30,36,59,38,91,49,6,79,7,35,100,19,
46,47,33,29,8,63,86,71,45,9,77,82,
64,10,66,39,11,103,12,75,53,62,13,109,
117,23,108,92,67,93,121,43,14,15,68,16,
76,87,118,78,17,114,110,69,21,111,95,105,
42,37,24,56,44,101,83,94,106,113,51,18,
60,70,31,72,27,126,28
};

/*
 * Name of this one is quite obvious...
 */

void CenterText(int line, char *text) {
   mvprintw(line,(COLS/2)-((int)strlen(text)/2),text);
}

/*
 * If "string" is shorter than "length", pad it with ' ' (spaces)
 */

void pad(char *string, int length) {
        int l;

        l=strlen(string);
        while(l<length) {
                string[l] = ' ';
                l++;
        }

        string[l]='\0';
}

/*
 * Build the final tag, ready to be inserted into the file
 */

void buildtag(char *buf, struct id3tag *tag) {

	strcpy(buf,"TAG");
	pad(tag->title,TEXT_FIELD_LEN); 
	strncat(buf,tag->title,TEXT_FIELD_LEN);
	pad(tag->artist,TEXT_FIELD_LEN); 
	strncat(buf,tag->artist,TEXT_FIELD_LEN);
	pad(tag->album,TEXT_FIELD_LEN); 
	strncat(buf,tag->album,TEXT_FIELD_LEN);
	pad(tag->year,INT_FIELD_LEN); 
	strncat(buf,tag->year,INT_FIELD_LEN);
	pad(tag->comment,TEXT_FIELD_LEN); 
	strncat(buf,tag->comment,TEXT_FIELD_LEN);
	strncat(buf,tag->genre,1);
}

/*
 * Another one with an obvious name :-)
 */

void display_help() {

	printf("%s\n\n",VERSION);
	printf("  Usage: mp3info [-h | -G] (-i | -d | -t title -a artist -l album -y year 
	-c comment -g genre) MP3_FILE [ MP3_FILE [ ... ]]\n\n");
	printf("-h	--	Display this info and exit\n");
	printf("-G	--	Display genre list and exit\n");
	printf("-v	--	Obsolete (used to be  view tag and exit)\n");
	printf("-i	--	Edit ID3 tag interactively\n");
	printf("-d	--	Delete ID3 tag\n\n");
	printf("  Specify ID3 tag fields:\n\n");
	printf("-t title   --   Track title\n");
	printf("-a artist  --   Artist name\n");
	printf("-l album   --   Album name\n");
	printf("-y year    --   Copyright Year\n");
	printf("-c comment --   Comment\n");
	printf("-g genre   --   Genre (use -G option for a list of valid genres)\n\n");
	printf("Non-specified fields, if existant, will remain unchanged.\n");
	printf("Genres can be specified as numbers or names: '-g 17' same as '-g Rock'\n");
	printf("Remember: Multiple word fields must be enclosed in \"\" (eg: -t \"A title\")\n\n");


}

/*
 * Display an existing tag (if any), or error message
 */

void view_tag(char *filename,struct id3tag *tag) {
	   char genre[40]="";

	   printf("File: %s\n",filename);
	   printf("Title:   %s\n",tag->title);
	   printf("Artist:  %s\n",tag->artist);
	   printf("Album:   %-30s ",tag->album);
	   printf("Year: %s\n",tag->year);
	   printf("Comment: %-30s ",tag->comment);
           text_genre(tag->genre,genre);
	   printf("Genre: %s\n\n",genre);

}

void display_genres(void) {
	int i;
	printf("MP3 ID3 Genres\n==============\n");
	for(i=0;i<42;i++) {
		printf("%3d %-22s %3d %-22s %3d %-22s\n",alphagenreindex[i],
		typegenre[alphagenreindex[i]],alphagenreindex[i+42],
		typegenre[alphagenreindex[i+42]],alphagenreindex[i+84],
		typegenre[alphagenreindex[i+84]]);
	}
}

unsigned int get_genre (char *genre) {
	int num_genre=0;

        if(genre[0] == '\0') { return 255; }

	sscanf(genre,"%u",&num_genre);
	if(num_genre == 0) {
		if(genre[0] != '0') {
			while(num_genre++ < 128) {
				if(!strcasecmp(genre,typegenre[num_genre-1])) {
					return num_genre-1;
				}
			}
			num_genre=256;
		}		
	}

	if(num_genre < 0 || num_genre > 126) {
		num_genre=256;
	}
	return num_genre;
}

void text_genre(unsigned char *genre,char *buffer) {
   int genre_num = (int) genre[0];

   if(genre_num < 127) {
	sprintf(buffer,"%s [%d]",typegenre[genre_num],genre_num);
   } else if(genre_num < 255) {
	sprintf(buffer,"(UNKNOWN) [%d]",genre_num);
   } else {
	buffer[0]='\0';
   }
}



