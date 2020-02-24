/* write_gtable.c
   ==============
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
 $Log: write_gtable.c,v $
 Revision 1.13  2002/02/12 15:01:51  barnes
 Removed redundant headers.

 Revision 1.12  2002/02/06 22:52:57  barnes
 Added channel to the header record.

 Revision 1.11  2002/02/06 21:30:14  barnes
 Modification to include the channel number in the file.

 Revision 1.10  2001/06/27 20:49:26  barnes
 Added license tag

 Revision 1.9  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.8  1999/10/12 20:44:28  barnes
 Fixed bug in recording header.

 Revision 1.7  1999/10/12 20:21:11  barnes
 Modifications to include extra record information.

 Revision 1.6  1999/10/08 21:05:24  barnes
 Added extensions for noise and program id.

 Revision 1.5  1999/10/08 20:24:17  barnes
 Modifications for the extended file format.

 Revision 1.4  1999/10/05 17:48:30  barnes
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
#include <string.h>
#include <math.h>

#include "hardware.h" 
#include "inv_mag.h"

#include "math_constant.h"
#include "radar_scan.h"
#include "gtable.h"

#define NBLOCKS 2
#define NPARM1 18
#define NPARM2 8
#define NPARM3 12



int write_gtable(int fid,struct gtable *ptr,char *logbuf,int xtd) {
  int i,j;
 
  int syr,smo,sdy,shr,smt,ssc;
  int eyr,emo,edy,ehr,emt,esc;
  char buf[256];
  double sec;

  char *name_blk1[]={"st_id","chn","nvec","freq0","major_rev","minor_rev",
                     "prog_id","noise_mean","noise_sd","gsct",
                     "v_min","v_max","p_min","p_max","w_min","w_max",
                     "ve_min","ve_max",0};
  char *unit_blk1[]={"count","count","count","KHz",
                     "count","count","count","count",
                     "count","count","m/sec","m/sec","dB","dB","m/sec",
                     "m/sec","m/sec","m/sec",0};
  char *type_blk1[]={"int","int","int","float","int",
                     "int","int","float","float",
                     "int","float","float","float","float","float","float",
                      "float","float",0};


  char *name_blk2[]={"gmlong","gmlat","kvect","st_id","chn","grid_index",
                   "vlos","vlos_sd",0};
  char *unit_blk2[]={"degrees","degrees","degrees","count","count",
                   "count","m/sec","m/sec",0};
  char *type_blk2[]={"float","float","float","int","int",
                     "int","float","float",0};


  char *name_blk3[]={"gmlong","gmlat","kvect","st_id","chn","grid_index",
                   "vlos","vlos_sd","pwr","pwr_sd","wdt","wdt_sd",0};
  char *unit_blk3[]={"degrees","degrees","degrees","count","count",
                   "count","m/sec","m/sec","dB","dB","m/sec","m/sec",0};
  char *type_blk3[]={"float","float","float","int","int","int","float","float",
                     "float","float","float","float",0};





  epoch_time(ptr->ed_time,&eyr,&emo,&edy,&ehr,&emt,&sec);
  esc=sec;
  epoch_time(ptr->st_time,&syr,&smo,&sdy,&shr,&smt,&sec);
  ssc=sec;
 
  if (logbuf !=NULL) 
  sprintf(logbuf,"%d-%d-%d %d:%d:%d %d:%d:%d pnts=%d (%d)",
          syr,smo,sdy,shr,smt,ssc,ehr,emt,esc,ptr->npnt,ptr->st_id);

  sprintf(buf,"%d %d %d %d %d %d ", syr,smo,sdy,shr,smt,ssc);
  write(fid,buf,strlen(buf));

  sprintf(buf,"%d %d %d %d %d %d\n",eyr,emo,edy,ehr,emt,esc);
  write(fid,buf,strlen(buf));
  sprintf(buf,"%d\n",NBLOCKS);
  write(fid,buf,strlen(buf));

  sprintf(buf,"%d %d\n",1,NPARM1);
  write(fid,buf,strlen(buf));

  for (i=0;name_blk1[i] !=0;i++) {
    sprintf(buf,"%10s ",name_blk1[i]);
    write(fid,buf,strlen(buf));
  }
  sprintf(buf,"\n");
  write(fid,buf,strlen(buf));

 for (i=0;unit_blk1[i] !=0;i++) {
    sprintf(buf,"%10s ",unit_blk1[i]);
    write(fid,buf,strlen(buf));
  }
  sprintf(buf,"\n");
  write(fid,buf,strlen(buf));

 for (i=0;type_blk1[i] !=0;i++) {
    sprintf(buf,"%10s ",type_blk1[i]);
    write(fid,buf,strlen(buf));
  }
  sprintf(buf,"\n");
  write(fid,buf,strlen(buf));

 

  sprintf(buf,"%10d %10d %10d %#10g %10d %10d ",ptr->st_id,ptr->chn,
	  ptr->npnt,(double)
          ptr->freq,
          MAJOR_REVISION,MINOR_REVISION);
  write(fid,buf,strlen(buf));
  sprintf(buf,"%10d %#10g %#10g %10d ",ptr->prog_id,ptr->noise.mean,
          ptr->noise.sd,ptr->gsct);
  write(fid,buf,strlen(buf));
  sprintf(buf,"%#10g %#10g %#10g %#10g %#10g %#10g %#10g %#10g\n",
          ptr->min[0],ptr->max[0],
          ptr->min[1],ptr->max[1],
          ptr->min[2],ptr->max[2],
          ptr->min[3],ptr->max[3]);
  write(fid,buf,strlen(buf));

  if (xtd==0) {

    sprintf(buf,"%d %d\n",ptr->npnt,NPARM2);
    write(fid,buf,strlen(buf));
 
    for (i=0;name_blk2[i] !=0;i++) {
      sprintf(buf,"%10s ",name_blk2[i]);
      write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    write(fid,buf,strlen(buf));

   for (i=0;unit_blk2[i] !=0;i++) {
      sprintf(buf,"%10s ",unit_blk2[i]);
      write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    write(fid,buf,strlen(buf));

   for (i=0;type_blk2[i] !=0;i++) {
      sprintf(buf,"%10s ",type_blk2[i]);
      write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    write(fid,buf,strlen(buf));
  } else {


    sprintf(buf,"%d %d\n",ptr->npnt,NPARM3);
    write(fid,buf,strlen(buf));
 
    for (i=0;name_blk3[i] !=0;i++) {
      sprintf(buf,"%10s ",name_blk3[i]);
      write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    write(fid,buf,strlen(buf));

   for (i=0;unit_blk3[i] !=0;i++) {
      sprintf(buf,"%10s ",unit_blk3[i]);
      write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    write(fid,buf,strlen(buf));

   for (i=0;type_blk3[i] !=0;i++) {
      sprintf(buf,"%10s ",type_blk3[i]);
      write(fid,buf,strlen(buf));
    }
    sprintf(buf,"\n");
    write(fid,buf,strlen(buf));
  }


 
  for (i=0;i<ptr->pnum;i++) {
    if (ptr->pnt[i].cnt==0) continue;
    if (xtd==0) sprintf(buf,"%#10g %#10g %#10g %10d %10d %10d %#10g %#10g\n",
            ptr->pnt[i].mlon,ptr->pnt[i].mlat,
            ptr->pnt[i].azm,ptr->st_id,ptr->chn,ptr->pnt[i].ref,
            ptr->pnt[i].vel.median,ptr->pnt[i].vel.sd);
    else sprintf(buf,
     "%#10g %#10g %#10g %10d %10d %10d %#10g %#10g %#10g %#10g %#10g %#10g\n",
            ptr->pnt[i].mlon,ptr->pnt[i].mlat,
            ptr->pnt[i].azm,ptr->st_id,ptr->chn,ptr->pnt[i].ref,
            ptr->pnt[i].vel.median,ptr->pnt[i].vel.sd,
            ptr->pnt[i].pwr.median,ptr->pnt[i].pwr.sd,
            ptr->pnt[i].wdt.median,ptr->pnt[i].wdt.sd);

    write(fid,buf,strlen(buf));
 
  }
  return 0;
} 

int fwrite_gtable(FILE *fp,struct gtable *ptr,char *logbuf,int xtd) {
  return write_gtable(fileno(fp),ptr,logbuf,xtd);
}
