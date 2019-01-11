/*
 * gmp3info.c
 *
 * GTK MP3info's main file. MP3info is an ID3 tag editor.
 *
 * Original Author:
 *       Ricardo Cerqueira      <rmc@rccn.net>
 *
 * This software is covered by the GNU public license, which should be in a file named
 * LICENSE acompanying this.
 *
 */

#include <gtk/gtk.h>
#include "mp3info.h"

gchar *gtext_genre(unsigned char genre);
unsigned char gget_genre (char *genre);

char *typegenre[129] = 
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
           "Dance Hall","Undefined","Undefined",""
}; 

int alphagenreindex[] = {
128,123,74,73,34,99,40,20,26,90,116,41,85,
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

GtkWidget	*id3win,*id3win_frame_global;
GtkWidget	*id3win_frame_title,*id3win_frame_artist,*id3win_frame_album;
GtkWidget	*id3win_frame_year,*id3win_frame_comment, *id3win_frame_albyear;
GtkWidget	*id3win_frame_genre;
GtkWidget	*id3win_text_title,*id3win_text_artist,*id3win_text_album;
GtkWidget	*id3win_text_year,*id3win_text_comment,*id3win_combo_genre;
GtkWidget	*id3win_frame_buttons,*id3win_ok_button,*id3win_cancel_button;
GList 		*genrelist=NULL;
gchar		title[31]="",artist[31]="",album[31]="",year[4]="",comment[31]="",fbuf[4];

char fulltag[128];
unsigned char genre;
FILE  *fp;
unsigned char sig[2];

char *gpad (char *string, int length) {
	while(strlen(string)<length) {
		strcat(string," ");
	}
	return string;
}

void unpad (char *string) {
	int x;
	for (x=(strlen(string)-1); x>=0; x--) {
		if (string[x]!=' ') {
			return;
		} else {
			string[x]='\0';
		}
	}
}

void exit_save( GtkWidget *widget, GtkWidget *data) {
   	char tmp[31];
   	strcpy(fulltag,"TAG");
   	strcpy(tmp,gtk_entry_get_text(GTK_ENTRY(id3win_text_title)));
   	strcat(fulltag,gpad(tmp,30));
   	strcpy(tmp,gtk_entry_get_text(GTK_ENTRY(id3win_text_artist)));
   	strcat(fulltag,gpad(tmp,30));
   	strcpy(tmp,gtk_entry_get_text(GTK_ENTRY(id3win_text_album)));
   	strcat(fulltag,gpad(tmp,30));
   	strcpy(tmp,gtk_entry_get_text(GTK_ENTRY(id3win_text_year)));
   	strcat(fulltag,gpad(tmp,4));
   	strcpy(tmp,gtk_entry_get_text(GTK_ENTRY(id3win_text_comment)));
   	strcat(fulltag,gpad(tmp,30));
        strcpy(tmp,gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(id3win_combo_genre)->entry)));
        fulltag[127]=gget_genre(tmp);
   	fwrite(fulltag,1,128,fp);
   	fclose(fp);
   	gtk_main_quit();
}

