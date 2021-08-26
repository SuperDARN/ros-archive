/* a_d_drive.h
   =========== */

#ifndef _AD_DRIVER
#define _AD_DRIVER


#define DRV_STAT_UNKNOWN 0
#define DRV_ERROR 'E'
#define GET_BUF_NUM 'n'
#define GET_BUF_SIZE 's'
#define GET_BUF_ADR 'a'
#define GET_SCAN_STATUS 'S'
#define DO_SCAN 'd'
#define GET_PROXY_PID 'p'

#define SCAN_OK 0
#define SCAN_ERROR 'F'

int get_buf_num(pid_t task_id);
int get_buf_size(pid_t task_id);
void *get_buf_adr(pid_t task_id,int buffer);
int get_scan_status(pid_t task_id);
int do_scan(pid_t task_id,int buffer,int bytes,
			int mode,int channels);
pid_t get_scan_reset(pid_t task_id);
int scan_reset(pid_t task_id,pid_t proxy_id);

#endif