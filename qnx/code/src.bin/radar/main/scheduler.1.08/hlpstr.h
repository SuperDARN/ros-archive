/* hlpstr.h
   ========
   Author: R.J.Barnes
*/


char *hlpstr[]={
  "schedule - schedules control programs.\n\n",
  "Usage: schedule [--help] [-d] [-h] [-t] [-m] [-l log] [-n name]\n",
  "                schedule\n\n",
  "--help    display the help message.\n",
  "-d        reload the schedule every day.\n",
  "-h        reload the schedule every hour.\n",
  "-t        reload the schedule every 10 minutes.\n",
  "-m        reload the schedule every minute.\n",
  "-l log    the name of the schedule log file. The log is stored in the\n",
  "          file log.XXX, where XXX is the day of the year.\n",
  "-n name   the name registered with the operating system by the task.\n",
  "schedule  name of the schedule file to load.\n",



   NULL};
