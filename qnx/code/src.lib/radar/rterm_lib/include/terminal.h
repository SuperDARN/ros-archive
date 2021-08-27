/* menu system for terminal */


#ifndef _UTIL_TERMINAL

#include <sys/qnxterm.h>


enum menu_type {
  MENU_BUTTON,
  MENU_WRITE
};

struct menu_entry {
  int row,col;
  enum menu_type type;  /* type of menu entry */
  unsigned attr_slct_on;
  unsigned attr_slct_off;  
  unsigned attr_crs_on;
  unsigned attr_crs_off;
  int select;
  int len;
  char *text;
};

void draw_menu_item(struct menu_entry *,int,int);
int draw_menu(struct menu_entry *,int);
void setup_mouse();
int menu_handler(struct menu_entry *,int);
void centre_text(int,unsigned,char *);
int confirm_prompt(char *);
void report_error(char *);
void show_message(char *);

#define _UTIL_TERMINAL
#endif