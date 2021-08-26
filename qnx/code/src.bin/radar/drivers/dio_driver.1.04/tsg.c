/* tsg.c
   ===== */
   
/*
 $Log: tsg.c,v $
 Revision 1.22  2000/09/18 17:57:09  barnes
 Removed redundant header.

 Revision 1.21  2000/03/19 20:03:46  barnes
 Modifications to show both pulse sequences as they are transmitted

 Revision 1.20  2000/03/19 18:33:10  barnes
 Fixed bug in freeing memory for the timing sequences.

 Revision 1.19  2000/03/17 15:39:32  barnes
 Implemented stereo clear frequency search.

 Revision 1.18  2000/02/03 12:53:23  barnes
 Attempt to fix problem with masking out unused bits in the
 timing sequence control word.

 Revision 1.17  1999/09/18 19:04:40  barnes
 Added the extended pulse sequence to the stereo mode.

 Revision 1.16  1999/09/18 17:36:45  barnes
 Modifications for stereo.

 Revision 1.15  1999/04/14 14:58:50  barnes
 Modifications to prevent deadlock if we lose the clock pulse.

 Revision 1.14  1999/04/02 15:49:34  barnes
 Modifications to support Frequency switching.

 Revision 1.13  1998/11/09 17:11:38  barnes
 Modifications to further support stereo operations - the send_tsg routine
 has been broken into two sections, one for stereo and one for mono.

 Revision 1.12  1998/11/08 19:11:58  barnes
 Modifications to support stereo operations.

 Revision 1.11  1998/11/07 00:19:16  barnes
 Extensive changes for stereo operations.
 Moved all of the hard coded configuration information into a configuration
 file.

 Revision 1.10  1998/11/06 01:19:57  barnes
 Implemented the majority of the Stereo Radar code. The remaining sections
 involve selecting the correct PIO port based on the channel number and the
 implementation of the channel switching timing sequence.

 Revision 1.9  1998/10/23 17:41:28  barnes
 Modifications for the extended timing sequence code.

 Revision 1.8  1998/09/11 17:33:25  root
 Modifications to remove short integer types.

 Revision 1.7  1998/06/03 19:10:24  root
 Modification to the version control system.

 Revision 1.6  1997/06/10 20:17:13  root
 Moved sd_radar_id.h header.

 Revision 1.5  1997/04/16 10:59:46  root
 Debug mode can now be set on the command line.

 Revision 1.4  1997/01/23 22:01:42  root
 Julian Thornhill added code for watchdog timer card.

 * Revision 1.3  1996/09/23  14:58:44  root
 * Added code for syowa radars.
 *
 * Revision 1.2  1996/06/05  14:46:53  root
 * Changed the debug mode so there is a delay before the send_tsg
 * function returns. This makes it more realistic.
 *
 * Revision 1.1  1996/03/13  15:13:31  root
 * Initial revision
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/qnxterm.h>
#include <sys/kernel.h>
#include <sys/name.h>
#include <sys/dev.h>

#include "freq_lib.h"

#include "main.h"
#include "DIO.h"
#include "PIO48.h"
#include "port.h"
#include "do_op.h"

#include "out_tsg.h"
#include "out_tsg_x.h"
#include "out_tsg_s.h"
#include "out_tsg_sx.h"

extern int watch_port;
extern int debug;
extern struct dio_hdw hdw;
extern struct dio_param param;
extern struct freq_table *fptr;

#define MAX_TSG 32
#define DIO_OK 0
#define DIO_ERR 'R'
       
/* manages the timing sequences */

int tsg_length[MAX_TSG];
unsigned char *repeat[MAX_TSG];
unsigned char *code[MAX_TSG];
unsigned char *xtd[MAX_TSG];
unsigned int tlen[MAX_TSG];

extern int dio_adr[2];

void init_tsg() {

  /* ensure that the timing sequence arrays are NULL initialised */  

  int i;    
  for (i=0;i<16;i++) {
    tsg_length[i]=0;
	repeat[i]=NULL;
    code[i]=NULL;
	xtd[i]=NULL;
  }
}

