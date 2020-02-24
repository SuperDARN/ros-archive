/* do_tsg.c
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
 $Log: do_tsg.c,v $
 Revision 1.4  2001/06/27 20:54:47  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:12:04  barnes
 Added Author Name

 Revision 1.2  2000/03/17 17:38:51  barnes
 Modifications for the stereo clear frequency search.

 Revision 1.1  1998/11/09 16:23:35  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "dio.h"
#include "read_tsg.h"

extern int ex;
extern int cnt;
extern unsigned char code[MAX_TSG];
extern unsigned char rep[MAX_TSG];
extern unsigned char xtd[MAX_TSG];
extern pid_t dio_id;
extern pid_t ad_id;
extern pid_t reset_id;

extern bufnum;
extern bufsze;
extern void *bufadr[16];


int test_tsg(char *arg) {
  int status=0;
  int i,j;
  char chstr[2]={'A','B'};
  int stereo=-1;
  int repnum=0;
  int tsgid[2]={-1,-1};
  int exflg[2]={-1,-1};
  int stflg[2]={-1,-1};
  int frqflg[2]={-1,-1};
  int fnum=0;
  int flist[4]={-1,-1,-1};
  int *fptr[2]={NULL,NULL};
  int delay[2]={-1,-1};
  int *dptr=NULL;
  char str[16];
  unsigned char seq[2];
  unsigned char xflg[2];
  unsigned char sflg[2];


  if (arg !=NULL) {
    char *astr=NULL;
    astr=strtok(arg," ");
    if ((astr !=NULL) && (astr[0]=='s')) stereo=1;
    else if (astr !=NULL) stereo=0;
    for (j=0;j<(stereo==1)+1;j++) {
      astr=strtok(NULL," ");
      if (astr !=NULL) tsgid[j]=atoi(astr);
      astr=strtok(NULL," ");
      if ((stereo==1) && (astr !=NULL)) delay[j]=atoi(astr);
      astr=strtok(NULL," ");
      if ((astr !=NULL) && (astr[0]=='q')) stflg[j]=1;
      else if (astr !=NULL) stflg[j]=0;
      astr=strtok(NULL," ");
      if ((astr !=NULL) && (astr[0]=='e')) exflg[j]=1;
      else if (astr !=NULL) exflg[j]=0;
      astr=strtok(NULL," ");
      if ((astr !=NULL) && (astr[0]=='f')) frqflg[j]=1;
      else if (astr !=NULL) frqflg[j]=0;
      if (frqflg[j]==1) {
        char *fstr=NULL;
        fstr=strtok(NULL," ");
        fnum=0; 
        while ((fstr !=NULL) && (fnum<2)) {
          flist[fnum+j*2]=atoi(fstr);
          if ((flist[fnum+j*2]>8000) && (flist[fnum+j*2]<25000)) fnum++;
          if (fnum<2) fstr=strtok(NULL," ");
        }
      }
    }
    astr=strtok(NULL," ");
    if (astr !=NULL) repnum=atoi(astr);
  }     

  if (stereo==-1) {
    while (stereo==-1) {
      fprintf(stdout,
			 "Stereo operation (normal/stereo):");
      fflush(stdout);
      fgets(str,16,stdin);
      if (str[0]=='.') return 1;
      if (str[0]=='s') stereo=1;
      else if (str[0] !=0) stereo=0;
    }
  }

  for (j=0;j<stereo+1;j++) { 
    while ((tsgid[j]<0) || (tsgid[j]>31)) {
      fprintf(stdout,"Please enter a timing sequence between 0 and 32");
      if (stereo==1) fprintf(stdout," for channel %c:",chstr[j]);
      else fprintf(stdout,":");
      fflush(stdout);
      fgets(str,16,stdin);
      if (str[j] !=0) tsgid[j]=atoi(str);     
      if (str[j]  =='.') return 1;
    } 

    if (stereo !=0) {
      while (delay[j]<0) {
        fprintf(stdout,"Please enter a delay for this channel:");
        fflush(stdout);
        fgets(str,16,stdin);
        if (str[j] !=0) delay[j]=atoi(str);     
        if (str[j]  =='.') return 1;
      } 
    }

    if (stflg[j]==-1) {
      while (stflg[j]==-1) {
        fprintf(stdout,
			 "Read status information ");
        if (stereo==1) 
          fprintf(stdout,"on channel %c (normal/status):",chstr[j]);
        else fprintf(stdout,"(normal/status):");
        fflush(stdout);
        fgets(str,16,stdin);
        if (str[0]=='.') return 1;
        if (str[0]=='s') stflg[j]=1;
        else if (str[0] !=0) stflg[j]=0;
      }
    }

    if (exflg[j]==-1) {
      while (exflg[j]==-1) {
        fprintf(stdout,
			 "Transmit normal or beam switching sequence ");
        if (stereo==1) 
          fprintf(stdout,"on channel %c (normal/beam):",chstr[j]);
        else fprintf(stdout,"(normal/beam):");

        fflush(stdout);
        fgets(str,16,stdin);
        if (str[0]=='.') return 1;
        if (str[0]=='b') exflg[j]=1;
        else if (str[0] !=0) exflg[j]=0;
      }
    }
    if (frqflg[j]==-1) {
      while (frqflg[j]==-1) {
        fprintf(stdout,
			 "Use normal or frequency switching ");
        if (stereo==1) 
          fprintf(stdout,"on channel %c (normal/frequency):",chstr[j]);
        else fprintf(stdout,"(normal/frequency):");
        fflush(stdout);
        fgets(str,16,stdin);
        if (str[0]=='.') return 1;
        if (str[0]=='f') frqflg[j]=1;
        else if (str[0] !=0) frqflg[j]=0;
      }
    }
    if ((frqflg[j] !=0) && (fnum<2)) {
      while(fnum<2) {
        fprintf(stdout,"Frequency %d:",fnum);
        fflush(stdout);
        fgets(str,32,stdin);
        if (str[0]=='.') return 1;
        if ((str[0]=='x') && (fnum>0)) break;
        flist[fnum+2*j]=atoi(str);
        if ((flist[fnum+2*j]>8000) && (flist[fnum+2*j]<25000)) fnum++;
      }
      fnum=0;
    }
  }
  while ((repnum<=0) || (repnum>5000)) {
    fprintf(stdout,"How many times should the sequence be repeated:");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) repnum=atoi(str);     
    if (str[0]  =='.') return 1;
  }

  if (stereo==1) fprintf(stdout,"Transmitting in Stereo mode.\n\n");

  for (i=0;i<stereo+1;i++)  {  
    if (stereo==1) fprintf(stdout,"Channel:%c\n\n",chstr[i]);

     fprintf(stdout,"Using sequence id:%d\n",tsgid[i]);
     if (stereo==1)  fprintf(stdout,"Delay:%d\n",delay[i]);
     if (stflg[i]==1) fprintf(stdout,"Reading status after pulse transmit\n");
     else fprintf(stdout,"Ignoring status signals.\n");
     if (exflg[i]==1) fprintf(stdout,"Using beam switching\n");
     else fprintf(stdout,"Using regular sequence\n");
     if (frqflg[i]==1) fprintf(stdout,"Using frequency switching:%d %d\n",
								flist[i*2],flist[i*2+1]);
     else fprintf(stdout,"Single frequency mode\n");
   }
   fprintf(stdout,"Repeating sequence %d times.\n",repnum);
 
  for (i=0;i<2;i++) {
    seq[i]=tsgid[i];
    sflg[i]=stflg[i];
    xflg[i]=exflg[i];
  }
  if (frqflg[0]==1) fptr[0]=flist;
  if (frqflg[1]==1) fptr[1]=&flist[2];
 
  if (stereo==1) dptr=delay;

  for (i=0;i<repnum;i++) {
    if ((frqflg[0]==1) || (frqflg[1]==1)) 
       status=send_tsg(dio_id,stereo,seq,xflg,sflg,dptr,&fptr);
    else status=send_tsg(dio_id,stereo,seq,xflg,sflg,dptr,NULL);
    if (status !=0) break;
  }  
  return status;
}
