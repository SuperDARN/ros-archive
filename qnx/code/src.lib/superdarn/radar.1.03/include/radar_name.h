/* radar_name.h
   ============ */

#ifndef _RADAR_NAME_H
#define _RADAR_NAME_H

struct radar_name {
  int id;
  unsigned char code;
  int status;
  char name[64];
  char operator[128];
};

struct radar_id {
  int num;
  struct radar_name *radar;
};

struct radar_id *load_radar(FILE *fp);
char *radar_name(struct radar_id *ptr,int st_id);
char *radar_operator(struct radar_id *ptr,int st_id);
char radar_code(struct radar_id *ptr,int st_id);
int radar_id(struct radar_id *ptr,char code);
int radar_status(struct radar_id *ptr,int st_id);

#endif