void zero_tsg() {

  /* free all the timing sequence arrays */

  int i;
    
  for (i=0;i<32;i++) {
        tsg_length[i]=0;
	if (repeat[i] !=NULL) free(repeat[i]);
	if (code[i] !=NULL) free(code[i]);
	if (xtd[i] !=NULL) free(xtd[i]);
        repeat[i]=NULL;
        code[i]=NULL; 
        xtd[i]=NULL;
  }
}
	
int get_length(char seq_id) {

  /* get the length of a timing sequence (used to verify id) */

  if ((code[seq_id] ==NULL) || (repeat[seq_id] == NULL))  return 0;
  return tsg_length[seq_id]; 
}

void download(pid_t pid) {

  /* download a timing sequence from the main computer */
  int status,i;
  struct _mxfer_entry msg[3];
  
  struct {
    char seq;
    char flag;
	int length; 
  } header;
   
  status=(Readmsg(pid,sizeof(char),&header,sizeof(header))!=sizeof(header));
  status=(header.length <=0); /* invalid sequence length */

  if (status ==0) {

    /* free memory for sequence already claiming this id */

    if (code[header.seq] !=NULL) free(code[header.seq]);
    if (repeat[header.seq] !=NULL) free(repeat[header.seq]);
    if (xtd[header.seq] !=NULL) free(xtd[header.seq]);

    /* claim memory to store sequence */

    if (status ==0)
	   status=((code[header.seq]=malloc(header.length)) ==NULL);
    if (status ==0) 
	   status=((repeat[header.seq]=malloc(header.length)) ==NULL);
    if ((status ==0) && (header.flag !=0)) 
	   status=((xtd[header.seq]=malloc(header.length)) ==NULL);
    if (status==0) { 
	  tsg_length[header.seq]=header.length;

    /* download the sequence */

	  status=(Readmsg(pid,sizeof(char)+sizeof(header),code[header.seq],
	          header.length) != header.length);
	  if (status==0) status=(Readmsg(pid,sizeof(char)+
	  		sizeof(header)+header.length,
	        repeat[header.seq],header.length) != header.length);	
	  if ((status==0) && (header.flag !=0)) status=(Readmsg(pid,sizeof(char)+
	  		sizeof(header)+2*header.length,
	        xtd[header.seq],header.length) != header.length);	

			  
      if (status==0) {	/* unpack the array */
        int beam;
        tlen[header.seq]=0;
	    for (i=0;i<header.length;i++) {
	      code[header.seq][i]=code[header.seq][i] | 0x80;
          if (xtd[header.seq] !=NULL) {
            beam=xtd[header.seq][i] & 0x0f;
            if (hdw.BEAM_inv !=0) beam=(beam ^ 0x0f) & 0x0f;  
            xtd[header.seq][i]=(xtd[header.seq][i] & 0xf0) | beam;
          }
          tlen[header.seq]+=1+repeat[header.seq][i];
	    }
	  }
	}  	
    code[header.seq][header.length-1]=
      code[header.seq][header.length-1] & 0x7f;			 	 
  }
  
  if (status ==0) header.seq=DIO_OK;
    else header.seq=DIO_ERR;
  Reply(pid,&header.seq,sizeof(header.seq));  
}

