/* radar_shell.h
   =========== */

#ifndef _RADAR_SHELL
#define _RADAR_SHELL
#define STR_MAX 128 
enum {  /* variable types */
  var_SHORT,
  var_INT,
  var_LONG,
  var_FLOAT,
  var_DOUBLE,
  var_STRING
};

struct rsentry {
  char *name; 
  int type;
  void *data;
};

struct rstable {
  int num;
  struct rsentry *ptr;
};

pid_t register_program(char *,char *);
int rs_shell(pid_t schedule_id,struct rsentry *ptr);
int radar_shell(pid_t schedule_id,struct rstable *ptr);
int radar_shell_add(struct rstable *ptr,char *name,int type,void *data);
int radar_shell_parse(struct rstable *ptr,char *name,...);
void radar_shell_free(struct rstable *ptr);
struct rsentry *radar_shell_read(struct rstable *ptr,int num);
int poll_exit(pid_t schedule_id);
#endif


