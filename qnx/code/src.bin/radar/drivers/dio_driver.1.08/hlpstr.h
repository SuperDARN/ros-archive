/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

char *hlpstr[]={
"dio_drive - DIO driver.\n\n",
"Usage: dio_drive [--help] [-db] [-s] [-c] [-d] [-wd wport]\n",
"                 [-p port,..] [-cf cfgname] [-ft frqname] name\n\n",
"--help        display the help message.\n",
"-db           run in debug mode - do not communicate with the hardware.\n",
"-s            operate in stereo mode.\n",
"-c            display the command ticker.\n",
"-d            print the configuration to standard output and exit.\n", 
"-wd wport     hardware port number of the watchdog timer.\n",
"-p  port,...  hardware port numbers of the DIO cards.\n",
"-cf cfgname   name of the hardware configuration file to load.\n",
"-ft frqname   name of the forbidden frequency table to load.\n",
"name          the name registered with the operating system by the task.\n",
   NULL};


