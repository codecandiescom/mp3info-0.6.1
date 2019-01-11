/*
 * mp3info.c
 *
 * MP3info's main file. MP3info is an ID3 tag editor.
 *
 * Original Author:
 *       Ricardo Cerqueira      <rmc@rccn.net>
 *
 * This software is covered by the GNU public license, which should be in a file named
 * LICENSE acompanying this.
 *
 */

#include "mp3info.h"
#include <sys/stat.h>

char *typegenre [128] = 
           { 
           "Blues","Classic Rock","Country","Dance","Disco", 
           "Funk","Grunge","Hip-Hop","Jazz","Metal", 
           "New Age","Oldies","Other","Pop","R&B", 
           "Rap","Reggae","Rock","Techno","Industrial", 
           "Alternative","Ska","Death Metal","Pranks","Soundtrack", 
           "Euro-Techno","Ambient","Trip-Hop","Vocal","Jazz+Funk", 
           "Fusion","Trance","Classical","Instrumental","Acid", 
           "House","Game","Sound Clip","Gospel","Noise", 
           "AlternRock","Bass","Soul","Punk","Space", 
           "Meditative","Instrumental Pop","Instrumental Rock","Ethnic","Gothic", 
           "Darkwave","Techno-Industrial","Electronic","Pop-Folk","Eurodance", 
           "Dream","Southern Rock","Comedy","Cult","Gangsta", 
           "Top 40","Christian Rap","Pop/Funk","Jungle","Native American", 
           "Cabaret","New Wave","Psychadelic","Rave","Showtunes", 
           "Trailer","Lo-Fi","Tribal","Acid Punk","Acid Jazz", 
           "Polka","Retro","Musical","Rock & Roll","Hard Rock", 
           "Folk","Folk-Rock","National Folk","Swing","Fast Fusion", 
           "Bebob","Latin","Revival","Celtic","Bluegrass", 
           "Avantgarde","Gothic Rock","Progressive Rock","Psychedelic Rock","Symphonic Rock", 
           "Slow Rock","Big Band","Chorus","Easy Listening","Acoustic", 
           "Humour","Speech","Chanson","Opera","Chamber Music", 
           "Sonata","Symphony","Booty Bass","Primus","Porn Groove", 
           "Satire","Slow Jam","Club","Tango","Samba", 
           "Folklore","Ballad","Power Ballad","Rhythmic Soul","Freestyle", 
           "Duet","Punk Rock","Drum Solo","Acapella","Euro-House", 
           "Dance Hall","Undefined","Undefined"
}; 




