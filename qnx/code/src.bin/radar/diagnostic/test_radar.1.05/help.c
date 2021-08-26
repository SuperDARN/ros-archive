/* help.c
   ====== */

/*
 $Log: help.c,v $
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

extern int ad_flg;
extern int dio_flg;

extern char dio_name[256];
extern char a_d_name[256];

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
    if (dio_flg==1) fprintf(stdout,"Attached to task:%s\n",dio_name);
    if (ad_flg==1) {
      fprintf(stdout,"Attached to a_d_drive registered as:%s\n",a_d_name);
      fprintf(stdout,"There are %d buffers of %d bytes.\n",bufnum,bufsze);
    }
    fprintf(stdout,"\nCommands can be typed in full or the abbreviation (abr.)");
    fprintf(stdout," used.\nArguments can optionally be included on the");
    fprintf(stdout," comand line, missing arguments\nwill be prompted for.\n"); 
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

