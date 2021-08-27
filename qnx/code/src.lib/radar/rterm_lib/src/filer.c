/* filer.c
   ======= */

/* 
$Log: filer.c,v $
Revision 1.2  1999/03/25 20:34:13  barnes
Changed header names.

Revision 1.1  1998/06/19 10:43:20  root
Initial revision

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/qnxterm.h>
#include "terminal.h"
#include "filer.h"
#include "filer_def.h"
#include <dirent.h>

#define TRUE 1
#define FALSE 0



int read_directory(char *dirname,char ***list) {

  DIR *dirp;
  struct dirent *direntp;

  dirp=opendir(dirname);
  if (dirp != NULL) { /* correctly opened directory */
      int index;
      if (*list==NULL) {
        if ( (*list=(char **) malloc(sizeof(char *))) == NULL) {
           fprintf(stderr,"fatal error claiming memory - exiting now \n");
           exit(1);
        } 
      } else {
        if ( (*list=(char **) realloc(*list,sizeof(char *))) == NULL) {
           fprintf(stderr,"fatal error claiming memory - exiting now \n");
           exit(1);
        }
      }
      for(index=0;(direntp = readdir(dirp)) != NULL;index++) {

      if (  (*((*list)+index)=(char *) malloc
			  (sizeof(char)*(1+strlen(direntp->d_name))) ) == NULL) {
        fprintf(stderr,"fatal error claiming memory - exiting now \n");
        exit(1);
      }    
      strcpy(*((*list)+index),direntp->d_name);
      if ( ((*list)=(char **) 
					realloc(*list,(2+index)*sizeof(char *)) )== NULL) {
        fprintf(stderr,"fatal error claiming memory - exiting now \n");
        exit(1);
      }    
    }
    *((*list)+index)=NULL;
    closedir(dirp);
    return 1;
  }
  return 0;
}

/* a simple file manager */

int mouse_bot_cursor() {
if ( (term_state.mouse_col >= (term_state.num_cols-FILER_WIDTH)/2+1)
   && (term_state.mouse_col <= (term_state.num_cols+FILER_WIDTH)/2-1) 
   && (term_state.mouse_row >= (term_state.num_rows-FILER_HEIGHT)/2+3)
   && (term_state.mouse_row <= (term_state.num_rows+FILER_HEIGHT)/2-1) )
    return term_state.mouse_row-(term_state.num_rows-FILER_HEIGHT)/2-3;
  return -1;
}

int mouse_top_cursor(char *text) {
if ( (term_state.mouse_row == (term_state.num_rows-FILER_HEIGHT)/2+1)
   && (term_state.mouse_col >= (term_state.num_cols-FILER_WIDTH)/2+1)
   && (term_state.mouse_col <= (term_state.num_cols+FILER_WIDTH)/2-1) )
    return term_state.mouse_col-(term_state.num_cols-FILER_WIDTH)/2-1;
  return -1;
}

int str_insert(char *text,char *str,int offset) {
  if (strlen(text) >= FILER_STRING-1) return FALSE;
  if (offset<strlen(text)) {
    char tmp[FILER_STRING];
    strcpy(tmp,str);
    strcat(tmp,&text[offset]);
    strcpy(&text[offset],tmp);
  } else {
    strcat(text,str);
  }
  return TRUE;
}

int str_overwrite(char *text,char *str,int offset) {
  if (strlen(text) >= FILER_STRING-1) return FALSE;
  if (offset<strlen(text)) {
    char tmp[FILER_STRING];
    strcpy(tmp,str);
    strcat(tmp,&text[offset+strlen(str)]);
    strcpy(&text[offset],tmp);
  } else {
    strcat(text,str);
  }
  return TRUE;
}

int delete(char *text,int offset,int dir) {
  if (strlen(text) == 0) return FALSE;
  if (dir) {
    strcpy(&text[offset],&text[offset+1]);
  } else {
    strcpy(&text[offset-1],&text[offset]);
  } 
  return TRUE;
}

void print_bot_text(char **text,int cursor,int *index) {
 int list;
 term_box_fill( (term_state.num_rows-FILER_HEIGHT)/2+3,
		   (term_state.num_cols-FILER_WIDTH)/2+1,
           FILER_WIDTH-2,FILER_HEIGHT-4,FILER_ATTR,TERM_BOX_NO_FRAME,' ');
 if ((cursor-*index)>=(FILER_HEIGHT-5)) *index=cursor-(FILER_HEIGHT-5);
 if ((cursor-*index)<0) *index=cursor;
 for (list=0;(text[list+*index] != NULL) && 
	 (list < FILER_HEIGHT-4);list++) {
   if (strlen(text[list+*index]) < FILER_WIDTH-2) {
      term_type((term_state.num_rows-FILER_HEIGHT)/2+3+list,
	            (term_state.num_cols-FILER_WIDTH)/2+1,
		        text[list+*index],0,FILER_BOT_ATTR);
   } else {
      term_type((term_state.num_rows-FILER_HEIGHT)/2+3+list,
	            (term_state.num_cols-FILER_WIDTH)/2+1,
		        text[list+*index],FILER_WIDTH-2,FILER_BOT_ATTR);
   }  
  }
  if (strlen(text[cursor]) < FILER_WIDTH-2) {
     term_type((term_state.num_rows-FILER_HEIGHT)/2+3+cursor-*index,
	           (term_state.num_cols-FILER_WIDTH)/2+1,
		       text[cursor],0,FILER_BOT_CRS_ATTR);
  } else {
     term_type((term_state.num_rows-FILER_HEIGHT)/2+3+cursor-*index,
	           (term_state.num_cols-FILER_WIDTH)/2+1,
		       text[cursor],FILER_WIDTH-2,FILER_BOT_CRS_ATTR);
  }
}