int main(int argc, char *argv[]) {
FILE  *fp;
int c, i, interactive = 0, view_only=1,delete_tag=0,file_open,retcode=0,has_tag=0;
int new_genre=0,firstfilearg;
struct id3tag tag,new_tag;
char fullline[129], fbuf[4];
char error_msg[256];
unsigned char sig[2];
unsigned int g;
struct stat filestat;


   new_tag.title[0]=new_tag.artist[0]=new_tag.album[0]=new_tag.year[0]=
   new_tag.comment[0]=new_tag.genre[0]=0;


   if (argc < 2 ) /* Only command is given. Short help */ {
	   printf("%s\r\n  Usage: %s [-h | -G] (-i | -d | -t title -a artist -l album -y year 
	-c comment -g genre) MP3_FILE [ MP3_FILE [ ... ]]\r\n",VERSION,argv[0]); 
	   return 0;
   }


   while ((c=getopt(argc,argv,"vhGidt:a:l:y:c:g:"))!=-1) {

	switch(c) {
		case 'v': /* View mode automatic when no changes made to ID3 tag */
			break;
		case 'h':
			display_help(); return 0;
			break;
		case 'G':
			display_genres(); return 0;
			break;
		case 'i':
			view_only=0;
			interactive = 1;
			break;
		case 'd':
			view_only=0;
			delete_tag=1;
			break;
		case 't':
			strncpy(new_tag.title,optarg,TEXT_FIELD_LEN);
			view_only=0;
			break;
		case 'a':
			strncpy(new_tag.artist,optarg,TEXT_FIELD_LEN);
			view_only=0;
			break;
		case 'l':
			strncpy(new_tag.album,optarg,TEXT_FIELD_LEN);
			view_only=0;
			break;
		case 'y':
			strncpy(new_tag.year,optarg,INT_FIELD_LEN);
			view_only=0;
			break;
		case 'c':
			strncpy(new_tag.comment,optarg,TEXT_FIELD_LEN);
			view_only=0;
			break;
		case 'g':
			g=get_genre(optarg);
			if(g <= 255) {
				new_tag.genre[0] = (unsigned char) g;
				new_genre=1;
				view_only=0;
			} else {
				fprintf(stderr,"Error: '%s' is not a recognized genre name or number.\n",optarg);
				fprintf(stderr,"Use the '-G' option to see a list of valid genre names and numbers\n");
				retcode |= 6;
				return retcode;
			}
			sscanf(optarg,"%u",&g);
			break;
	}
   }

   firstfilearg=optind;

   for(i=optind;i < argc; i++) { /* Iterate over all filenames */
      file_open=0;
      if (view_only == 1) { 
        if ( !( fp=fopen(argv[i],"r") ) ) {
  	        sprintf(error_msg,"Error opening MP3: %s",argv[i]);
                perror(error_msg);
		retcode |= 1;
        } else {
		file_open=1;
	}
      } else {
        if ( !( fp=fopen(argv[i],"rb+") ) ) {
  	        sprintf(error_msg,"Error opening MP3: %s",argv[i]);
                perror(error_msg);
	        retcode |= 1;
        } else {
		file_open=1;
	}
      }

      if(file_open == 1) {

         /* Let's see if it's an MP3. SMJ 06/1999 */  
 
         fread(sig,sizeof(sig),1,fp);                         
         sig[0] &= 0xff;
         sig[1] &= 0xf0;                                                         
         if(!((sig[0] == 0xff) && (sig[1] == 0xf0))) {
                 fprintf(stderr,"%s is not an MP3 file!\n",argv[i]);        
                 retcode |= 2;
         } else { /* Yup, it's an MP3 */

            /* Go to EOF and check if there's already a tag */

            if ( fseek(fp, -128, SEEK_END )) { 
		retcode |= 4;
	    } else {
               fread(fbuf,1,3,fp); fbuf[3] = '\0';
   	       tag.title[0]=tag.artist[0]=tag.album[0]=tag.year[0]=tag.comment[0]=0;
	       tag.genre[0]=255;
               if (!strcmp("TAG",fbuf)) {
            	   /* Holy cow, Batman! I found something! Better save it for later use... */
		   has_tag=1;
            	   fseek(fp, -125, SEEK_END); 

            	   fread(tag.title,1,30,fp); tag.title[30] = '\0';
            	   fread(tag.artist,1,30,fp); tag.artist[30] = '\0';
            	   fread(tag.album,1,30,fp); tag.album[30] = '\0';
            	   fread(tag.year,1,4,fp); tag.year[4] = '\0';
            	   fread(tag.comment,1,30,fp); tag.comment[30] = '\0';
            	   fread(tag.genre,1,1,fp);
            	   /* freads moved the file pointer. Get it back to the beginning of the tag so it 	*
            	    * will be overwritten 								*/
                   fseek(fp, -128, SEEK_END);
               } else {
            	   /* No tags here. Back to the EOF to append the new one */
		   has_tag=0;
            	   fseek(fp, 0, SEEK_END); 
               }

	       if(view_only) {
	          if(has_tag) {
		     view_tag(argv[i],&tag);
                  } else {
   	       	     printf("No ID3 found in MP3 file %s\n",argv[i]);
                  }
   
	       } else {

	          if(new_tag.title[0]) {
			strncpy(tag.title,new_tag.title,TEXT_FIELD_LEN);
	          }

	          if(new_tag.artist[0]) {
			strncpy(tag.artist,new_tag.artist,TEXT_FIELD_LEN);
	          }

	          if(new_tag.album[0]) {
			strncpy(tag.album,new_tag.album,TEXT_FIELD_LEN);
	          }

	          if(new_tag.comment[0]) {
			strncpy(tag.comment,new_tag.comment,TEXT_FIELD_LEN);
	          }

	          if(new_tag.year[0]) {
			strncpy(tag.year,new_tag.year,INT_FIELD_LEN);
	          }

	          if(new_genre) {
			tag.genre[0]=new_tag.genre[0];
	          }
  		  
		  if( interactive ) {
	 	     tagedit_curs(argv[i],i-firstfilearg+1,argc-firstfilearg,&tag);
		  }
                  
                  buildtag(fullline,&tag);
               
                  /* Finally! Get it done! */
                  if(!delete_tag) {
			fwrite(fullline,1,128,fp);
                  }

	       }
            }

	 }                                                    

         fclose(fp);
         if(delete_tag && has_tag) {
		stat(argv[i],&filestat);
		truncate(argv[i],filestat.st_size-128);
	 }
      }



   }

   if(optind == argc) {
	fprintf(stderr,"No MP3 files specified!\n");
	retcode |= 8;
   }

   return retcode;

}



