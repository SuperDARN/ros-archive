/* terminal.c
   ========== */

/* 
$Log: terminal.c,v $
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
#include "terminal_def.h"   
   
#define MENU_MOUSE 1   
   
void draw_menu_item(struct menu_entry *menu,int entry,int cursor) {

 if (menu[entry].select !=0) {
    if (cursor==1) {
    term_printf(menu[entry].row,menu[entry].col,
			    menu[entry].attr_slct_on,menu[entry].text);
    } else {
    term_printf(menu[entry].row,menu[entry].col,
			    menu[entry].attr_slct_off,menu[entry].text);
    }
  } else {
    if (cursor==1) {
    term_printf(menu[entry].row,menu[entry].col,
			    menu[entry].attr_crs_on,menu[entry].text);
    } else {
    term_printf(menu[entry].row,menu[entry].col,
			    menu[entry].attr_crs_off,menu[entry].text);
    }
  }
} 

int draw_menu(struct menu_entry *menu,int cursor) {
  int entry;

  for (entry=0;menu[entry].text !=NULL;entry+=1) {
    if (entry==cursor) { 
       draw_menu_item(menu,entry,1); 
    } else {
       draw_menu_item(menu,entry,0);
    }
  }
  return entry;
}


void setup_mouse() {
 term_mouse_flags(TERM_MOUSE_SELECT | TERM_MOUSE_MOVED | TERM_MOUSE_FOLLOW,
			      TERM_MOUSE_SELECT | TERM_MOUSE_MOVED | TERM_MOUSE_FOLLOW);  
 term_mouse_on();
}

int menu_mouse_cursor(struct menu_entry *menu) {
  int entry;

  for (entry=0;menu[entry].text !=NULL;entry+=1) {
  if ( (term_state.mouse_row==menu[entry].row) &&
       (term_state.mouse_col>=menu[entry].col) &&
       (term_state.mouse_col<=menu[entry].col+strlen(menu[entry].text)) )
       return entry; 
  }
  return -1;
}  

int menu_handler(struct menu_entry *menu,int cursor) {
  int max_entry;
  int loop=1;
  unsigned key_cd;

  max_entry=draw_menu(menu,cursor);

  while (loop) {
    key_cd=term_key();
    if ((key_cd & K_MOUSE_POS) == K_MOUSE_POS) {
       /* mouse position has changed */
       int mouse_cursor;
       mouse_cursor=menu_mouse_cursor(menu);
       if ( (mouse_cursor >=0) && (mouse_cursor != cursor) ) {
          draw_menu_item(menu,cursor,0);
          cursor=mouse_cursor;
          draw_menu_item(menu,mouse_cursor,1);
       }
       if ( ((key_cd & K_MOUSE_BSELECT) == K_MOUSE_BSELECT) 
			 && (cursor == menu_mouse_cursor(menu)) ) {
         if (menu[cursor].type == MENU_WRITE) {
           term_mouse_off();
           if (menu[cursor].len==0) menu[cursor].len=strlen(menu[cursor].text);
           term_field(menu[cursor].row,menu[cursor].col,menu[cursor].text,
		     menu[cursor].len,menu[cursor].text,menu[cursor].attr_slct_on);
           term_mouse_on();
         } else {
           draw_menu_item(menu,cursor,2);
         }
         loop=0;
       }    
    } else if (key_cd==259) {
       draw_menu_item(menu,cursor,0);
       cursor=(cursor == 0) ? (max_entry-1) : (cursor-1);     
       draw_menu_item(menu,cursor,1);
    } else if (key_cd==258) {
        draw_menu_item(menu,cursor,0);
        cursor=(cursor == (max_entry-1) ) ? 0 : (cursor+1);
        draw_menu_item(menu,cursor,1);
    } else if ( (key_cd==13) && (menu[cursor].type == MENU_BUTTON) ) {
      /* a button has been pressed */
      draw_menu_item(menu,cursor,2);
      loop=0;
    } else if (menu[cursor].type == MENU_WRITE) {
       /* field being edited */       
       if (MENU_MOUSE) term_mouse_off();
       term_unkey(key_cd);
       if (menu[cursor].len==0) menu[cursor].len=strlen(menu[cursor].text);
       term_field(menu[cursor].row,menu[cursor].col,menu[cursor].text,
		 menu[cursor].len,menu[cursor].text,menu[cursor].attr_slct_on);
       if (MENU_MOUSE) term_mouse_on();
       loop=0;
    }
  }
  return cursor;
}

void centre_text(int row,unsigned attr,char *text) {
  term_type(row,(term_state.num_cols-strlen(text))/2,text,0,attr);
}

int confirm_prompt(char *text) {

/* prompts for user confirmation returns 0 for yes */

   struct menu_entry menu_prompt[]={
     {0,0,MENU_BUTTON,PROMPT_SLC_ON,PROMPT_SLC_OFF,
     PROMPT_SLC_ON,PROMPT_SLC_OFF,0,0,"<Yes>"},
     {0,0,MENU_BUTTON,PROMPT_SLC_ON,PROMPT_SLC_OFF,
     PROMPT_SLC_ON,PROMPT_SLC_OFF,0,0,"<No >"},
     0
   };
   int cursor=0;
   term_box_fill((term_state.num_rows-4)/2,(term_state.num_cols-strlen(text)-2)/2,
			strlen(text)+2,5,PROMPT_BOX,TERM_BOX_FRAME,' ');
   centre_text(term_state.num_rows/2-1,PROMPT_TEXT,text);
   menu_prompt[0].col=(term_state.num_cols-13)/2;
   menu_prompt[0].row=1+term_state.num_rows/2;
   menu_prompt[1].col=(term_state.num_cols+8)/2;
   menu_prompt[1].row=1+term_state.num_rows/2;
   cursor=menu_handler(menu_prompt,0);
   return cursor;
}

void report_error(char *text) {

/* reports error and waits */

   struct menu_entry menu_prompt[]={
     {0,0,MENU_BUTTON,PROMPT_SLC_ON,PROMPT_SLC_OFF,
     PROMPT_SLC_ON,PROMPT_SLC_OFF,0,0,"<Continue>"},
     0
   };
   term_box_fill((term_state.num_rows-4)/2,(term_state.num_cols-strlen(text)-2)/2,
			strlen(text)+2,5,PROMPT_BOX,TERM_BOX_FRAME,' ');
   centre_text(term_state.num_rows/2-1,PROMPT_TEXT,text);
   menu_prompt[0].col=(term_state.num_cols-10)/2;
   menu_prompt[0].row=1+term_state.num_rows/2;
   menu_handler(menu_prompt,0);
}

void show_message(char *text) {

/* reports message */

  term_box_fill((term_state.num_rows-4)/2,(term_state.num_cols-strlen(text)-2)/2,
			strlen(text)+2,3,PROMPT_BOX,TERM_BOX_FRAME,' ');
  centre_text(term_state.num_rows/2-1,PROMPT_TEXT,text);
}