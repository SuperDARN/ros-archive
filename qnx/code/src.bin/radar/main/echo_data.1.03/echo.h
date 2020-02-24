/* echo.h
   ======
   Author: R.J.Barnes
*/

/* structure for the attached task list */

struct task_list {
  pid_t task_id;
  int plen;
  char *name;
  char *pass;
  struct task_list *next;
};