int send_seq_mono(int channel,
                  unsigned char seq_id,unsigned char xt_flag,int *ftable) {

  /* Send out a timing sequence */

  int s;
  unsigned char fbytes[6];
  unsigned char *f=NULL; 
  int fadr[3];
  int TSG_port=0,TSG_safe=0,TSG_mask=0,TSG_state=0,CLOCK_port=0,CLOCK_mask=0; 
  int BEAM_port=0,BEAM_mask=0,BEAM_state=0;

  if (hdw.micro_clock[channel]<0) return 0;
  if (hdw.TSG_port[channel]<0) return 0;
  if ((xt_flag & 0x02) && 
      ( (hdw.FMHz10_port[channel]<0) ||
        (hdw.FMKHz_port[channel]<0) ||
        (hdw.FKHz_port[channel]<0) ) ) xt_flag=xt_flag & 0x01;
  if ((xt_flag & 0x01) && 
	 (hdw.BEAM_port[channel]<0)) xt_flag=xt_flag & 0x02;     

  if ((code[seq_id] ==NULL) || (repeat[seq_id] == NULL))  return -1;
  if ((xt_flag !=0) && (xtd[seq_id]==NULL)) xt_flag=0; 
  if (((xt_flag & 0x02) !=0) && (ftable==NULL)) 
     xt_flag=xt_flag & 0x01; 

 

  if (xt_flag & 0x02) {
    /* decode the frequency tables here */
    cnv_freq(ftable[0],fbytes);
    cnv_freq(ftable[1],fbytes+3);
    f=fbytes;
    fadr[2]=DIO_adr(hdw.FMHz10_port[channel]);
    fadr[1]=DIO_adr(hdw.FMKHz_port[channel]);
    fadr[0]=DIO_adr(hdw.FKHz_port[channel]); 
    
    /* set the bytes to reflect the current port values */
    fbytes[0]=DIO_value(hdw.FMHz10_port[channel],fbytes[0],
	                    hdw.FMHz10_mask[channel]);
    fbytes[1]=DIO_value(hdw.FMKHz_port[channel],fbytes[1],
	                    hdw.FMKHz_mask[channel]);
    fbytes[2]=DIO_value(hdw.FKHz_port[channel],fbytes[2],
	                    hdw.FKHz_mask[channel]);
    fbytes[3]=DIO_value(hdw.FMHz10_port[channel],fbytes[3],
	                    hdw.FMHz10_mask[channel]);
    fbytes[4]=DIO_value(hdw.FMKHz_port[channel],fbytes[4],
	                    hdw.FMKHz_mask[channel]);
    fbytes[5]=DIO_value(hdw.FKHz_port[channel],fbytes[5],
	                    hdw.FKHz_mask[channel]);
  } 
  
  TSG_port=DIO_adr(hdw.TSG_port[channel]);
  TSG_safe=hdw.TSG_safe[channel]; /* safe interface box */
  TSG_mask=hdw.TSG_mask[channel];
  TSG_state=DIO_state(hdw.TSG_port[channel]) & ~TSG_mask;

  CLOCK_port=DIO_adr(hdw.CLOCK_port[channel]);
  CLOCK_mask=hdw.CLOCK_mask[channel]; 

  if (xt_flag & 0x01) {
     BEAM_port=DIO_adr(hdw.BEAM_port[channel]);
     BEAM_mask=hdw.BEAM_mask[channel];
     BEAM_state=DIO_state(hdw.BEAM_port[channel]) & ~BEAM_mask;
  }
    
  /* send the sequence here */
  if (watch_port !=0) kick_watchdog(watch_port,42);

  if (debug !=0) {
    int dval;
    dval=(int) tlen[seq_id]/100.0;
    if (dval==0) delay(1);
    else delay(dval);
    return 0;
  }  

  PUT_DIO(hdw.micro_clock_cw[channel],i8254_MODE_3 + 
							i8254_SC0 +
							i8254_RLL, 0xFF);
  PUT_DIO(hdw.micro_clock[channel], COUNT_10MICRO, 0xFF);

  if (xt_flag !=0) /* output the extended sequence */
    s=out_tsg_x(xtd[seq_id],code[seq_id],repeat[seq_id],
                tsg_length[seq_id],xt_flag,f,
		TSG_safe,TSG_port,TSG_mask,TSG_state,
                CLOCK_port,CLOCK_mask,BEAM_port,BEAM_mask,BEAM_state,
		fadr);
	       

  put_beam(channel,param.beam[channel]);

  if (xt_flag==0) /* output a regular sequence */ 
    s=out_tsg(code[seq_id],repeat[seq_id],
              tsg_length[seq_id],TSG_safe,TSG_port,TSG_mask,TSG_state,
	      CLOCK_port,CLOCK_mask);
  return s;
}


