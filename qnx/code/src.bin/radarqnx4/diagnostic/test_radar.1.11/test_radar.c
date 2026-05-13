/* test_radar.c
   ============
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/

/* 
 $Log: test_radar.c,v $
 Revision 1.13  2005/06/16 16:34:02  barnes
 Modifications for phase steered radars.

 Revision 1.12  2004/06/21 23:21:22  barnes
 QNX compile audit.

 Revision 1.11  2004/05/10 14:06:10  barnes
 Modification to deal with new libraries.

 Revision 1.10  2001/06/27 20:54:47  barnes
 Added license tag

 Revision 1.9  2001/01/29 18:12:04  barnes
 Added Author Name

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
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include "rtypes.h"
#include "option.h"

#include "dio.h"
#include "ad.h"
#include "do_op.h"
#include "do_tsg.h"
#include "help.h"
#include "task_name.h"
#include "hlpstr.h"

#define MAX_INPUT_LINE 256

unsigned char dioflg=0;
unsigned char adflg=0;

char *dioname=NULL;
char *adname=NULL;

char diodname[256]={DIO_NAME};
char addname[256]={A_D_NAME};

ipcid_t dioid=NULL;
ipcid_t adid=NULL;
ipcid_t resetid=NULL;

int bufsze=0;
int bufnum=0;
void *bufadr[16];

struct OptionData opt;
int a;

struct cmd_lst {
   char *cmd;
   char *abv;
};


struct cmd_lst lst[]={ 
  {"quit","q"},
  {"help","h"},
  {"reset","r"},
  {"beam","b"},
  {"phase","P"},
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
  char inputline[MAX_INPUT_LINE];
  int help=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"d",'t',&dioname);
  OptionAdd(&opt,"a",'t',&adname);
  OptionAdd(&opt,"ta",'x',&adflg);
  OptionAdd(&opt,"td",'x',&dioflg);
  a=OptionProcess(1,argc,argv,&opt,NULL);  

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (dioname==NULL) dioname=diodname;
  if (adname==NULL)  adname=addname;


  if ((dioflg==1) && ((dioid=qnx_name_locate(0,dioname,0,NULL))==-1)) {
     fprintf(stderr,"Failed to locate task:%s\n",dioname);
     exit(1);
  }

  if ((adflg==1) && ((adid=qnx_name_locate(0,adname,0,NULL))==-1)) {
     fprintf(stderr,"Failed to locate task:%s\n",adname);
     exit(1);
  }

  resetid=ADGetScanReset(adid);

  fprintf(stdout,"test_radar\n"),
  fprintf(stdout,"==========\n\n");
  if (dioflg==1) 
    fprintf(stdout,"Attached to dio_drive registered as:%s\n",dioname);
  if (adflg==1) { 
    fprintf(stdout,"Attached to ad_drive registered as:%s\n",adname);
  
    bufnum=ADGetBufNum(adid);
    bufsze=ADGetBufSize(adid);
   
    for (i=0;i<bufnum;i++) bufadr[i]=ADGetBufAdr(adid,i);

    fprintf(stdout,"There are %d buffers of %d bytes.\n",bufnum,bufsze);
  }
  fprintf(stdout,"For information, type %chelp%c and press enter.\n",
		  '"','"');
     
  do {
    fprintf(stdout,">");
    fflush(stdout);
    fgets(inputline,MAX_INPUT_LINE,stdin);
    cmd=strtok(inputline,delim);
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
		 if (dioflg==1) {
           status=DIOReset(dioid);
           log_result(status);
         }
         break;
       case 3:
		 if (dioflg==1) {
           status=test_beam(arg);
           log_result(status);
         }
         break;
       case 4:
		 if (dioflg==1) {
           status=test_phase(arg);
           log_result(status);
         }
         break;

       case 5:
		 if (dioflg==1) {
           status=test_freq(arg);
           log_result(status);
         }
         break;
       case 6:
		 if (dioflg==1) {
           status=test_gain(arg);
           log_result(status);
         }
         break;
       case 7:
		 if (dioflg==1) {
           status=test_antenna(arg);
           log_result(status);
         }
         break;
       case 8: 
		 if (dioflg==1) {
           status=test_mode(arg);
           log_result(status);
         }
         break;
       case 9: 
         if (dioflg==1) test_verify(arg);
         break;
       case 10: 
         if (dioflg==1) test_status(arg);
         break;
       case 11:
		 if (dioflg==1) {
           status=test_filter(arg);
           log_result(status);
         }
         break;
       case 12:
		 if (dioflg==1) { 
           status=test_fclr(arg);
           log_result(status);
         }
         break;
       case 13:
		 if (dioflg==1) {
           status=test_tsg(arg);
           log_result(status);
         }
         break;
       case 14:
		 if (dioflg==1) {
  		   status=load_tsg(arg);
           log_result(status);
         }
         break;
       case 15:
		 if (adflg==1) {
           status=dump_ad(arg);
           log_result(status); 
         }
         break;
       case 16:
         if (adflg==1) {
           status=test_proxy(arg);
           log_result(status);
         }
         break;
       case 17:
         if (adflg==1) test_ad_stat(arg);
         break;
       case 18:
         if (adflg==1) status=test_transfer(arg);
         break;
       case 19:
         if (dioflg==1) status=test_channel(arg);
         log_result(status);
         break;
       case 20:
         if (dioflg==1) test_fstatus(arg);
         break;
       default:
         fprintf(stdout,"Command not recognized, type %chelp%c for help.\n",
				 '"','"');
         break;
    }   
  } while (loop);
  fprintf(stdout,"Terminated.\n");
}   
