/* write_grid.c
   ============ */

/*
 $Log: write_grid.c,v $
 Revision 1.14  1999/10/12 20:59:29  barnes
 Fixed bugs in the output format.

 Revision 1.13  1999/10/12 20:48:23  barnes
 Added determination as to whether the extended or regular output should be produced.

 Revision 1.12  1999/10/12 20:44:28  barnes
 Fixed bug in recording header.

 Revision 1.11  1999/10/08 20:24:17  barnes
 Modifications for the extended file format.

 Revision 1.10  1999/10/08 16:49:29  barnes
 Fixed problem with missing return values.

 Revision 1.9  1999/10/06 17:35:12  barnes
 Changed some of the names of routines.

 Revision 1.8  1999/10/06 17:26:24  barnes
 Removed redundant code that bumped the end hour by 24 if we ran over
 a day.

 Revision 1.7  1999/10/06 17:24:45  barnes
 Split the write routine into sub-routines.

 Revision 1.6  1999/10/06 12:42:46  barnes
 Fixed bug in writing the file headers.

 Revision 1.5  1999/10/05 18:31:40  barnes
 Fixed bug in recording block header.

 Revision 1.4  1999/10/05 17:48:14  barnes
 Modifications to tidy up the headers.

 Revision 1.3  1999/10/05 14:10:04  barnes
 Changed file format so that block headers are included with blocks.

 Revision 1.2  1999/10/04 20:07:43  barnes
 New File Format Changes.

 Revision 1.1  1999/08/11 19:56:15  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>

#include "cnv_time.h"
#include "file_lib.h"

#include "griddata.h"


#define NBLOCKS 2

int encode_grid_one(FILE *fp,struct griddata *ptr) {
  int i;
  char *name[]={"st_id","nvec","freq0","major_rev","minor_rev",
                "prog_id","noise_mean","noise_sd","gsct","v_min","v_max",
                "p_min","p_max","w_min","w_max","ve_min","ve_max",0};
  char *unit[]={"count","count","KHz","count","count",
                "count","count","count","count","m/sec","m/sec",
                "dB","dB","m/sec","m/sec","m/sec","m/sec",0};
  char *type[]={"int","int","float","int","int","int",
                "float","float","int","float","float","float","float",
                "float","float","float","float",0};

  int npnt=0;

  for (i=0;i<ptr->npnt[0];i++) if (ptr->sdata[i].st_id !=-1) npnt++;
  
  fprintf(fp,"%d %d\n",npnt,ptr->nprm[0]);
 
  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  for (i=0;i<ptr->npnt[0];i++) if (ptr->sdata[i].st_id !=-1) {  
      fprintf(fp,"%10d %10d %#10g %10d %10d ",
      ptr->sdata[i].st_id,ptr->sdata[i].npnt,ptr->sdata[i].freq0,
      (int) ptr->sdata[i].major_revision, (int) ptr->sdata[i].minor_revision);
      fprintf(fp,"%10d %#10g %#10g %10d ",
      ptr->sdata[i].prog_id,ptr->sdata[i].noise.mean,ptr->sdata[i].noise.sd,
      (int) ptr->sdata[i].gsct);
      fprintf(fp,"%#10g %#10g %#10g %#10g %#10g %#10g %#10g %#10g\n",
              ptr->sdata[i].vel.min,ptr->sdata[i].vel.max,
              ptr->sdata[i].pwr.min,ptr->sdata[i].pwr.max,
              ptr->sdata[i].wdt.min,ptr->sdata[i].wdt.max,
              ptr->sdata[i].verr.min,ptr->sdata[i].verr.max);




   }
  return 0;
}



int encode_grid_two(FILE *fp,struct griddata *ptr) {
  int i;
  char *name[]={"gmlong","gmlat","kvect","st_id","grid_index",
                   "vlos","vlos_sd",0};
  char *unit[]={"degrees","degrees","degrees","count",
                   "count","m/sec","m/sec",0};
  char *type[]={"float","float","float","int","int","float","float",0};

  int npnt=0;

  for (i=0;i<ptr->npnt[1];i++) if (ptr->data[i].st_id !=-1) npnt++;
  
  fprintf(fp,"%d %d\n",npnt,ptr->nprm[1]);
 
  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  for (i=0;i<ptr->npnt[1];i++) {
    if (ptr->data[i].st_id==-1) continue;
    fprintf(fp,"%#10g %#10g %#10g %10d %10d %#10g %#10g\n",
              ptr->data[i].mlon,
              ptr->data[i].mlat,
              ptr->data[i].azm,
              ptr->data[i].st_id,
              ptr->data[i].index,
              ptr->data[i].vel.median,
              ptr->data[i].vel.sd);
 
  
  }
  return 0;
}



int encode_grid_three(FILE *fp,struct griddata *ptr) {
  int i;
  char *name[]={"gmlong","gmlat","kvect","st_id","grid_index",
                   "vlos","vlos_sd",
                   "pwr","pwr_sd","wdt","wdt_sd",0};
  char *unit[]={"degrees","degrees","degrees","count",
                   "count","m/sec","m/sec","dB","dB","m/sec","m/sec",0};
  char *type[]={"float","float","float","int","int","float","float",
                "float","float","float","float",0};

  int npnt=0;

  for (i=0;i<ptr->npnt[1];i++) if (ptr->data[i].st_id !=-1) npnt++;
  
  fprintf(fp,"%d %d\n",npnt,ptr->nprm[1]);
 
  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  for (i=0;i<ptr->npnt[1];i++) {
    if (ptr->data[i].st_id==-1) continue;
    fprintf(fp,
      "%#10g %#10g %#10g %10d %10d %#10g %#10g %#10g %#10g %#10g %#10g\n",
              ptr->data[i].mlon,
              ptr->data[i].mlat,
              ptr->data[i].azm,
              ptr->data[i].st_id,
              ptr->data[i].index,
              ptr->data[i].vel.median,
              ptr->data[i].vel.sd,
              ptr->data[i].pwr.median,
              ptr->data[i].pwr.sd,
              ptr->data[i].wdt.median,
              ptr->data[i].wdt.sd);
 
  
  }
  return 0;
}


int write_grid(FILE *fp,struct griddata *ptr,int flag) {
  int syr,smo,sdy,shr,smt,ssc;
  int eyr,emo,edy,ehr,emt,esc;
  double sec;
 
  epoch_time(ptr->st_time,&syr,&smo,&sdy,&shr,&smt,&sec);
  ssc=(int) sec;
  epoch_time(ptr->ed_time,&eyr,&emo,&edy,&ehr,&emt,&sec);
  esc=(int) sec;
  if (flag !=0)
  fprintf(stderr,"%d-%d-%d %d:%d:%d %d:%d:%d pnts=%d files=%d\n",
           syr,smo,sdy,shr,smt,ssc,ehr,emt,esc,ptr->npnt[1],ptr->npnt[0]);


  fprintf(fp,"%d %d %d %d %d %d ",syr,smo,sdy,shr,smt,ssc);
  fprintf(fp,"%d %d %d %d %d %d\n",eyr,emo,edy,ehr,emt,esc);
  fprintf(fp,"%d\n",NBLOCKS);

  encode_grid_one(fp,ptr);
  if (ptr->nprm[1]==7) encode_grid_two(fp,ptr);
  else encode_grid_three(fp,ptr);
 
  return 0;
}