int send_seq_stereo(unsigned char *seq_id,unsigned int *tsg_delay,
		            unsigned char *xt_flag,int **ftable_ptr) {

  /* Send out a timing sequence */
  int s=0;
  unsigned char fbytes_A[6];
  unsigned char fbytes_B[6];

  int fadr_A[3],fadr_B[3];
  unsigned char *f_A=NULL;
  unsigned char *f_B=NULL;
 
  int TSG_port_A=0,TSG_safe_A=0,TSG_mask_A,TSG_state_A;
  int BEAM_port_A=0,BEAM_mask_A,BEAM_state_A=0;

  int TSG_port_B=0,TSG_safe_B=0,TSG_mask_B,TSG_state_B;
  int BEAM_port_B=0,BEAM_mask_B,BEAM_state_B=0;

  int CLOCK_port=0,CLOCK_mask=0;

  if (hdw.micro_clock[0]<0) return 0;
  if (hdw.TSG_port[0]<0) return 0;
  if (hdw.micro_clock[1]<0) return 0;
  if (hdw.TSG_port[1]<0) return 0;

  if (xt_flag !=NULL) {
    if ((xt_flag[0] & 0x02) && 
        ( (hdw.FMHz10_port[0]<0) ||
          (hdw.FMKHz_port[0]<0) ||
          (hdw.FKHz_port[0]<0) ) ) xt_flag[0]=xt_flag[0] & 0x01;
    if ((xt_flag[1] & 0x02) && 
        ( (hdw.FMHz10_port[1]<0) ||
          (hdw.FMKHz_port[1]<0) ||
          (hdw.FKHz_port[1]<0) ) ) xt_flag[1]=xt_flag[1] & 0x01;

    if ((xt_flag[0] & 0x01) && 
	   (hdw.BEAM_port[0]<0)) xt_flag[0]=xt_flag[0] & 0x02;     
    if ((xt_flag[1] & 0x01) && 
	   (hdw.BEAM_port[1]<0)) xt_flag[1]=xt_flag[0] & 0x02;     
  }

  if ((code[seq_id[0]] ==NULL) || (repeat[seq_id[0]] == NULL))  return -1;
  if ((code[seq_id[1]] ==NULL) || (repeat[seq_id[1]] == NULL))  return -1;

  if (xt_flag !=NULL) {
    if ((xt_flag[0] !=0) && (xtd[seq_id[0]]==NULL)) xt_flag[0]=0;
    if ((xt_flag[1] !=0) && (xtd[seq_id[1]]==NULL)) xt_flag[1]=0;
  } 

  if (xt_flag !=NULL) {
    if (((xt_flag[0] & 0x02) !=0) && ((ftable_ptr==NULL) || 
        (ftable_ptr[0]==NULL))) 
       xt_flag[0]=xt_flag[0] & 0x01; 
    if (((xt_flag[1] & 0x02) !=0) && ((ftable_ptr==NULL) || 
       (ftable_ptr[1]==NULL))) 
       xt_flag[1]=xt_flag[1] & 0x01; 

    if (xt_flag[0] & 0x02) {
      /* decode the frequency tables here */
      cnv_freq((ftable_ptr[0])[0],fbytes_A);
      cnv_freq((ftable_ptr[0])[1],fbytes_A+3);
      f_A=fbytes_A;
      fadr_A[2]=DIO_adr(hdw.FMHz10_port[0l]);
      fadr_A[1]=DIO_adr(hdw.FMKHz_port[0]);
      fadr_A[0]=DIO_adr(hdw.FKHz_port[0]); 
    
      /* set the bytes to reflect the current port values */
      fbytes_A[0]=DIO_value(hdw.FMHz10_port[0],fbytes_A[0],
   	                    hdw.FMHz10_mask[0]);
      fbytes_A[1]=DIO_value(hdw.FMKHz_port[0],fbytes_A[1],
	                      hdw.FMKHz_mask[0]);
      fbytes_A[2]=DIO_value(hdw.FKHz_port[0],fbytes_A[2],
	                    hdw.FKHz_mask[0]);
      fbytes_A[3]=DIO_value(hdw.FMHz10_port[0],fbytes_A[3],
	                    hdw.FMHz10_mask[0]);
      fbytes_A[4]=DIO_value(hdw.FMKHz_port[0],fbytes_A[4],
	                    hdw.FMKHz_mask[0]);
      fbytes_A[5]=DIO_value(hdw.FKHz_port[0],fbytes_A[5],
	                    hdw.FKHz_mask[0]);

    } 
    if (xt_flag[1] & 0x02) {
      /* decode the frequency tables here */
      cnv_freq((ftable_ptr[1])[0],fbytes_B);
      cnv_freq((ftable_ptr[1])[1],fbytes_B+3);
      f_B=fbytes_B;
      fadr_B[2]=DIO_adr(hdw.FMHz10_port[1l]);
      fadr_B[1]=DIO_adr(hdw.FMKHz_port[1]);
      fadr_B[0]=DIO_adr(hdw.FKHz_port[1]); 
    
      /* set the bytes to reflect the current port values */
      fbytes_B[0]=DIO_value(hdw.FMHz10_port[1],fbytes_B[0],
	                    hdw.FMHz10_mask[1]);
      fbytes_B[1]=DIO_value(hdw.FMKHz_port[1],fbytes_B[1],
	                    hdw.FMKHz_mask[1]);
      fbytes_B[2]=DIO_value(hdw.FKHz_port[1],fbytes_B[2],
	                    hdw.FKHz_mask[1]);
      fbytes_B[3]=DIO_value(hdw.FMHz10_port[1],fbytes_B[3],
	                    hdw.FMHz10_mask[1]);
      fbytes_B[4]=DIO_value(hdw.FMKHz_port[1],fbytes_B[4],
	                    hdw.FMKHz_mask[1]);
      fbytes_B[5]=DIO_value(hdw.FKHz_port[1],fbytes_B[5],
	                    hdw.FKHz_mask[1]);

    } 
  }

  TSG_port_A=DIO_adr(hdw.TSG_port[0]);
  TSG_safe_A=hdw.TSG_safe[0]; /* safe interface box */
  TSG_mask_A=hdw.TSG_mask[0]; 
  TSG_state_A=DIO_state(hdw.TSG_port[0]) & ~TSG_mask_A;

  TSG_port_B=DIO_adr(hdw.TSG_port[1]);
  TSG_safe_B=hdw.TSG_safe[1]; /* safe interface box */
  TSG_mask_B=hdw.TSG_mask[1]; 
  TSG_state_B=DIO_state(hdw.TSG_port[1]) & ~TSG_mask_B;



  CLOCK_port=DIO_adr(hdw.CLOCK_port[0]); /* clocks are cross connected */
  CLOCK_mask=hdw.CLOCK_mask[0]; 

  if (xt_flag !=NULL) {
    if (xt_flag[0] & 0x01) {
       BEAM_port_A=DIO_adr(hdw.BEAM_port[0]);
       BEAM_mask_A=hdw.BEAM_mask[0];
       BEAM_state_A=DIO_state(hdw.BEAM_port[0]) & ~BEAM_mask_A;
    }

    if (xt_flag[1] & 0x01) {
       BEAM_port_B=DIO_adr(hdw.BEAM_port[1]);
       BEAM_mask_B=hdw.BEAM_mask[1];
       BEAM_state_B=DIO_state(hdw.BEAM_port[1]) & ~BEAM_mask_B;
    }
  }


  /* send the sequence here */
  if (watch_port !=0) kick_watchdog(watch_port,42);

  if (debug !=0) {
    int dval;
    dval=(int) tlen[seq_id[0]]/100.0;
    if (dval==0) delay(1);
    else delay(dval);
    return 0;
  }  

  PUT_DIO(hdw.micro_clock_cw[0],i8254_MODE_3 + 
							i8254_SC0 +
							i8254_RLL, 0xFF);
  PUT_DIO(hdw.micro_clock[0], COUNT_10MICRO, 0xFF);
  PUT_DIO(hdw.micro_clock_cw[1],i8254_MODE_3 + 
							i8254_SC0 +
							i8254_RLL, 0xFF);
  PUT_DIO(hdw.micro_clock[1], COUNT_10MICRO, 0xFF);

  if ((xt_flag !=NULL) && /* output the extended sequence */
     ((xt_flag[0] !=0) || (xt_flag[1] !=0)))
       s=out_tsg_sx(xtd[seq_id[0]],xtd[seq_id[1]],
                    code[seq_id[0]],code[seq_id[1]],
                    repeat[seq_id[0]],repeat[seq_id[1]],
                    tsg_length[seq_id[0]],tsg_length[seq_id[1]],
                    tsg_delay[0],tsg_delay[1],
                    xt_flag[0],xt_flag[1],
                    f_A,f_B,
                    TSG_safe_A,TSG_safe_B,
                    TSG_port_A,TSG_port_B,
                    TSG_mask_A,TSG_mask_B,
                    TSG_state_A,TSG_state_B,
	            CLOCK_port,CLOCK_mask,
		    BEAM_port_A,BEAM_mask_A,BEAM_state_A,
		    BEAM_port_B,BEAM_mask_B,BEAM_state_B,
		    fadr_A,fadr_B);
  
  put_beam(0,param.beam[0]);
  put_beam(1,param.beam[1]);


 if ((xt_flag==NULL) || 
    ((xt_flag[0]==0) && (xt_flag[1]==0))) /* output a regular sequence */ 
    s=out_tsg_s(code[seq_id[0]],code[seq_id[1]],
              repeat[seq_id[0]],repeat[seq_id[1]],
              tsg_length[seq_id[0]],tsg_length[seq_id[1]],
              tsg_delay[0],tsg_delay[1],
              TSG_safe_A,TSG_safe_B,
              TSG_port_A,TSG_port_B,
              TSG_mask_A,TSG_mask_B,
              TSG_state_A,TSG_state_B,
	           CLOCK_port,CLOCK_mask);

  return s;
}

