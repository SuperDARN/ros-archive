/* rawdump.c
   =========
   Author: R.J.Barnes
*/

/*
 $Log: rawdump.c,v $
 Revision 1.10  2006/09/22 15:33:37  code
 Fixed bug in including alternative lag zero power.

 Revision 1.9  2006/09/22 15:28:53  code
 Added the alternative lag-zero power to the lag table.

 Revision 1.8  2006/08/23 19:56:17  code
 Added sample delay associated with digital receiver.

 Revision 1.7  2006/08/18 15:39:42  code
 Fixed bugs in getting the lag table.

 Revision 1.6  2006/08/17 21:31:15  code
 Added support for beam and frequency.

 Revision 1.5  2006/05/10 18:16:39  barnes
 Fixed bug in not freeing buffers after they are written.

 Revision 1.4  2006/04/11 19:06:27  barnes
 Added output in DataMap format.

 Revision 1.3  2006/04/07 20:16:18  barnes
 Fixed missing attenuation setting.

 Revision 1.2  2006/04/07 20:10:48  barnes
 Removed redundant task name.

 Revision 1.1  2006/04/07 20:05:43  barnes
 Initial revision
 
*/

#include <stdio.h>
#include <string.h>
#include <sys/name.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include "rtypes.h"
#include "dmap.h"
#include "rtime.h"
#include "option.h"
#include "limit.h"
#include "tsg.h"
#include "echoraw.h"
#include "ad.h"
#include "taskname.h"
#include "hlpstr.h"

/*
 $Log: rawdump.c,v $
 Revision 1.10  2006/09/22 15:33:37  code
 Fixed bug in including alternative lag zero power.

 Revision 1.9  2006/09/22 15:28:53  code
 Added the alternative lag-zero power to the lag table.

 Revision 1.8  2006/08/23 19:56:17  code
 Added sample delay associated with digital receiver.

 Revision 1.7  2006/08/18 15:39:42  code
 Fixed bugs in getting the lag table.

 Revision 1.6  2006/08/17 21:31:15  code
 Added support for beam and frequency.

 Revision 1.5  2006/05/10 18:16:39  barnes
 Fixed bug in not freeing buffers after they are written.

 Revision 1.4  2006/04/11 19:06:27  barnes
 Added output in DataMap format.

 Revision 1.3  2006/04/07 20:16:18  barnes
 Fixed missing attenuation setting.

 Revision 1.2  2006/04/07 20:10:48  barnes
 Removed redundant task name.

 Revision 1.1  2006/04/07 20:05:43  barnes
 Initial revision

*/

#define TRIGGER 't'
#define SCAN 's'
#define SET 'S'

int bufnum;
int bufsze;
char *bufadr[32];

int ptr=0;
int max=1024*1024;
char buf[1024*1024];

int num=0;

struct mark {
  struct timespec tval;
  int atten;
  int chn;
  int off;
  int sze;
};

struct mark mark[1024];

char *echoraw=NULL;
char echorawd[32]={ECHORAW};

char *fname=NULL;

char fnamed[32]={"raw.dat"};


int arg=0;
struct OptionData opt;