void print_top_text(char *text,int cursor,int *index,int insert) {
  /* print the text string in the top box */

  term_box_fill( (term_state.num_rows-FILER_HEIGHT)/2+1,
		   (term_state.num_cols-FILER_WIDTH)/2+1,
           FILER_WIDTH-2,1,FILER_ATTR,TERM_BOX_NO_FRAME,' ');

  if (strlen(text) < (FILER_WIDTH-2) ) { /* no problems with string length */
     *index=0;
     term_type((term_state.num_rows-FILER_HEIGHT)/2+1,
		     (term_state.num_cols-FILER_WIDTH)/2+1,
		     text,0,FILER_TOP_ATTR);
      if (insert) {
        term_type( (term_state.num_rows-FILER_HEIGHT)/2+1,
	        	   (term_state.num_cols-FILER_WIDTH)/2+1+cursor,
		           &text[cursor],1,FILER_TOP_CRS_INS_ATTR);
      } else {
        term_type( (term_state.num_rows-FILER_HEIGHT)/2+1,
	        	   (term_state.num_cols-FILER_WIDTH)/2+1+cursor,
		           &text[cursor],1,FILER_TOP_CRS_OVR_ATTR);
      }   
  } else {
    /* calculate the correct index here */
      if ((cursor-*index)>=(FILER_WIDTH-3)) *index=cursor-(FILER_WIDTH-3);
      if ((cursor-*index)<0) *index=cursor;
      if (strlen(&text[*index])<FILER_WIDTH-2) {
        term_type((term_state.num_rows-FILER_HEIGHT)/2+1,
	    	 (term_state.num_cols-FILER_WIDTH)/2+1,
		     &text[*index],0,FILER_TOP_ATTR);
      } else {
        term_type((term_state.num_rows-FILER_HEIGHT)/2+1,
             (term_state.num_cols-FILER_WIDTH)/2+1,
             &text[*index],FILER_WIDTH-2,FILER_TOP_ATTR);
      }      
      if (insert) { 
        term_type( (term_state.num_rows-FILER_HEIGHT)/2+1,
	        	   (term_state.num_cols-FILER_WIDTH)/2+1+cursor-*index,
		           &text[cursor],1,FILER_TOP_CRS_INS_ATTR);
      } else {
        term_type( (term_state.num_rows-FILER_HEIGHT)/2+1,
    	    	   (term_state.num_cols-FILER_WIDTH)/2+1+cursor-*index,
	    	       &text[cursor],1,FILER_TOP_CRS_OVR_ATTR);
      }
  }
}

void filer_box(char *title) {
  /* draw the filer box on the screen */

  int line;
  term_box_fill( (term_state.num_rows-FILER_HEIGHT)/2,
		   (term_state.num_cols-FILER_WIDTH)/2,
           FILER_WIDTH,FILER_HEIGHT,FILER_ATTR,TERM_BOX_FRAME,' ');
  term_type((term_state.num_rows-FILER_HEIGHT)/2+2,
		   (term_state.num_cols-FILER_WIDTH)/2,
		   &term_state.box_left_tee,1,FILER_ATTR);
  term_type((term_state.num_rows-FILER_HEIGHT)/2+2,
		   (term_state.num_cols+FILER_WIDTH-1)/2,
		   &term_state.box_right_tee,1,FILER_ATTR);
  for(line=(term_state.num_cols-FILER_WIDTH)/2+1;
      line< (term_state.num_cols+FILER_WIDTH-1)/2;
      line++)
    term_type( (term_state.num_rows-FILER_HEIGHT)/2+2,line,
	          &term_state.box_horizontal,1,FILER_ATTR);  
  term_type( (term_state.num_rows-FILER_HEIGHT)/2,
		     (term_state.num_cols-FILER_WIDTH)/2+1,title,0,FILER_ATTR);
}

