/* test_radar.c  
   ============ */

/* 
 $Log: test_radar.c,v $
 Revision 1.8  1999/04/10 20:25:22  barnes
 Added help message system.

 Revision 1.7  1999/04/07 21:36:14  barnes
 Changed name of the dio driver task.

 Revision 1.6  1999/04/02 18:18:12  barnes
 Fixed problem with reading extended timing sequences.

 Revision 1.5  1999/03/30 19:12:36  barnes
 Moved task names into a header.

 Revision 1.4  1998/11/09 16:28:38  barnes
 Added the ability to query the a specific command with help.

 Revision 1.3  1998/11/05 22:55:20  barnes
 Added the channel switching for stereo cutlass.

 Revision 1.2  1998/11/04 15:55:54  barnes
 Changed name of the dio driver task.

 Revision 1.1  1998/10/27 19:15:30  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "print_info.h"

#include "message.h"
#include "dio.h"
#include "a_d_drive.h"
#include "task_name.h"
#include "hlpstr.h"

#define MAX_INPUT_LINE 256

int dio_flg=1;
int ad_flg=1;

char dio_name[256]={DIO_NAME};
char a_d_name[256]={A_D_NAME};
pid_t dio_id=NULL;
pid_t ad_id=NULL;
pid_t reset_id=NULL;

int bufsze=0;
int bufnum=0;
void *bufadr[16];

struct cmd_lst {
   char *cmd;
   char *abv;
};

struct cmd_lst lst[]={ 
  {"quit","q"},
  {"help","h"},
  {"reset","r"},
  {"beam","b"},
  {"freq","f"},
  {"gain","g"},
  {"antenna","a"},
  {"test","t"},
  {"verify","v"},
  {"status","st"},
  {"filter","fi"},
  {"fclr","c"},
  {"tsg","s"},
  {"load","l"},
  {"dump","d"},
  {"proxy","p"},
  {"ad_status","ad"},
  {"transfer","tr"},
  {"channel","cn"},
  {"fstatus","fs"},
  0
};

int query(char *str) {
  char query[16];

  fprintf(stdout,"%s",str);
  fflush(stdout);
  fgets(query,16,stdin);
  if ((query[0]=='Y') || (query[0]=='y')) return 1;
  return 0;
}


int decode_cmd(char *cmd) {
  int i;
  if (cmd==NULL) return -1;
  for (i=0;lst[i].cmd !=0;i++) 
	if ((strcmp(cmd,lst[i].cmd)==0) || (strcmp(cmd,lst[i].abv)==0)) break;
  
  if (lst[i].cmd !=0) return i;
  return -1;
}

void log_result(int status) {
  if (status==0) fprintf(stdout,"Command:Ok\n");
  else if (status==1) fprintf(stdout,"Command:Aborted\n");
  else fprintf(stdout,"Command:Failed\n");
}

void main(int argc,char *argv[]) {

  char delim[]={' ','\n'};
  int i;
  int yesno;
  int status;
  char *cmd=NULL,*arg=NULL;
  int loop=1;
  int c;
  char input_line[MAX_INPUT_LINE];
  int help=0;

  if (argc>1) {
    for (c=1;c<argc;c++) {
      if (strcmp(argv[c],"--help")==0) help=1;
      else if (strcmp(argv[c],"-d")==0) {
        strcpy(dio_name,argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-a")==0) {
        strcpy(a_d_name,argv[c+1]);
      } else if (strcmp(argv[c],"-td")==0) ad_flg=0;
      else if (strcmp(argv[c],"-ta")==0) dio_flg=0;
    }
  } 
  
  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0);
  }

  if ((dio_flg==1) && ((dio_id=locate_task_id(dio_name))==-1)) {
     fprintf(stderr,"Failed to locate task:%s\n",dio_name);
     exit(1);
  }

  if ((ad_flg==1) && ((ad_id=locate_task_id(a_d_name))==-1)) {
     fprintf(stderr,"Failed to locate task:%s\n",a_d_name);
     exit(1);
  }

  reset_id=get_scan_reset(ad_id);

  fprintf(stdout,"test_radar\n"),
  fprintf(stdout,"==========\n\n");
  if (dio_flg==1) 
    fprintf(stdout,"Attached to dio_drive registered as:%s\n",dio_name);
  if (ad_flg==1) { 
    fprintf(stdout,"Attached to a_d_drive registered as:%s\n",a_d_name);
  
    bufnum=get_buf_num(ad_id);
    bufsze=get_buf_size(ad_id);
   
    for (i=0;i<bufnum;i++) bufadr[i]=get_buf_adr(ad_id,i);

    fprintf(stdout,"There are %d buffers of %d bytes.\n",bufnum,bufsze);
  }
  fprintf(stdout,"For information, type %chelp%c and press enter.\n",
		  '"','"');
     
  do {
    fprintf(stdout,">");
    fflush(stdout);
    fgets(input_line,MAX_INPUT_LINE,stdin);
    cmd=strtok(input_line,delim);
    arg=strtok(NULL,delim+1);
    c=decode_cmd(cmd);
    switch (c) {
       case 0:
         yesno=query("Are you sure (y/n)");
         if (yesno !=0) loop=0;
         break;
       case 1:
         print_help(arg);
         break;
       case 2:
		 if (dio_flg==1) {
           status=reset_dio(dio_id);
           log_result(status);
         }
         break;
       case 3:
		 if (dio_flg==1) {
           status=test_beam(arg);
           log_result(status);
         }
         break;
       case 4:
		 if (dio_flg==1) {
           status=test_freq(arg);
           log_result(status);
         }
         break;
       case 5:
		 if (dio_flg==1) {
           status=test_gain(arg);
           log_result(status);
         }
         break;
       case 6:
		 if (dio_flg==1) {
           status=test_antenna(arg);
           log_result(status);
         }
         break;
       case 7: 
		 if (dio_flg==1) {
           status=test_mode(arg);
           log_result(status);
         }
         break;
       case 8: 
         if (dio_flg==1) test_verify(arg);
         break;
       case 9: 
         if (dio_flg==1) test_status(arg);
         break;
       case 10:
		 if (dio_flg==1) {
           status=test_filter(arg);
           log_result(status);
         }
         break;
       case 11:
		 if (dio_flg==1) { 
           status=test_fclr(arg);
           log_result(status);
         }
         break;
       case 12:
		 if (dio_flg==1) {
           status=test_tsg(arg);
           log_result(status);
         }
         break;
       case 13:
		 if (dio_flg==1) {
  		   status=load_tsg(arg);
           log_result(status);
         }
         break;
       case 14:
		 if (ad_flg==1) {
           status=dump_ad(arg);
           log_result(status); 
         }
         break;
       case 15:
         if (ad_flg==1) {
           status=test_proxy(arg);
           log_result(status);
         }
         break;
       case 16:
         if (ad_flg==1) status=test_ad_stat(arg);
         break;
       case 17:
         if (ad_flg==1) status=test_transfer(arg);
         break;
       case 18:
         if (dio_flg==1) status=test_channel(arg);
         log_result(status);
         break;
       case 19:
         if (dio_flg==1) test_fstatus(arg);
         break;
       default:
         fprintf(stdout,"Command not recognized, type %chelp%c for help.\n",
				 '"','"');
         break;
    }   
  } while (loop);
  fprintf(stdout,"Terminated.\n");
}   
