/* data structures for my scheduler routine */

#define SCHED_LINE_LENGTH 1024
#define SCHED_MAX_ENTRIES 1024
#define DELIM " \t\n"

/* each entry consists of time at which a command starts 
   + the load command */

struct scd_entry {
  double stime;
  char command[SCHED_LINE_LENGTH];
};

struct scd_blk {
  char name[256];
  int num; /* number of scheduled events */
  int cnt;
  char path[SCHED_LINE_LENGTH];
  struct scd_entry entry[SCHED_MAX_ENTRIES];
  char command[SCHED_LINE_LENGTH];
  int refresh;
  pid_t pid;
}; 
 
  
  
