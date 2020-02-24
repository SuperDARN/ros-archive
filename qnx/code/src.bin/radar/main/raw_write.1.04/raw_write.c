/* raw_write.c
   ===========
   Author: R.J.Barnes
*/

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
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
 $Log: raw_write.c,v $
 Revision 6.20  2001/06/27 20:56:19  barnes
 Added license tag

 Revision 6.19  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 6.18  2000/09/18 18:21:47  barnes
 Added missing header.

 Revision 6.17  1999/04/10 20:49:26  barnes
 Added help message system.

 Revision 6.16  1999/03/30 19:23:41  barnes
 Moved task names into header.

 Revision 6.15  1999/03/19 18:37:57  barnes
 Modifications to support the new file libraries.

 Revision 6.14  1998/11/09 23:04:23  barnes
 Incorporated rawdata header.

 Revision 6.13  1998/09/08 14:38:23  barnes
 Fixed problem with a change in the environment variable that locates
 the directory in which files are written.

 Revision 6.12  1998/09/08 14:30:33  root
 Modification to allow the threshold value to be recorded in the file
 header.

 Revision 6.11  1998/06/19 14:48:36  root
 Added support for the new radar_name function.

 Revision 6.10  1998/06/11 15:46:44  root
 Changed the log message when the task is reset.

 Revision 6.9  1998/06/11 15:44:12  root
 Added support for the TASK_RESET message.

 Revision 6.8  1998/06/11 14:54:38  root
 Moved usage into a different file.

 Revision 6.7  1998/06/04 18:58:11  root
 Modification to stop the raw file being opened and closed every time
 a data block is processed.

 Revision 6.6  1998/06/03 20:19:00  root
 Modification to the revision control system.

 Revision 6.5  1998/05/29 13:33:50  root
 Fixed incorrect text message on termination.

 Revision 6.4  1998/05/29 11:58:28  root
 Added code to shut down properly on receipt of the TASK_QUIT message.

 Revision 6.3  1998/05/29 11:24:45  root
 modified calling sequence of write_raw.c

 Revision 6.2  1998/05/29 10:05:09  root
 Removed redundant line of code.

 Revision 6.1  1998/05/28 18:35:54  root
 New version of the task that uses multi-plexed data messages.

 Revision 5.7  1998/05/27 19:19:38  root
 Modifications for operation with the new libraries.

 Revision 5.6  1997/05/02 18:22:55  root
 Fixed memory leakage problem and bug in skipping data writes.

 Revision 5.5  1997/03/26 15:22:14  root
 Corrected bug that prevented version number being recorded in the file.

 * Revision 5.4  1997/03/26  12:50:43  root
 * Fixed bug that left files open if a beam was skipped.
 *
 * Revision 5.3  1997/03/26  12:44:19  root
 * Added flag to skip write operations.
 *
 * Revision 5.2  1997/03/25  18:24:57  root
 * Added record of version number in the error log.
 *
 * Revision 5.1  1997/03/25  18:20:54  root
 * New version using the file_io library.
 *
 * Revision 1.1  1997/03/25  18:19:19  root
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include "print_info.h"
#include "rawdata.h"
#include "radar_name.h"
#include "raw_write.h"
#include "message.h"
#include "decode_msg.h"
#include "task_msg.h"
#include "file_io.h"
#include "log_error.h"

#include "version.h"
#include "task_name.h"
#include "hlpstr.h"

struct radar_id *rid;

main (int argc,char *argv[]) {

  char *envstr;
  int status;
  int raw_id;
  pid_t task_id;
  int reply_tid;
  char msg;
  char rmsg;
  int i,j; 
  struct rawdata *rdt;
  char *rawname=NULL;
  FILE *fp;
  char sfx;  
  int recnum;
  int help=0;

  struct task_decode tblk;
  struct task_decode *ptr; 
  char errbuf[256];
  char raw_name[32]={TASK_NAME};
  char errlog_name[32]={ERRLOG};
  struct task_id *tsk;
  int thresh=0;

  envstr=getenv("SD_RADARNAME");
  if (envstr == NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(1);
  }

 
  fp=fopen(envstr,"r");
  rid=load_radar(fp);
  fclose(fp);

  ptr=&tblk;
 
  /* allocate memory to store the records */
 
  if (argc>1) { 
    int c;
    for (c=1;c<argc;c++) {
      if (strcmp(argv[c],"--help")==0) help=1;
      if (strcmp(argv[c],"-t")==0) {
        thresh=atoi(argv[c+1]);
        c++; 
      } else if (strcmp(argv[c],"-e")==0) {
        strcpy(errlog_name,argv[c+1]);
        c++;
      } else strcpy(raw_name,argv[c]);
    }
  } 

  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0);
  }


  tsk=make_task_id(errlog_name);
  if (tsk==NULL) {
    perror("Unable to establish error handler.");
    exit(EXIT_FAILURE);
  }  

  if ( (raw_id=qnx_name_attach(0,raw_name) ) == -1) { 
    perror("Unable to attach raw_write task");
    log_error(tsk,raw_name,
						"Unable to register name");
    exit(EXIT_FAILURE);
  }

  sprintf(errbuf,"Started (version %s)",VSTRING);
  log_error(tsk,raw_name,errbuf);  

  while (1) {
   
    reply_tid=Receive(0,&msg,sizeof(char));
    rmsg=decode_msg(reply_tid,msg,ptr);
    Reply(reply_tid,&rmsg,sizeof(char));
    if (msg==TASK_RESET) {
      log_error(tsk,raw_name,"Reset");
      if (rawname !=NULL) free(rawname);
      rawname=NULL;
    } else if (msg==TASK_QUIT) {
      log_error(tsk,raw_name,"Stopped.");
      exit(0); 
    } else if (msg==TASK_DATA) {
      for (i=0;i<ptr->block.num;i++) {
        if (ptr->block.data[i].type !=RAW_TYPE) continue;
        rdt=(struct rawdata *) (ptr->block.ptr[ptr->block.data[i].index]);
        if (close_poll(ptr,i) !=0) {
          log_error(tsk,raw_name,"Closing file");
          if (rawname !=NULL) free(rawname);
          rawname=NULL;
        }
        if (open_poll(ptr,i) !=0) {
          log_error(tsk,raw_name,"Opening file");
          sfx=0;
          rawname=open_file(getenv("SD_DAT_PATH"),
							ptr->block.data[i].time,
							radar_code(rid,rdt->PARMS.ST_ID),"dat",0,&sfx,0); 
          fp=fopen(rawname,"a");
          if (fp !=NULL) status=raw_header_fwrite("rawwrite",VSTRING,thresh,
											     ptr->open.ptr,fp);
          else status=1;
        
          if (status !=0) {
            if (fp !=NULL) fclose(fp);
            /* failed to write header */
            log_error(tsk,raw_name,
					"Write header failed");
          }
          recnum=0L;
        } 

        if ((fp==NULL)  && (rawname !=NULL)) fp=fopen(rawname,"a");

        if (fp==NULL) {
           log_error(tsk,raw_name,"File not open");
           continue;
        }      

        /* process the records in here */
        recnum++;
        status=raw_fwrite(fp,"rawwrite",rdt,thresh,recnum);
        if (status !=0) log_error(tsk,raw_name,"Error writing record");
           
      }
      fclose(fp);
      fp=NULL;
      free(ptr->store);          
    }
  }
} 
