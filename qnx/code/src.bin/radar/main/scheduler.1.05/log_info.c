/* log_info.c
   ========== */

/*
 $Log: log_info.c,v $
 Revision 1.4  2000/10/30 17:25:51  barnes
 Fixed possible fault if the a NULL pointer is returned to the file pointer.

 Revision 1.3  1999/03/30 19:04:41  barnes
 Removed debugging code.

 Revision 1.2  1999/03/30 18:45:20  barnes
 Fixed problem of trailing spaces in the schedule.

 Revision 1.1  1999/03/30 18:29:38  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char logname[256]={"scdlog"};

void log_info(int flg,char *str) {
  FILE *fp;
  char *date;
  char logpath[256];
  time_t ltime;
  struct tm *time_of_day;
 
  time(&ltime);  
  time_of_day=gmtime(&ltime);

  date=asctime(time_of_day);  
  date[strlen(date)-1]=':';
  if (flg==0) fprintf(stderr,date);
  fprintf(stderr,str);
  fprintf(stderr,"\n");
  
  sprintf(logpath,"%s.%.3d",logname,time_of_day->tm_yday+1);
  fp=fopen(logpath,"a");
  if (fp !=NULL) {
    if (flg==0) fprintf(fp,date);
    fprintf(fp,str);
    fprintf(fp,"\n");
    fclose(fp);
  }
}










