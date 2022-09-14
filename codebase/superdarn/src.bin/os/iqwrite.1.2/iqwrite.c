/* iqwrite
    =========== 
   Author: R.J.Barnes
*/

/*
 $License$
*/

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtypes.h"
#include "option.h"
#include "tcpipmsg.h"

#include "limit.h"
#include "rprm.h"
#include "iqdata.h"
#include "radar.h"
#include "rmsg.h"
#include "rmsgrcv.h"
#include "fio.h"
#include "shmem.h"
#include "iqwrite.h"
#include "errlog.h"

#include "errstr.h"
#include "hlpstr.h"

#include "version.h"

#define DEF_PORT 44100

struct OptionData opt;

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;

struct RMsgBlock rblk;
unsigned char *store=NULL; 

struct DMsg {
  int tag;
  struct RadarParm *prm;
  struct IQData *iqdt;
  unsigned int *badtr;
  char *iqs;
  int *iqoff;
};

int dnum=0;
int dptr=0;
struct DMsg dmsg[32];

char *chn;

char *taskname="iqwrite";

char *errhost=NULL;
char *derrhost="127.0.0.1";
int errport=44000;
int errsock=-1;

char errbuf[1024];

float thr=0.0;

int operate(pid_t parent,int sock) {
  FILE *fp;
  int s,i;
  int msg,rmsg;
  int flg=0;

  char *filename=NULL;

  unsigned char *cbufadr=NULL;
  size_t cbuflen;

  while(1) {

    s=TCPIPMsgRecv(sock,&msg,sizeof(int));

    if (s !=sizeof(int)) break;
    rmsg=TASK_OK;
    switch (msg) {
    
    case TASK_OPEN:
      ErrLog(errsock,taskname,"Opening file.");
      rmsg=RMsgRcvDecodeOpen(sock,&cbuflen,&cbufadr);

      if (rmsg==TASK_OK) flg=1;
      break;
    case TASK_CLOSE:
      ErrLog(errsock,taskname,"Closing file.");
      if (filename !=NULL) free(filename);
      filename=NULL;
      break;
    case TASK_RESET:
      ErrLog(errsock,taskname,"Reset.");
      if (filename !=NULL) free(filename);
      filename=NULL;
      break;
    case TASK_QUIT:
      ErrLog(errsock,taskname,"Stopped.");
      TCPIPMsgSend(sock,&rmsg,sizeof(int));
      exit(0);
      break;
    case TASK_DATA:
      ErrLog(errsock,taskname,"Received Data.");
      rmsg=RMsgRcvDecodeData(sock,&rblk,&store);    
    default:
      break;  
    }
    TCPIPMsgSend(sock,&rmsg,sizeof(int));

    if (msg==TASK_DATA) {
      dnum=0;
      for (i=0;i<rblk.num;i++) {
        for (dptr=0;dptr<dnum;dptr++) 
          if (dmsg[dptr].tag==rblk.data[i].tag) break;
          if (dptr==dnum) {
            dmsg[dptr].tag=rblk.data[i].tag;
            dmsg[dptr].prm=NULL;
            dmsg[dptr].badtr=NULL;
            dmsg[dptr].iqdt=NULL;
            dmsg[dptr].iqs=NULL;
            dmsg[dptr].iqoff=NULL;
            dnum++;
	  }
          switch (rblk.data[i].type) {
	  case PRM_TYPE:
            dmsg[dptr].prm=(struct RadarParm *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          case IQ_TYPE:
            dmsg[dptr].iqdt=(struct IQData *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          case BADTR_TYPE:
            dmsg[dptr].badtr=(unsigned int *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          case IQS_TYPE:
            dmsg[dptr].iqs=(char *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          case IQO_TYPE:
            dmsg[dptr].iqoff=(int *)
	      (rblk.ptr[rblk.data[i].index]);
            break;          
          default:
            break;
	  }
      }
      if ((filename==NULL) && (flg !=0)) {
        flg=0;
        if ((dnum>0) && (dmsg[0].prm !=NULL)) {
         
          filename=FIOMakeFile(getenv("SD_IQ_PATH"),
			       dmsg[0].prm->time.yr,
                               dmsg[0].prm->time.mo,
                               dmsg[0].prm->time.dy,
                               dmsg[0].prm->time.hr,
                               dmsg[0].prm->time.mt,
                               dmsg[0].prm->time.sc,
			       RadarGetCode(network,dmsg[0].prm->stid,0),
			       chn,"iqdat",0,0);
	}
        if (filename==NULL) ErrLog(errsock,taskname,"Could not create file.");
      }
      if (filename !=NULL) {
        fp=fopen(filename,"a");
        if (fp==NULL) ErrLog(errsock,taskname,"Error opening file.");
	else {
          int fd,st;
          unsigned char *p;
          int offset=0;
          for (dptr=0;dptr<dnum;dptr++) {
            if (dmsg[dptr].iqs==NULL) continue; /* no buffer */
            if (dmsg[dptr].iqoff !=NULL) offset=*dmsg[dptr].iqoff;
            p=ShMemAlloc(dmsg[dptr].iqs,IQBUFSIZE,O_RDWR,0,&fd);
     
            s=IQFwrite(fp,dmsg[dptr].prm,
		       dmsg[dptr].iqdt,dmsg[dptr].badtr,
                       (int16 *) (p+offset));  
            st=ShMemFree(p,dmsg[dptr].iqs,IQBUFSIZE,0,fd);
            
            if (s==-1) break;
	  }
          if (s==-1) ErrLog(errsock,taskname,"Error writing record.");
          fclose(fp);
	}
      }
      if (store !=NULL) free(store);
      store=NULL;
    }  
  }

  return 0;
}

int main(int argc,char *argv[]) {
  FILE *fp=NULL;
  char *envstr=NULL;
  
  int port=DEF_PORT,arg=0;
  int sock;

  unsigned char help=0; 
  unsigned char option=0; 

  socklen_t length;
  socklen_t clength;

  struct sockaddr_in server;
  struct sockaddr_in client;

  fd_set ready;

  struct hostent *gethostbyname();
  pid_t root;

  int msgsock=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"lp",'i',&port);
  OptionAdd(&opt,"eh",'t',&errhost);
  OptionAdd(&opt,"ep",'i',&errport);
  OptionAdd(&opt,"t",'f',&thr);
  OptionAdd(&opt,"c",'t',&chn);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  if (errhost==NULL) errhost=derrhost;
  errsock=TCPIPMsgOpen(errhost,errport);

  

  sprintf(errbuf,"Started (version %d.%.2d) listening on port %d",
          MAJOR_VERSION,MINOR_VERSION,port);
  ErrLog(errsock,taskname,errbuf);  


  signal(SIGCHLD,SIG_IGN); 
  signal(SIGPIPE,SIG_IGN);

  root=getpid();

  sock=socket(AF_INET,SOCK_STREAM,0); /* create our listening socket */
  if (sock<0) {
    perror("opening stream socket");
    exit(1);
  }

  /* name and bind socket to an address and port number */

  server.sin_family=AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  if (port !=0) server.sin_port=htons(port); 
  else server.sin_port=0;
  
  if (bind(sock,(struct sockaddr *) &server,sizeof(server))) {
     perror("binding stream socket");
     exit(1);
  }

  /* Find out assigned port number and print it out */

  length=sizeof(server);
  if (getsockname(sock,(struct sockaddr *) &server,&length)) {
     perror("getting socket name");
     exit(1);
  }

  listen(sock,5); /* mark our socket willing to accept connections */
  
  do {

      /* block until someone wants to attach to us */

      FD_ZERO(&ready);
      FD_SET(sock,&ready);
      if (select(sock+1,&ready,0,0,NULL) < 0) { 
       perror("while testing for connections");
       continue;
      }
     
      /* Accept the connection from the client */

      fprintf(stdout,"Accepting a new connection...\n");
      clength=sizeof(client);
      msgsock=accept(sock,(struct sockaddr *) &client,&clength);
        
      if (msgsock==-1) {
         perror("accept"); 
         continue;
      }

      if (fork() == 0) {
        close(sock);
        operate(root,msgsock);
        exit(0);
      }
      close (msgsock);
  } while(1);

 
  return 0;

}
   

 






















