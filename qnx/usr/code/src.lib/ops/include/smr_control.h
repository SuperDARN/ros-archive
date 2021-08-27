/* smr_control.h
   ============= */

int get_smr(struct task_id *ptr,int *blist,int *pwr);
int put_smr(struct task_id *ptr,int beamno,int *blist,int pwr);

