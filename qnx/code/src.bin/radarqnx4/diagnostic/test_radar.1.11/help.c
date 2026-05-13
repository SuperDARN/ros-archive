/* help.c
   ======
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
 $Log: help.c,v $
 Revision 1.9  2004/07/06 13:28:52  barnes
 Modified log.

 Revision 1.8  2004/06/21 23:21:22  barnes
 QNX compile audit.

 Revision 1.7  2004/05/10 14:06:10  barnes
 Modification to deal with new libraries.

 Revision 1.6  2001/06/27 20:54:47  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:12:04  barnes
 Added Author Name

 Revision 1.4  2000/11/13 20:25:37  barnes
 Added missing help message for fstatus.

 Revision 1.3  1998/11/09 16:28:38  barnes
 Added the ability to query the a specific command with help.

 Revision 1.2  1998/11/08 17:22:17  barnes
 Added code to support stereo cutlass and modified the calling sequence
 for send_tsg.

 Revision 1.1  1998/10/27 19:15:30  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

extern int adflg;
extern int dioflg;

extern char dioname[256];
extern char adname[256];

extern int bufsze;
extern int bufnum;

struct {
  char *name;
  char *abbrev;
  char *opt;
} help[]={ 
  {"quit     ","q",""},
  {"help     ","h",""},
  {"reset    ","r",""},
  {"beam     ","b","[beam no.]"},
  {"freq     ","f","[frequency]"},
  {"gain     ","g","[attenuation]"},
  {"antenna  ","a","[antenna] [beam/auto/fix]"},
  {"test     ","t","[on/off]"},
  {"verify   ","v","[id]"},
  {"status   ","st","[clear/leave] [full/partial]"},
  {"filter   ","fi","[filter]"},
  {"fclr     ","c","[id] [frequencies...]"},
  {"tsg      ","s",
"[stereo] [id] [repeat] [stat/normal] [beam/normal] [freq/normal] [freq.].."},
  {"load     ","l","[id] [filename]"},
  {"dump     ","d","[buffer] [bytes] [filename]"},
  {"proxy    ","p",""},
  {"ad_status","ad",""},
  {"transfer ","tr","[buffer] [bytes] [channels] [hardware/software]"},
  {"channel  ","cn","[channel]"},
  {"fstatus  ","fs",""},
  0
};


void print_help(char *arg) {
  int i;
  if (arg==NULL) {
    fprintf(stdout,"test_radar\n"),
    fprintf(stdout,"==========\n\n");
    if (dioflg==1) fprintf(stdout,"Attached to task:%s\n",dioname);
    if (adflg==1) {
      fprintf(stdout,"Attached to dt2828 registered as:%s\n",adname);
      fprintf(stdout,"There are %d buffers of %d bytes.\n",bufnum,bufsze);
    }
    fprintf(stdout,
      "\nCommands can be typed in full or the abbreviation (abr.)");
    fprintf(stdout," used.\nArguments can optionally be included on the");
    fprintf(stdout,
      " comand line, missing arguments\nwill be prompted for.\n"); 
    fprintf(stdout,"Commands can be aborted by typing '.'\n");
    fprintf(stdout,"\nCommand   \t(Abr.)\tArguments\n");
    for (i=0;help[i].name !=NULL;i++) {
      fprintf(stdout,"%s\t(%s) \t%s\n",help[i].name,
                       help[i].abbrev,help[i].opt);
      if ((i>0) && ((i % 8)==0)) {
        fprintf(stdout,"More...\n");
        fflush(stdout);
        getch();
      }
    }
  } else {
    for (i=0;help[i].name !=NULL;i++) {
      if ((strcmp(help[i].name,arg) ==0) || (strcmp(help[i].abbrev,arg)==0))
      fprintf(stdout,"%s\t(%s) \t%s\n",help[i].name,help[i].abbrev,
			 help[i].opt);
    }
  }
}

