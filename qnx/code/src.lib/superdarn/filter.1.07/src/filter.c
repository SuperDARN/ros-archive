/* filter.c
   ========
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
 $Log: filter.c,v $
 Revision 1.24  2001/06/27 20:47:43  barnes
 Added license tag

 Revision 1.23  2001/01/29 18:11:52  barnes
 Added Author Name

 Revision 1.22  2000/08/01 19:17:22  barnes
 Fixed bug in the filter that meant the end beams were duplicated.

 Revision 1.21  2000/07/12 16:05:53  barnes
 Fixed bug in filtering incomplete scans.

 Revision 1.20  2000/07/10 13:06:05  barnes
 Fixed incorrect return from free_cell.

 Revision 1.19  2000/03/21 20:16:00  barnes
 Several bug fixes to do with making sure the output of the filter agrees
 with the inputs.

 Revision 1.18  2000/03/21 18:18:21  barnes
 Extended cell pointer array to cope with weird and wacky modes.

 Revision 1.17  1999/10/14 14:16:21  barnes
 Added attenuation to record.

 Revision 1.16  1999/10/12 22:04:35  barnes
 Fixed bug in setting the program id.

 Revision 1.15  1999/10/12 18:43:03  barnes
 Propagate a version of the ground scatter flag through the filter.

 Revision 1.14  1999/10/08 22:13:00  barnes
 Modifications for the extended format.

 Revision 1.13  1999/08/12 13:23:07  barnes
 Removed redundant filter function.

 Revision 1.12  1999/07/23 20:29:44  barnes
 Greatly improved speed by only re-allocating memory as it is needed.

 Revision 1.11  1999/04/06 17:19:26  barnes
 Fixed bug in one of the filter criterion.

 Revision 1.10  1999/04/06 17:09:42  barnes
 Added a more general purpose filtering routine that allows
 different parameters to be filtered.

 Revision 1.9  1999/01/28 18:17:40  barnes
 Fixed bug in median filter.

 Revision 1.8  1999/01/27 16:42:59  barnes
 Fixed bug in setting the noise value.

 Revision 1.7  1999/01/26 19:04:12  barnes
 Added the ability to select whether the output of the filter preserved the
 form of the input data, or uses an average value determined across the set.

 Revision 1.6  1999/01/25 19:17:21  barnes
 Modifications to use the modified radardata structure.

 Revision 1.5  1998/06/25 18:03:07  barnes
 Changed variance calculation so that is done internally.

 Revision 1.4  1998/06/25 16:03:36  barnes
 Fixed problem of ambiguous function name "cmp".

 Revision 1.3  1998/06/07 22:33:04  barnes
 Modifications to use the radardata structure and to use epoch time.

 Revision 1.2  1998/03/06 17:40:23  barnes
 Fixed problem mismatched pointer types in qsort throwing a compiler warning.

 Revision 1.1  1998/02/03 23:11:58  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "limit.h"
#include "radar_scan.h"
#include "cell.h"
#include "error.h"


double prm_filter_v(struct datapoint *ptr) {
  return ptr->v;
}

double prm_filter_w(struct datapoint *ptr) {
  return ptr->w_l;
}

double prm_filter_p(struct datapoint *ptr) {
  return ptr->p_l;
}


int cmp_filter_v(const void *p1,const void *p2) {
  struct datapoint **f1,**f2;
  f1=(struct datapoint **) p1;
  f2=(struct datapoint **) p2;
  if ((*f1)->v<(*f2)->v) return -1;
  if ((*f1)->v>(*f2)->v) return 1;
  return 0;
}

int cmp_filter_p(const void *p1,const void *p2) {
  struct datapoint **f1,**f2;
  f1=(struct datapoint **) p1;
  f2=(struct datapoint **) p2;
  if ((*f1)->p_l<(*f2)->p_l) return -1;
  if ((*f1)->p_l>(*f2)->p_l) return 1;
  return 0;
}

int cmp_filter_w(const void *p1,const void *p2) {
  struct datapoint **f1,**f2;
  f1=(struct datapoint **) p1;
  f2=(struct datapoint **) p2;
  if ((*f1)->w_l<(*f2)->w_l) return -1;
  if ((*f1)->w_l>(*f2)->w_l) return 1;
  return 0;
}



struct datapoint *median(struct datapoint **array,int size,
			 int (*cmpfn) (const void *,const void *)) {
  qsort( (void *) array,size,sizeof(struct datapoint *),cmpfn);
  return array[size/2];
}


struct datapoint *median_filter(struct cell *cell,
				int (*cmpfn) (const void *,const void *)) {
  struct datapoint *darray[1024];
  int i,j,k,l;
  int index=0;
 
  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      for (k=0;k<3;k++) {
        for (l=0;l<cell[i*9+3*j+k].cnt;l++) {
          if (cell[i*9+3*j+k].sct[l] !=0) darray[index++]=
	      &cell[i*9+3*j+k].data[l];  
	}
      }
    }
  }
  
  if (index>0) return median(darray,index,cmpfn);
  else return 0; 
}


int weight_scatter(struct cell *cell,int bm,int rng) {


  int weight[3][3][3]={
    { {1,1,1},{1,2,1},{1,1,1} },
    { {2,2,2},{2,4,2},{2,2,2} },
    { {1,1,1},{1,2,1},{1,1,1} }
  };
  int level=0;
  int i,j,k,l; 
  for (i=0;i<3;i++) 
    for (j=0;j<3;j++) 
      for (k=0;k<3;k++) 
        for (l=0;l<cell[i*9+3*j+k].cnt;l++)
          level+=weight[i][j][k]*cell[i*9+3*j+k].sct[l];
 
  return level;    
}

void free_cell(struct cell *cell) {
 int i,j,k;

 for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      for (k=0;k<3;k++) {
         if (cell[i*9+3*j+k].sct !=NULL) free(cell[i*9+3*j+k].sct); 
         if (cell[i*9+3*j+k].data !=NULL) free(cell[i*9+3*j+k].data); 
      }
    }
 }

}

int blank_cell(struct cell *cell) {
  int i,j;
 
  for (i=0;i<(3*3*3);i++) {
    for (j=0;j<cell[i].cnt;j++) cell[i].sct[j]=0;
  }
  return 0;
}

int build_cell(struct cell *cell,
	       struct radardata *input,int bm,int rng) {
  int i,j,k,l,x;
  int kmin=0;
  int kmax=3;

  int jmin=0;
  int jmax=3;
  int c=0;
  int m,n;
 
  if (bm==15) jmax=2;
  if (bm==0) jmin=1;
  if (rng==0) kmin=1;
  if (rng==74) kmax=2;

  for (i=0;i<3;i++) {
    for (j=jmin;j<jmax;j++) {
      for (k=kmin;k<kmax;k++) {
         m=bm+j-1;
         n=rng+k-1;   
         if (input[i].bmcnt[m]==0) continue;
         l=input[i].bmcnt[m];
         if (l !=cell[i*9+3*j+k].cnt) {
           cell[i*9+3*j+k].cnt=l;

           if (cell[i*9+3*j+k].sct==NULL) 
             cell[i*9+3*j+k].sct=malloc(l*sizeof(char));
           else  cell[i*9+3*j+k].sct=realloc(cell[i*9+3*j+k].sct,
                                            l*sizeof(char));
        
           if (cell[i*9+3*j+k].data==NULL) 
             cell[i*9+3*j+k].data=malloc(l*sizeof(struct datapoint));
           else  cell[i*9+3*j+k].data=realloc(cell[i*9+3*j+k].data,
                                            l*sizeof(struct datapoint));


	 }

         memcpy(cell[i*9+3*j+k].data,input[i].rng[m][n],
		l*sizeof(struct datapoint));
         memcpy(cell[i*9+3*j+k].sct,input[i].sct[m][n],l*sizeof(char));
      }
    }
  }
  return c;
}

  
int filter_raw(int mode,struct radardata *input,
	          struct radardata *output,int prm) {
  int i,j,k,w;
  int bm,rng;
  struct datapoint *pnt;   
 
  double error;
  int thresh[2] = {12,24};

  struct cell cell[3*3*3];

  memset(cell,0,3*3*3*sizeof(struct cell));

  output->scnt=input[1].scnt;
  output->st_id=input[1].st_id;
  output->ftype=input[1].ftype;
  output->stored=input[1].stored;
  output->major_rev=input[1].major_rev;
  output->minor_rev=input[1].minor_rev;
  output->stime=input[1].stime;
  output->etime=input[1].etime;

  output->stime=input[1].stime;
  output->etime=input[1].etime;
       
  if ((mode & 4)==4) {
    for (i=0;i<16;i++) {    
      if (input[1].bmcnt[i] !=0) {
         output->bmcnt[i]=1;
         if (output->cpid[i]==NULL) output->cpid[i]=malloc(sizeof(int));
         if (output->intt[i]==NULL) output->intt[i]=malloc(sizeof(int));
         if (output->frang[i]==NULL) output->frang[i]=malloc(sizeof(int));
         if (output->rsep[i]==NULL) output->rsep[i]=malloc(sizeof(int));
         if (output->freq[i]==NULL) output->freq[i]=malloc(sizeof(int));
         if (output->noise[i]==NULL) output->noise[i]=malloc(sizeof(int));
         if (output->atten[i]==NULL) output->atten[i]=malloc(sizeof(int));
         if (output->time[i]==NULL) output->time[i]=malloc(sizeof(double));

         output->frang[i][0]=input[1].frang[i][0];
         output->rsep[i][0]=input[1].rsep[i][0];
         output->freq[i][0]=input[1].freq[i][0]; 
         output->noise[i][0]=input[1].noise[i][0];
         output->atten[i][0]=input[1].atten[i][0];
         output->intt[i][0]=input[1].intt[i][0];
         output->cpid[i][0]=input[1].cpid[i][0];
         output->time[i][0]=input[1].time[i][0]; 
      }
    }
  } else {
     for (i=0;i<16;i++) {
       double avtime=0;
       int avnoise=0;
       int avatten=0;
       int avfreq=0;
       int avrsep=0;
       int avfrang=0;
       int avintt=0;
       int avcnt=0;
       int cpid=-1;
       for (j=0;j<3;j++) {
	 for (k=0;k<input[j].bmcnt[i];k++) {
            output->bmcnt[i]=1; 
            avtime+=input[j].time[i][k];
            avnoise+=input[j].noise[i][k];
            avatten+=input[j].atten[i][k];
            avfreq+=input[j].freq[i][k];
            avrsep+=input[j].rsep[i][k];
            avfrang+=input[j].frang[i][k];
            avintt+=input[j].intt[i][k];
            if (cpid==-1) cpid=input[j].cpid[i][k];
            avcnt++;
	  }
       }
       if (avcnt==0) avcnt=1;
     

       if (output->bmcnt[i]==1) {
         if (output->cpid[i]==NULL) output->cpid[i]=malloc(sizeof(int));
         if (output->intt[i]==NULL) output->intt[i]=malloc(sizeof(int));
         if (output->frang[i]==NULL) output->frang[i]=malloc(sizeof(int));
         if (output->rsep[i]==NULL) output->rsep[i]=malloc(sizeof(int));
         if (output->freq[i]==NULL) output->freq[i]=malloc(sizeof(int));
         if (output->noise[i]==NULL) output->noise[i]=malloc(sizeof(int));
         if (output->atten[i]==NULL) output->atten[i]=malloc(sizeof(int));
         if (output->time[i]==NULL) output->time[i]=malloc(sizeof(double));
         output->frang[i][0]=avfrang/avcnt;
         output->rsep[i][0]=avrsep/avcnt;
         output->freq[i][0]=avfreq/avcnt; 
         output->intt[i][0]=avintt/avcnt;
         output->cpid[i][0]=cpid;
         output->noise[i][0]=avnoise/avcnt;
         output->atten[i][0]=avatten/avcnt;
         output->time[i][0]=avtime/avcnt;
       }
     }
  }

  for (bm=0;bm<16;bm++) {
     
    if (output->bmcnt[bm]==0) continue;
    for(rng=0;rng<75;rng++) {
      if (output->sct[bm][rng]==NULL) 
        output->sct[bm][rng]=malloc(sizeof(char));
      if (output->rng[bm][rng]==NULL) 
        output->rng[bm][rng]=malloc(sizeof(struct datapoint));

      build_cell(cell,input,bm,rng);       
          
      output->rng[bm][rng][0].gsct=0;
    
      w=weight_scatter(cell,bm,rng); 
  
      if ((bm==0) || (bm==15)) w=w*1.5;
      if (w>thresh[mode % 3]) {

        
  
	if (prm & 0x01) {
         
           error=find_error(cell,prm_filter_v); 
           pnt=median_filter(cell,cmp_filter_v);
         
           if (pnt !=NULL) {
             output->sct[bm][rng][0]=1;
             if (pnt->gsct !=0) 
             output->rng[bm][rng][0].gsct=output->rng[bm][rng][0].gsct | 0x01; 
             output->rng[bm][rng][0].v=pnt->v;          
             output->rng[bm][rng][0].v_e=error;
           }
	}
	
        if (prm & 0x02) {
           error=find_error(cell,prm_filter_p);
           pnt=median_filter(cell,cmp_filter_p);
           if (pnt !=NULL) {
             output->sct[bm][rng][0]=1;
             if (pnt->gsct !=0)      
             output->rng[bm][rng][0].gsct=output->rng[bm][rng][0].gsct | 0x02;
             output->rng[bm][rng][0].p_l=pnt->p_l;          
             output->rng[bm][rng][0].p_l_e=error;
           }
	}
 
         if (prm & 0x04) {
           error=find_error(cell,prm_filter_w);
           pnt=median_filter(cell,cmp_filter_w);
           if (pnt !=NULL) {
             output->sct[bm][rng][0]=1;
             if (pnt->gsct !=0)    
             output->rng[bm][rng][0].gsct=output->rng[bm][rng][0].gsct | 0x04;
             output->rng[bm][rng][0].w_l=pnt->w_l;          
             output->rng[bm][rng][0].w_l_e=error;
           }
	}
      } else output->sct[bm][rng][0]=0;
      blank_cell(cell);
    }
  }
  
  free_cell(cell);
  
  return 0;
}







