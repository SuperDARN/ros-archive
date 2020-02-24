/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

char *hlpstr[]={
  "server - Serves data over a TCP/IP connection.\n\n",
  "Usage: server [--help] [-e errlog] [-ec echo] [-n name] port\n\n",
  "--help     display the help message.\n",
  "-e errlog  the name registered with the operating system by the\n",
  "           error log task.\n",  
  "-ec echo   the name registered with the operating system by the\n",
  "           echo data task.\n",  
  "-n name    the name registered with the operating system be the task.\n",
  "-cn a/b    channel to accept data from.\n",
  "port       port number to serve the data from. If not specified a port\n",
  "           will be allocated by the operating system.\n",
  
   NULL};