int main(int argc, char *argv[] ) {
    
   int i;

   if(argc==2) { 
    if ( !( fp=fopen(argv[1],"rb+") ) ) {
            perror("Error opening MP3");
            return 0;
    }

    fread(sig,sizeof(sig),1,fp);
    sig[0] &= 0xff;                                                          
    sig[1] &= 0xf0;                                                          
    if(!((sig[0] == 0xff) && (sig[1] == 0xf0))) {  
            perror("File is not an MP3!");        
            return 0;
    }

    if ( fseek(fp, -128, SEEK_END )) { return 0; }
    fread(fbuf,1,3,fp); fbuf[3] = '\0';
    if (!strcmp("TAG",fbuf)) {
 	   fseek(fp, -125, SEEK_END); 
 	   fread(title,1,30,fp); unpad(title);
 	   fread(artist,1,30,fp); unpad(artist);
 	   fread(album,1,30,fp); unpad(album);
 	   fread(year,1,4,fp); unpad(year);
 	   fread(comment,1,30,fp); unpad(comment);
	   fread(&genre,1,1,fp);
           fseek(fp, -128, SEEK_END);
    } else {
 	   fseek(fp, 0, SEEK_END); 
    }
  
	gtk_init (&argc, &argv);

	id3win=gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(id3win),"MP3info - An ID3 tag editor");
	gtk_window_set_policy (GTK_WINDOW(id3win), FALSE,FALSE,FALSE);
	gtk_widget_set_usize(id3win,450,340);
	gtk_container_border_width(GTK_CONTAINER(id3win),10);
	gtk_signal_connect(GTK_OBJECT(id3win), "delete_event", (GtkSignalFunc) gtk_exit, NULL);
	id3win_frame_global=gtk_vbox_new(FALSE,10);
	
        id3win_frame_title=gtk_frame_new("Title");
        gtk_container_border_width(GTK_CONTAINER(id3win_frame_title),5);
        id3win_text_title=gtk_entry_new_with_max_length(30);
	gtk_entry_set_text(GTK_ENTRY(id3win_text_title),title);
        gtk_container_add(GTK_CONTAINER(id3win_frame_title),id3win_text_title);
	gtk_box_pack_start(GTK_BOX(id3win_frame_global),id3win_frame_title,TRUE,TRUE,0);

        id3win_frame_artist=gtk_frame_new("Artist");
        gtk_container_border_width(GTK_CONTAINER(id3win_frame_artist),5);
        id3win_text_artist=gtk_entry_new_with_max_length(30);
	gtk_entry_set_text(GTK_ENTRY(id3win_text_artist),artist);
        gtk_container_add(GTK_CONTAINER(id3win_frame_artist),id3win_text_artist);
	gtk_box_pack_start(GTK_BOX(id3win_frame_global),id3win_frame_artist,TRUE,TRUE,0);

	id3win_frame_albyear=gtk_hbox_new(FALSE,30);
        id3win_frame_album=gtk_frame_new("Album");
        gtk_container_border_width(GTK_CONTAINER(id3win_frame_album),5);
        id3win_text_album=gtk_entry_new_with_max_length(30);
	gtk_entry_set_text(GTK_ENTRY(id3win_text_album),album);
        gtk_container_add(GTK_CONTAINER(id3win_frame_album),id3win_text_album);
	gtk_box_pack_start(GTK_BOX(id3win_frame_albyear),id3win_frame_album,TRUE,TRUE,0);

        id3win_frame_year=gtk_frame_new("Year");
	gtk_widget_set_usize(id3win_frame_year,2,0);
        gtk_container_border_width(GTK_CONTAINER(id3win_frame_year),5);
        id3win_text_year=gtk_entry_new_with_max_length(4);
	gtk_entry_set_text(GTK_ENTRY(id3win_text_year),year);
        gtk_container_add(GTK_CONTAINER(id3win_frame_year),id3win_text_year);
	gtk_box_pack_start(GTK_BOX(id3win_frame_albyear),id3win_frame_year,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(id3win_frame_global),id3win_frame_albyear,TRUE,TRUE,0);

        id3win_frame_comment=gtk_frame_new("Comment");
        gtk_container_border_width(GTK_CONTAINER(id3win_frame_comment),5);
        id3win_text_comment=gtk_entry_new_with_max_length(30);
	gtk_entry_set_text(GTK_ENTRY(id3win_text_comment),comment);
        gtk_container_add(GTK_CONTAINER(id3win_frame_comment),id3win_text_comment);
	gtk_box_pack_start(GTK_BOX(id3win_frame_global),id3win_frame_comment,TRUE,TRUE,0);

        id3win_frame_genre=gtk_frame_new("Genre");
        gtk_container_border_width(GTK_CONTAINER(id3win_frame_genre),5);
        id3win_combo_genre=gtk_combo_new();
        for(i=0;i<128;i++) {
		genrelist = g_list_append(genrelist, typegenre[alphagenreindex[i]]);
	}
        gtk_combo_set_popdown_strings(GTK_COMBO(id3win_combo_genre),genrelist);
        gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(id3win_combo_genre)->entry), gtext_genre(genre));
        gtk_container_add(GTK_CONTAINER(id3win_frame_genre),id3win_combo_genre);
	gtk_box_pack_start(GTK_BOX(id3win_frame_global),id3win_frame_genre,TRUE,TRUE,0);

	id3win_frame_buttons=gtk_hbox_new(TRUE,30);
	id3win_ok_button=gtk_button_new_with_label("OK");
        gtk_box_pack_start(GTK_BOX(id3win_frame_buttons),id3win_ok_button,TRUE,TRUE,0);
	id3win_cancel_button=gtk_button_new_with_label("Cancel");
        gtk_box_pack_start(GTK_BOX(id3win_frame_buttons),id3win_cancel_button,TRUE,TRUE,0);
	gtk_widget_set_usize(id3win_frame_buttons,30,20);
	gtk_box_pack_start(GTK_BOX(id3win_frame_global),id3win_frame_buttons,TRUE,TRUE,0);

        gtk_container_add(GTK_CONTAINER(id3win),id3win_frame_global);

	gtk_signal_connect (GTK_OBJECT (id3win_ok_button), "clicked", GTK_SIGNAL_FUNC (exit_save), NULL);
	gtk_signal_connect (GTK_OBJECT (id3win_cancel_button), "clicked", GTK_SIGNAL_FUNC (gtk_exit), NULL);
	
	gtk_widget_show_all(id3win);
	gtk_main();

	return(0);
   } else {
	   g_print("You must supply an MP3 file as an argument!\n");
	   return(0);
   }

			
}

gchar *gtext_genre(unsigned char genre) {
   int genre_num = (int) genre;

   if(genre_num < 127) {
	return((gchar *)typegenre[genre_num]);
   } else {
	return((gchar *)"");
   }
}

unsigned char gget_genre (char *genre) {
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
		num_genre=255;
	}
	return (unsigned char) num_genre;
}