unsigned filer_handler(char *text,char **list,char *dir_name) {
 
  int edit=1;
  int insert_mode=1;
  int top_index=0;
  int bot_index=0;
  int top_cursor=0;
  int bot_cursor=0;
  unsigned key_cd;
   
  print_top_text(text,top_cursor,&top_index,insert_mode);
  print_bot_text(list,bot_cursor,&bot_index);
  while (edit) { /* allow string editing functions */
    key_cd=term_key();
    if (key_cd == K_UP) { 
      bot_cursor--;
      bot_cursor=(bot_cursor<0) ? 0 : bot_cursor;
      print_bot_text(list,bot_cursor,&bot_index); 
      strcpy(text,dir_name);
      strcat(text,"/");
      strcat(text,list[bot_cursor]);
      top_cursor=strlen(text); /* update the filename here */
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == K_DOWN) {
      bot_cursor++;
      bot_cursor=(list[bot_cursor]==NULL) ? (bot_cursor-1) : bot_cursor;
      print_bot_text(list,bot_cursor,&bot_index); 
      strcpy(text,dir_name);
      strcat(text,"/");
      strcat(text,list[bot_cursor]);
      top_cursor=strlen(text); /* update the filename here */
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == K_CTL_UP) {
      bot_cursor=0;
      print_bot_text(list,bot_cursor,&bot_index); 
      strcpy(text,dir_name);
      strcat(text,"/");
      strcat(text,list[bot_cursor]);
      top_cursor=strlen(text); /* update the filename here */
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == K_CTL_DOWN) {
      int end;
      for (end=0;list[end] != NULL;end++);
      bot_cursor=end-1;
      print_bot_text(list,bot_cursor,&bot_index); 
      strcpy(text,dir_name);
      strcat(text,"/");
      strcat(text,list[bot_cursor]);
      top_cursor=strlen(text); /* update the filename here */
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == K_LEFT) { /* cursor movement */
      top_cursor--;
      top_cursor=(top_cursor<0) ? 0 : top_cursor;
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == K_RIGHT) {
      top_cursor++;
      top_cursor=(top_cursor>strlen(text)) ? strlen(text) : top_cursor;
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == K_CTL_LEFT) {
      top_cursor=0;
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == K_CTL_RIGHT) {
      top_cursor=strlen(text);
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == K_INSERT) {
      insert_mode=! insert_mode;
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == K_DELETE) {
      delete(text,top_cursor,1);
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == K_RUBOUT) {
      if ( (delete(text,top_cursor,0)) && (top_cursor>0) )
         top_cursor--;
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == K_ERASE) {
      strcpy(text,"");
      top_cursor =0;
      print_top_text(text,top_cursor,&top_index,insert_mode);
    } else if (key_cd == 0x0d)  { /* end edit */
      edit=0;
    } else if ( (key_cd >=0x20) && (key_cd <=0x7e) ) { 
     if (insert_mode) {
         if (str_insert(text,(char *) &key_cd,top_cursor)) top_cursor++;
     } else {
         if (str_overwrite(text,(char *) &key_cd,top_cursor)) top_cursor++;
     }
     print_top_text(text,top_cursor,&top_index,insert_mode); 
    } else if (key_cd == K_ESC) {
      strcpy(text,"");
      edit=0;
    } else if (key_cd & K_MOUSE_BSELECT == K_MOUSE_BSELECT) {
     int mse_crs;
     if ((mse_crs=mouse_top_cursor(text))>=0) {
       top_cursor=top_index+mse_crs;
       top_cursor=(top_cursor>strlen(text)) ? strlen(text) : top_cursor;
       print_top_text(text,top_cursor,&top_index,insert_mode);
      } else if ((mse_crs=mouse_bot_cursor())>=0) {
       int end;       
       bot_cursor=bot_index+mse_crs;
       for (end=0;list[end] != NULL;end++);
       bot_cursor=(bot_cursor>=end) ? (end-1) : bot_cursor;
       print_bot_text(list,bot_cursor,&bot_index);
       strcpy(text,dir_name);
       strcat(text,"/");
       strcat(text,list[bot_cursor]);
       top_cursor=strlen(text); /* update the filename here */
       print_top_text(text,top_cursor,&top_index,insert_mode); 
      }
    }
  }
return key_cd;

}

char *leaf_name(char *path) { /* return last bit of string */
  char *leaf_ptr;
  leaf_ptr=strrchr(path,'/');
  return (leaf_ptr != NULL) ? leaf_ptr : path;
}

int filer(char *title,char *path,char *dir_name,char *file_name) {

  char **dirlist;
  int running;
  dirlist=NULL;
  filer_box(title);
  if (strlen(dir_name)==0) { 
    read_directory("/",&dirlist); 
  } else {
    read_directory(dir_name,&dirlist);
  }
  strcpy(path,dir_name);
  strcat(path,"/");
  strcat(path,file_name);
  while (running) {
     filer_handler(path,dirlist,dir_name);
     if (read_directory(path,&dirlist)==1) { /* directory change */
        if (strcmp(leaf_name(path),"/.")==0) {
        strcpy(path,dir_name);
        strcat(path,"/");
        strcat(path,file_name);
        } else if (strcmp(leaf_name(path),"/..")==0) {
        strcpy(leaf_name(path),"\0");       
        strcpy(leaf_name(path),"\0");
        strcpy(dir_name,path);
        strcat(path,"/");
        strcat(path,file_name); 
        } else {
        strcpy(dir_name,path);
        strcat(path,"/");
        strcat(path,file_name);
        }
     } else {
      running=FALSE;
     }
  }
  if (strlen(path) !=0) return TRUE;
  return FALSE;
}