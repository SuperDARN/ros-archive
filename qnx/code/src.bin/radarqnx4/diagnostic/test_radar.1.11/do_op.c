/* do_op.c
   =======
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
 $Log: do_op.c,v $
 Revision 1.16  2005/06/16 16:34:02  barnes
 Modifications for phase steered radars.

 Revision 1.15  2004/06/21 23:21:22  barnes
 QNX compile audit.

 Revision 1.14  2004/05/10 14:06:10  barnes
 Modification to deal with new libraries.

 Revision 1.13  2001/06/27 20:54:47  barnes
 Added license tag

 Revision 1.12  2001/01/29 18:12:04  barnes
 Added Author Name

 Revision 1.11  2000/11/14 16:04:37  barnes
 Modification to report the status of the frequency synthesizer.

 Revision 1.10  2000/11/14 15:24:46  barnes
 Added code to report when the synthesizer is in local mode.

 Revision 1.9  2000/03/17 17:38:51  barnes
 Modifications for the stereo clear frequency search.

 Revision 1.8  1999/04/02 18:18:12  barnes
 Fixed problem with reading extended timing sequences.

 Revision 1.7  1999/03/29 16:09:44  barnes
 Fixed problem with A/D transfer.

 Revision 1.6  1998/11/09 16:23:35  barnes
 Added support for stereo operations.

 Revision 1.5  1998/11/08 22:18:42  barnes
 Changed calling sequence of send_tsg.

 Revision 1.4  1998/11/08 17:22:17  barnes
 Added code to support stereo cutlass and modified the calling sequence
 for send_tsg.

 Revision 1.3  1998/11/06 00:25:55  barnes
 Fixed bug in setting the test mode.

 Revision 1.2  1998/11/05 22:55:20  barnes
 Added the channel switching for stereo cutlass.

 Revision 1.1  1998/10/27 19:15:30  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "dio.h"
#include "ad.h"
#include "read_tsg.h"

#define LOCAL_FREQ 2

extern int ex;
extern int cnt;
extern unsigned char code[MAX_TSG];
extern unsigned char rep[MAX_TSG];
extern unsigned char xtd[MAX_TSG];
extern ipcid_t dioid;
extern ipcid_t adid;
extern ipcid_t resetid;

extern bufnum;
extern bufsze;
extern void *bufadr[16];

void show_status(int full,int *astat,int *lstat) {
  int agc, lpwr;
  int i,j;
  char str[2][32];
  fprintf(stdout,"AGC Status        | LOPWR Status               \n");
  for (i=0;i<(15*full+1);i++) {
    agc=astat[i];
    lpwr=lstat[i];
    sprintf(str[0],"-----------------");
    sprintf(str[1],"-----------------");
    for (j=0;j<16;j++) {
      if ((agc & 0x01) !=0) str[0][j]='*';
      if ((lpwr & 0x01) !=0) str[1][j]='*';
      agc=agc>>1;
      lpwr=lpwr>>1;
    }
    fprintf(stdout,"%s | %s\n",str[0],str[1]);
  }       
}

int test_beam(char *arg) {
  int beam=-1;
  char str[16];
  if (arg !=NULL) beam=atoi(arg);
  while ((beam<0) || (beam>15)) {
    fprintf(stdout,"Please enter a beam number between 0 and 15:");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) beam=atoi(str);     
    if (str[0]=='.') return 1;
  } 
  fprintf(stdout,"Setting beam to:%d\n",beam);
  return DIOSetBeam(dioid,beam);
}


int test_phase(char *arg) {
  int phase=-1,beam;
  char str[16];
  if (arg !=NULL) {
    char *astr;
    astr=strtok(arg," ");
    if (astr !=NULL) phase=atoi(astr);
    astr=strtok(NULL," "); 
    if (astr !=NULL) beam=atoi(astr);
    astr=strtok(NULL," "); 
  }
  while ((phase<-127) || (phase>128)) {
    fprintf(stdout,"Please enter a phase between -127 and 128:");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) phase=atoi(str);     
    if (str[0]=='.') return 1;
  } 
  while ((beam<0) || (beam>15)) {
    fprintf(stdout,
      "Please enter the beam number between 0 and 15:");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) beam=atoi(str);     
    if (str[0]  =='.') return 1;

  } 

  fprintf(stdout,"Setting phase to:%d (%d)\n",phase,beam);
  return DIOSetPhase(dioid,phase,beam);
}


int test_freq(char *arg) {
  int freq=-1;
  int status=0;
  char str[16];
  if (arg !=NULL) freq=atoi(arg);
  while ((freq<8000) || (freq>25000)) {
    fprintf(stdout,"Please enter a frequency:");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) freq=atoi(str);     
    if (str[0]=='.') return 1;
  } 
  fprintf(stdout,"Setting frequency to:%d\n",freq);
  status=DIOSetFreq(dioid,freq);
  if (status==LOCAL_FREQ) fprintf(stdout,"Synthesizer in local mode\n");
  return status;
}


int test_gain(char *arg) {
  int atten=-1;
  char str[16];
  if (arg !=NULL) atten=atoi(arg);
  while ((atten<0) || (atten>10)) {
    fprintf(stdout,"Please enter an attenuation level:");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) atten=atoi(str);     
    if (str[0]=='.') return 1;
  } 
  fprintf(stdout,"Setting gain to:%d\n",atten);
  return DIOSetGain(dioid,atten);
}

int test_filter(char *arg) {
  int filter=-1;
  char str[16];
  if (arg !=NULL) filter=atoi(arg);
  while ((filter<0) || (filter>2)) {
    fprintf(stdout,"Please enter a filter value:");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) filter=atoi(str);     
    if (str[0]=='.') return 1;
  } 
  fprintf(stdout,"Setting filter to:%d\n",filter);
  return DIOSetFilter(dioid,filter);
}


int test_channel(char *arg) {
  int channel=-1;
  char str[16];
  char cstr[2]={'a','b'};
  if (arg !=NULL) if (arg[0]=='a') channel=0;
  else if (arg !=NULL) channel=1;

  while ((channel<0) || (channel>1)) {
    fprintf(stdout,"Please enter a channel (a|b):");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0]=='a') channel=0;    
    if (str[0]=='b') channel=1;
    if (str[0]=='.') return 1;
  } 
  fprintf(stdout,"Setting channel to:%c\n",cstr[channel]);
  return DIOSetChannel(dioid,channel);
}

int test_antenna(char *arg) {
  int anten=-1;
  int mode=-1;
  char str[16];
  if (arg !=NULL) anten=atoi(strtok(arg," "));
  while ((anten<0) || (anten>15)) {
    fprintf(stdout,"Please enter an antenna number between 0 and 15:");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) anten=atoi(str);     
    if (str[0]=='.') return 1;
  } 
  if (arg !=NULL) {
    char *mstr=NULL;
    mstr=strtok(NULL," ");
    if (mstr !=NULL) {
      if (mstr[0]=='f') mode='f';
      if (mstr[0]=='a') mode='a';
      if (mstr[0]=='b') mode='b';
    }
  }
  if (mode<0) {
    while (mode==-1) {
      fprintf(stdout,"Set antenna mode (auto/beam/fix):");
      fflush(stdout);
      fgets(str,16,stdin);
      if (str[0]=='f') mode='f';
      if (str[0]=='a') mode='a';
      if (str[0]=='b') mode='b';
      if (str[0]=='.') return 1;
    }
  }
  fprintf(stdout,"Setting antenna to:%d ",anten);
  if (mode=='f') fprintf(stdout,"(fix)\n");
  else if (mode=='a') fprintf(stdout,"(auto)\n");
  else fprintf(stdout,"(beam)\n");
  return DIOSetAntenna(dioid,mode,anten);
}

int test_mode(char *arg) {
  int mode=-1;
  if ((arg !=NULL) && (strcmp(arg,"on")==0)) mode=1;
  else if (arg !=NULL) mode=0;
  else if (arg==NULL) {
    char str[16];
    while (mode==-1) {
      fprintf(stdout,"Set test mode (on/off):");
      fflush(stdout);
      fgets(str,16,stdin);
      if ((str[0] !=0) && (strcmp(str,"on")==0)) mode=1;
      else if (str[0] !=0) mode=0;
      if (str[0]=='.') return 1;
    }
  } 
  return DIOSetTestMode(dioid,mode);
}

void test_fstatus(char *arg) {
  int mode;
  mode=DIOReadFreqMode(dioid);
  if (mode==0) fprintf(stdout,"Remote mode\n"); 
  else fprintf(stdout,"Local mode\n"); 
  return; 
}


void test_verify(char *arg) {
  int tsgid=-1;
  int status;
  char str[16];
  if (arg !=NULL) tsgid=atoi(arg);
  while ((tsgid<0) || (tsgid>31)) {
    fprintf(stdout,"Please enter a timing sequence between 0 and 32:");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) tsgid=atoi(str);     
    if (str[0]  =='.') {
      fprintf(stdout,"Command:Aborted\n");
      return;
    }
  } 
  fprintf(stdout,"Verifying timing sequence:%d\n",tsgid);
  status=DIOVerifyID(dioid,tsgid);
  if (status<0) fprintf(stdout,"Command:Failed\n");
  else {
    fprintf(stdout,"Command:Ok\n");
    if (status==1) fprintf(stdout,"Sequence:Present\n");
    else fprintf(stdout,"Sequence:Missing\n");
  }
}

void test_status(char *arg) {
  int status;
  int astat[16];
  int lstat[16];
  
  int clr=-1,full=-1;

  if (arg !=NULL) {
    char *cstr=NULL;
    cstr=strtok(arg," ");
    if ((cstr !=NULL) && (cstr[0]=='c')) clr=1;
    else if (cstr !=NULL) clr=0;
  }
  if (clr==-1) {
    char str[16];
    while (clr==-1) {
      fprintf(stdout,"Clear or leave status after test (clear/leave):");
      fflush(stdout);
      fgets(str,16,stdin);
      if (str[0]=='c') clr=1;
      else if (str[0] !=0) clr=0;
      if (str[0]=='.') {
         fprintf(stdout,"Command:Aborted\n");
         return;
      }
    }
  }
  if (arg !=NULL) {
    char *fstr=NULL;
    fstr=strtok(NULL," ");
    if ((fstr !=NULL) && (fstr[0]=='f')) full=1;
    else if (fstr !=NULL) full=0;
  }
  if (full==-1) {
    char str[16];
    while (full==-1) {
      fprintf(stdout,"Get full or partial (full/partial):");
      fflush(stdout);
      fgets(str,16,stdin);
      if (str[0]=='f') full=1;
      else if (str[0] !=0) full=0;
      if (str[0]=='.') {
         fprintf(stdout,"Command:Aborted\n");
         return;
      }
    }
  }
  fprintf(stdout,"Getting status:");
  if (clr==1) fprintf(stdout,"(clear) ");
  else fprintf(stdout,"(leave) ");
  if (full==1) fprintf(stdout,"(full)\n");
  else fprintf(stdout,"(partial)\n");
  status=DIOGetStatus(dioid,clr,full,lstat,astat);
  if (status<0) fprintf(stdout,"Command:Failed\n");
  else {
    fprintf(stdout,"Command:Ok\n");
    show_status(full,astat,lstat);
  }
}

int test_fclr(char *arg) {
  char chstr[2]={'A','B'};
  int stereo=-1;
  int flist[2][64];
  char str[32];
  int tsgid[2]={-1,-1};
  unsigned char seqid[2];
  int delay[2]={-1,-1};
  int fnum[2]={0,0};
  int *fptr[2];
  int i,j;
  int status;
  fptr[0]=flist[0];
  fptr[1]=flist[1];

  if (arg !=NULL) {
    char *fstr=NULL;
    fstr=strtok(arg," ");
    if ((fstr !=NULL) && (fstr[0]=='s')) stereo=1;
    else if (fstr !=NULL) stereo=0;
    for (j=0;j<(stereo==1)+1;j++) {
      fstr=strtok(NULL," ");
      if (fstr !=NULL) tsgid[j]=atoi(fstr);
      fstr=strtok(NULL," ");
      if ((stereo==1) && (fstr !=NULL)) delay[j]=atoi(fstr); 
      fstr=strtok(NULL," ");
      while (fstr !=NULL) {
        if ((fstr[0]=='x') || (fstr[0]=='X')) break;
        flist[j][fnum[j]]=atoi(fstr);
        if ((flist[j][fnum[j]]>8000) && (flist[j][fnum[j]]<25000)) fnum[j]++;
        fstr=strtok(NULL," ");
      }
    }
  }     

  if (stereo==-1) {
    while (stereo==-1) {
      fprintf(stdout,"Stereo operation (normal/stereo):");
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
      if (str[0] !=0) tsgid[j]=atoi(str);     
      if (str[0]  =='.') return 1;
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

    if (fnum[j]==0) {
      while(1) {
        fprintf(stdout,"Frequency %d:",fnum[j]);
        fflush(stdout);
        fgets(str,32,stdin);
        if (str[0]=='.') return 1;
        if ((str[0]=='x') && (fnum[j]>0)) break;
        flist[j][fnum[j]]=atoi(str);
        if ((flist[j][fnum[j]]>8000) && (flist[j][fnum[j]]<25000)) fnum[j]++;
      }
    }
  }

  if (stereo==1) fprintf(stdout,"Transmitting in Stereo mode.\n\n");

  for (j=0;j<stereo+1;j++)  {  
    if (stereo==1) fprintf(stdout,"Channel:%c\n\n",chstr[j]);
  
    fprintf(stdout,"Using sequence id:%d\n",tsgid[j]);
    if (stereo==1) fprintf(stdout,"Delay:%d\n",delay[j]);

    fprintf(stdout,"Sending frequences:\n%d",flist[j][0]);
    for (i=1;i<fnum[j];i++) {
      if ((i % 8)==0) fprintf(stdout,"\n");
      else fprintf(stdout,",");
      fprintf(stdout,"%d",flist[j][i]);
    }
    fprintf(stdout,"\n");
  }
  seqid[0]=tsgid[0];
  seqid[1]=tsgid[1];

  status=DIOSendFCLR(dioid,stereo,seqid,delay,fnum,fptr);
  if (status==LOCAL_FREQ) fprintf(stdout,"Synthesizer in local mode\n");
  return status; 

}

int load_tsg(char *arg) {
  int tsgid=-1;
  int status;
  char delim[]={' ','\n'};
  FILE *fp;
  char str[128];
  char *fname=NULL;
  if (arg !=NULL) {
    char *str;
    str=strtok(arg," ");
    if (str !=NULL) tsgid=atoi(str);
    fname=strtok(NULL," ");
  }
  while ((tsgid<0) || (tsgid>31)) {
    fprintf(stdout,"Please enter a timing sequence between 0 and 32:");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) tsgid=atoi(str);     
    if (str[0]  =='.') return 1;
  } 

  if (fname==NULL) {
    while (fname==NULL) {
      fprintf(stdout,"Enter timing sequence file name:");
      fflush(stdout);
      fgets(str,128,stdin);
      if (str[0] =='.') return 1;
      if (str[0] !=0) fname=strtok(str,delim);
    }
  }
  fprintf(stdout,"Filename:%s\n",fname);
  fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stdout,"File not found.\n");
    return -1;
  }
  status=read_tsg(fp);
  if (status !=0) return -1;
  fprintf(stdout,"Timing sequence length:%d\n",cnt);
  if (ex !=0) fprintf(stdout,
	"Timing sequence contains extended information.\n");
  if (ex !=0) return DIOSetTSG(dioid,tsgid,cnt,code,rep,xtd); 
  status=DIOSetTSG(dioid,tsgid,cnt,code,rep,NULL);
  return status;
}

int dump_ad(char *arg) {
  int bval=-1;
  int size=-1;
  int status;
  char delim[]={' ','\n'};
  FILE *fp;
  char *fname=NULL;
  char str[128];

  if (arg !=NULL) {
    char *str;
    str=strtok(arg," ");
    if (str !=NULL) bval=atoi(str);
    str=strtok(NULL," "); 
    if (str !=NULL) size=atoi(str);
    fname=strtok(NULL," ");
  }
  while ((bval<0) || (bval>=bufnum)) {
    fprintf(stdout,"Please enter a buffer between 0 and %d:",bufnum-1);
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) bval=atoi(str);     
    if (str[0]  =='.') return 1;

  } 
  while ((size<1) || (size>bufsze)) {
    fprintf(stdout,
      "Please enter the number of bytes to store between 1 and %d:",bufsze);
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) size=atoi(str);     
    if (str[0]  =='.') return 1;
  } 
  if (fname==NULL) {
    while (fname==NULL) {
      fprintf(stdout,"Enter file name:");
      fflush(stdout);
      fgets(str,128,stdin);
      if (str[0]=='.') return 1;
      if (str[0] !=0) fname=strtok(str,delim);
    }
  }
  fprintf(stdout,"Filename:%s\n",fname);
  fprintf(stdout,"Storing %d bytes from buffer %d.\n",size,bval);
  if (bufadr[bval]==0) return -1;
  fp=fopen(fname,"w"); 
  if (fp==NULL) return -1;
  status=fwrite(bufadr[bval],size,1,fp);
  fclose(fp);
  if (status !=1) return -1;
  return 0;
}

int test_proxy(char *arg) {
  fprintf(stdout,"Triggering scan reset proxy on A/D driver.\n");
  return ADScanReset(adid,resetid);
}

void test_ad_stat(char *arg) {
  int status=0;
  status=ADGetScanStatus(adid);
  if (status !=0) fprintf(stdout,"A/D Status:Failed\n");
   else fprintf(stdout,"A/D Status:Ok\n");
}


int test_transfer(char *arg) {
  int bval=-1;
  int size=-1;
  int channel=-1;
  int mode=-1;
  char str[16];
  if (arg !=NULL) {
    char *astr;
    astr=strtok(arg," ");
    if (astr !=NULL) bval=atoi(astr);
    astr=strtok(NULL," "); 
    if (astr !=NULL) size=atoi(astr);
    astr=strtok(NULL," "); 
    if (astr !=NULL) channel=atoi(astr);
	astr=strtok(NULL," ");
    if ((astr !=NULL) && (astr[0]=='h')) mode=0;
    else if (astr !=NULL) mode=1;
  }
  while ((bval<0) || (bval>=bufnum)) {
    fprintf(stdout,"Please enter a buffer between 0 and %d:",bufnum-1);
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) bval=atoi(str);     
    if (str[0]  =='.') return 1;
  } 
  while ((size<1) || (size>bufsze)) {
    fprintf(stdout,
      "Please enter the number of bytes to transfer between 1 and %d:",
	   bufsze);
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) size=atoi(str);     
    if (str[0]  =='.') return 1;

  } 
  while ((channel<1) || (channel>4)) {
    fprintf(stdout,"Please enter the number of channels between 1 and 4:");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0] !=0) channel=atoi(str);     
    if (str[0]  =='.') return 1;
  } 
  while (mode==-1) {
    fprintf(stdout,"Use hardware or software trigger (hard/soft):");
    fflush(stdout);
    fgets(str,16,stdin);
    if (str[0]=='.') return 1;
    if (str[0]=='h') mode=0;
    else if (str[0] !=0) mode=1;
  }
  fprintf(stdout,"Transfering %d bytes to buffer %d using %d channels.\n",
		  size,bval,channel);
  if (mode==0) {
    fprintf(stdout,"Using hardware triggered A/D.\n");
    fprintf(stdout,
	"WARNING: A/D transfer will not occur without a sample pulse train.\n");
  } else fprintf(stdout,"Using software triggered A/D.\n");
  
  return ADDoScan(adid,bval,size,mode,channel);
}

