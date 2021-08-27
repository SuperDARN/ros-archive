/* setup.h
   ======= */

 
struct radar_hardware *setup_hardware();
struct task_id **setup_task(char **tasklist,int *num);
struct fit_block *setup_fit(int id,struct radar_hardware *hdw);
struct freq_table *get_freq_table();
int get_st_id();
 