int send(pid_t pid) {

  /* output a timing sequence and get the status bytes at the end */
 
  int ftable[4];
  int *ftable_ptr[2];
  int status;
  char reply;

  struct {
    char stereo;
    unsigned char seq[2];
    unsigned int delay[2];
    unsigned char xt_flag[2];
    unsigned char st_flag[2];
    int f1,f2,f3,f4;
  } header;

  status=(Readmsg(pid,sizeof(char),&header,sizeof(header))!=sizeof(header));
  if (header.stereo==0) {
    status=(code[header.seq[0]] ==NULL) | (repeat[header.seq[0]] == NULL);
    if (status ==0) reply=DIO_OK;
    else reply=DIO_ERR;
    Reply(pid,&reply,sizeof(reply)); 

    if (status ==0) { 
   
      if (((header.xt_flag[0] & 0x02) !=0) &&
          (test_freq(fptr,header.f1)==0) && 
          (test_freq(fptr,header.f2)==0)) {
          ftable[0]=header.f1;
          ftable[1]=header.f2;
      }      

      status=send_seq_mono(param.channel,
                           header.seq[0],header.xt_flag[0],ftable);
      param.seq_id[param.channel]=header.seq[0];
    
      get_status(param.channel);
      if (param.antenna_mode[param.channel] == ANTENNA_AUTO) {
        param.antenna[param.channel] ++;
	    param.antenna[param.channel] =
	      (param.antenna[param.channel] > 15) ? 0 : 
		   param.antenna[param.channel];
	    put_antenna(param.channel,param.antenna[param.channel]);
      }
    }
  } else {
    status=(code[header.seq[0]] ==NULL) | (repeat[header.seq[0]] == NULL) |
           (code[header.seq[1]] ==NULL) | (repeat[header.seq[1]] == NULL);
    if (status ==0) reply=DIO_OK;
    else reply=DIO_ERR;
    Reply(pid,&reply,sizeof(reply)); 

    if (status ==0) { 
      if (((header.xt_flag[0] & 0x02) !=0) &&
          (test_freq(fptr,header.f1)==0) && 
          (test_freq(fptr,header.f2)==0)) {
        ftable[0]=header.f1;
        ftable[1]=header.f2;
        ftable_ptr[0]=ftable;
      }
      if (((header.xt_flag[1] & 0x02) !=0) &&
          (test_freq(fptr,header.f3)==0) && 
          (test_freq(fptr,header.f4)==0)) {
        ftable[2]=header.f3;
        ftable[3]=header.f4;
        ftable_ptr[1]=&ftable[2];
      }

      status=send_seq_stereo(header.seq,header.delay,
                             header.xt_flag,ftable_ptr);
      param.seq_id[0]=header.seq[0];
      param.seq_id[1]=header.seq[1];
      param.status=status;

      if (header.st_flag[0] !=0) get_status(0);
      if (header.st_flag[1] !=0) get_status(1);
      if (param.antenna_mode[0] == ANTENNA_AUTO) {
          param.antenna[0] ++;
	      param.antenna[0] =
	        (param.antenna[0] > 15) ? 0 : 
		     param.antenna[0];
	      put_antenna(0,param.antenna[0]);
      }
      if (param.antenna_mode[1] == ANTENNA_AUTO) {
          param.antenna[1] ++;
	      param.antenna[1] =
	        (param.antenna[1] > 15) ? 0 : 
		     param.antenna[1];
	      put_antenna(1,param.antenna[1]);
      }
    } 
  }
  return status;
}

 