int main (int argc,char *argv[]) {
  int i;

  int help=0;
  pid_t rid;

  ipcid_t did;

  char rmsg;
  int status=0;
  int size=0;
  int bufnum;
  int numchn;
  int numsmp;
  int atten=0;

  int ascii=0;


  struct timespec tval;
  struct TSGprm prm;
  int mplgs;
  int lagnum;
  int rsmpdelay=0;
  int iatten=0;
  int pat[PULSE_SIZE];
  int lag[LAG_SIZE*2];
  float noise;
  int tfreq;
  int bmnum;



  int setflg=0;
  struct timespec stval;

  struct _mxfer_entry msg[7];

  int16 *sptr;

  int snum;
  int n,c;
  FILE *fp;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"e",'t',&echoraw);

  OptionAdd(&opt,"ascii",'x',&ascii);
  OptionAdd(&opt,"f",'t',&fname);

  arg=OptionProcess(1,argc,argv,&opt,NULL);  
 
  if (echoraw==NULL) echoraw=echorawd;

  if (fname==NULL) fname=fnamed;

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if ((did=EchoRawRegister("rawdump",echoraw))==-1) {
     fprintf(stderr,"Could not register with echoraw task.\n");
     exit(-1);
  }


  bufnum=ADGetBufNum(did);
  bufsze=ADGetBufSize(did);
  for (i=0;i<bufnum;i++) bufadr[i]=ADGetBufAdr(did,i);


  while (1) {
    rid=Receive(0,&rmsg,sizeof(char));
    switch (rmsg) { 
    case TRIGGER:
      status=0;
      _setmx(&msg[0],&status,sizeof(int));   
      Replymx(rid,1,msg);
      if (setflg==0) continue;
      fprintf(stderr,"Writing %d blocks to %s.\n",num,fname);
      fp=fopen(fname,"a");
      if (ascii) {
        int yr,mo,dy,hr,mt;
        double sc;
        
        TimeEpochToYMDHMS((int) stval.tv_sec,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(fp,"%.4d %.2d %.2d %.2d %.2d %.2d %d\n",
                yr,mo,dy,hr,mt,(int) sc,(int) stval.tv_nsec);
        fprintf(fp,"%d %d %d %d %d\n",
                prm.nrang,prm.frang,prm.rsep,prm.smsep,prm.lagfr);
        fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d\n",  
                prm.txpl,prm.mppul,prm.mpinc,
                prm.mlag,prm.samples,prm.smdelay,prm.rtoxmin,mplgs,
		lagnum,rsmpdelay,iatten);
        fprintf(fp,"%f %d %d\n",noise,bmnum,tfreq);
        for (i=0;i<prm.mppul;i++) {
          if (i !=0) fprintf(fp," ");
          fprintf(fp,"%d",pat[i]);
        }
        fprintf(fp,"\n");
        for (i=0;i<=lagnum;i++) {
          if (i !=0) fprintf(fp," ");
          fprintf(fp,"%d",lag[2*i]);
        }
        fprintf(fp,"\n");
        for (i=0;i<=lagnum;i++) {
          if (i !=0) fprintf(fp," ");
          fprintf(fp,"%d",lag[2*i+1]);
        }
        fprintf(fp,"\n");
        fprintf(fp,"%d\n",num);
        for (n=0;n<num;n++) {
          sptr=(int16 *) (buf+mark[n].off);
          snum=mark[n].sze/(4*mark[n].chn);

          fprintf(fp,"%d %d %d %d %d\n",
                (int) mark[n].tval.tv_sec,(int) mark[n].tval.tv_nsec,
                mark[n].chn,snum,mark[n].atten);

          for (c=0;c<mark[n].sze;c+=2) {
            fprintf(fp,"%d %d\n",sptr[c],sptr[c+1]);
          }
        }
      } else {
        struct DataMap *data;
        int yr,mo,dy,hr,mt,sc,us,s;
        double dsc;
        int32 pnum;
        int32 lnum[2];
        int32 tnum,snum;

        int32 tsc[1024];
        int32 tus[1024];
        int32 tatten[1024];
        int32 tchn[1024];
        int32 toff[1024];
        int32 tsze[1024];
        
        data=DataMapMake();
        sptr=(int16 *) buf;

        TimeEpochToYMDHMS((int) stval.tv_sec,&yr,&mo,&dy,&hr,&mt,&dsc);
        sc=(int) dsc;
        us=(int) stval.tv_nsec/1000.0;
        
        DataMapAddScalar(data,"time.yr",DATAINT,&yr);
        DataMapAddScalar(data,"time.mo",DATAINT,&mo);
        DataMapAddScalar(data,"time.dy",DATAINT,&dy);
        DataMapAddScalar(data,"time.hr",DATAINT,&hr);
        DataMapAddScalar(data,"time.mt",DATAINT,&mt);
        DataMapAddScalar(data,"time.sc",DATAINT,&sc);
        DataMapAddScalar(data,"time.us",DATAINT,&us);
        DataMapAddScalar(data,"nrang",DATAINT,&prm.nrang);
        DataMapAddScalar(data,"frang",DATAINT,&prm.frang);
        DataMapAddScalar(data,"rsep",DATAINT,&prm.rsep);
        DataMapAddScalar(data,"smsep",DATAINT,&prm.smsep);
        DataMapAddScalar(data,"lagfr",DATAINT,&prm.lagfr);
        DataMapAddScalar(data,"txpl",DATAINT,&prm.txpl);
        DataMapAddScalar(data,"mppul",DATAINT,&prm.mppul);
        DataMapAddScalar(data,"mpinc",DATAINT,&prm.mpinc);
        DataMapAddScalar(data,"mlag",DATAINT,&prm.mlag);
        DataMapAddScalar(data,"samples",DATAINT,&prm.samples);
        DataMapAddScalar(data,"smdelay",DATAINT,&prm.smdelay);
        DataMapAddScalar(data,"rtoxmin",DATAINT,&prm.rtoxmin);
        DataMapAddScalar(data,"mplgs",DATAINT,&mplgs);
        DataMapAddScalar(data,"lagnum",DATAINT,&lagnum); 
        DataMapAddScalar(data,"rsmdelay",DATAINT,&rsmpdelay);

        DataMapAddScalar(data,"iatten",DATAINT,&iatten);
        DataMapAddScalar(data,"noise",DATAFLOAT,&noise);
        DataMapAddScalar(data,"bmnum",DATAINT,&bmnum);
        DataMapAddScalar(data,"tfreq",DATAINT,&tfreq);

	

        DataMapAddArray(data,"ptab",DATAINT,1,&pnum,pat);
        DataMapAddArray(data,"ltab",DATAINT,2,lnum,lag);

        DataMapAddArray(data,"tsc",DATAINT,1,&tnum,tsc);
        DataMapAddArray(data,"tus",DATAINT,1,&tnum,tus);
        DataMapAddArray(data,"tatten",DATAINT,1,&tnum,tatten);
        DataMapAddArray(data,"tchn",DATAINT,1,&tnum,tchn);
        DataMapAddArray(data,"toff",DATAINT,1,&tnum,toff);
        DataMapAddArray(data,"tsze",DATAINT,1,&tnum,tsze);

        DataMapAddArray(data,"data",DATASHORT,1,&snum,sptr);
	
        pnum=prm.mppul;
        lnum[0]=2;
        lnum[1]=lagnum+1;
	tnum=num;
        snum=ptr/2;

        for (n=0;n<num;n++) {
          tsc[n]=(int) mark[n].tval.tv_sec;
          tus[n]=(int) mark[n].tval.tv_nsec/1000.0;
          tatten[n]=mark[n].atten;
          tchn[n]=mark[n].chn;
          toff[n]=mark[n].off;
          tsze[n]=mark[n].sze;
	}

      

        s=DataMapWrite(fileno(fp),data);

        DataMapFree(data);
      }


      fclose(fp);        
      num=0;
      ptr=0;
      setflg=0;
      break;
    case SCAN:
      _setmx(&msg[0],&size,sizeof(int));
      _setmx(&msg[1],&tval,sizeof(struct timespec));
      _setmx(&msg[2],&bufnum,sizeof(bufnum));
      _setmx(&msg[3],&numsmp,sizeof(numsmp));
      _setmx(&msg[4],&numchn,sizeof(numchn));
      _setmx(&msg[5],&atten,sizeof(atten));
             
      Readmsgmx(rid,sizeof(char),6,msg);
      status=0;
      _setmx(&msg[0],&status,sizeof(int));   
      Replymx(rid,1,msg);
      if (setflg==0) continue;     
      if (num>=1024) continue;
      if (numchn==1) {
	  if (ptr+numsmp*4>max) continue;
        memcpy(buf+ptr,bufadr[bufnum],numsmp*4);
        mark[num].tval.tv_sec=tval.tv_sec;
        mark[num].tval.tv_nsec=tval.tv_nsec;
        mark[num].chn=numchn;
        mark[num].off=ptr;
        mark[num].sze=numsmp*4;
        mark[num].atten=atten;
        ptr+=numsmp*4;
      } else {
	  if (ptr+numsmp*4*2>max) continue;
        memcpy(buf+ptr,bufadr[bufnum],numsmp*4*2);
        mark[num].tval.tv_sec=tval.tv_sec;
        mark[num].tval.tv_nsec=tval.tv_nsec;
        mark[num].chn=numchn;
        mark[num].off=ptr;
        mark[num].sze=numsmp*4*2;
        mark[num].atten=atten;
        ptr+=numsmp*4*2;
      }
      num++;
      break;
    case SET:
  _setmx(&msg[0],&size,sizeof(int));
      _setmx(&msg[1],&tval,sizeof(struct timespec));
      _setmx(&msg[2],&prm,sizeof(struct TSGprm));

      Readmsgmx(rid,sizeof(char),3,msg);
      _setmx(&msg[0],pat,sizeof(int)*prm.mppul);
      _setmx(&msg[1],&mplgs,sizeof(int));
      _setmx(&msg[2],&lagnum,sizeof(int));
 
      Readmsgmx(rid,sizeof(char)+sizeof(int)+
                sizeof(struct timespec)+sizeof(struct TSGprm),3,msg);

      _setmx(&msg[0],lag,sizeof(int)*(lagnum+1)*2);

      Readmsgmx(rid,sizeof(char)+sizeof(int)+
                sizeof(struct timespec)+sizeof(struct TSGprm)+
                sizeof(int)*(prm.mppul+2),1,msg);
 
      _setmx(&msg[0],&rsmpdelay,sizeof(int)); 
      _setmx(&msg[1],&iatten,sizeof(int));
      _setmx(&msg[2],&noise,sizeof(float));
      _setmx(&msg[3],&bmnum,sizeof(int));
      _setmx(&msg[4],&tfreq,sizeof(int));

      Readmsgmx(rid,sizeof(char)+sizeof(int)+
                sizeof(struct timespec)+sizeof(struct TSGprm)+
                sizeof(int)*(prm.mppul+2+2*(lagnum+1)),5,msg);





      _setmx(&msg[0],&status,sizeof(int));   
      status=0;
      Replymx(rid,1,msg);
      stval.tv_sec=tval.tv_sec;
      stval.tv_nsec=tval.tv_nsec;
      setflg=1;
    default:
      break;
    }    
  }
  return 0;
} 
