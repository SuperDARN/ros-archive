/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

char *hlpstr[]={
  "compress - Create summary files in Color Map File (CMP) format.\n\n",
  "Usage: compress [--help] [-a] [-p max] [-v max] [-w max] [-t pwr]\n",
  "                [-e errlog] [-ec echo] \n\n",
  "--help     display the help message.\n",
  "-a         record all three parameters in the file.\n", 
  "-p max     record back scattered power in the file. The upper limit\n",
  "           is max.\n"
  "-v max     record velocity in the file. The upper limit on the\n",
  "           velocity magnitude is max.\n"
  "-w max     record spectral width in the file. The upper limit is max.\n"
  "-t pwr     reject ranges with lag zero power lower than pwr dB\n"
  "-e errlog  the name registered with the operating system by the\n",
  "           error log task.\n",  
  "-ec echo   the name registered with the operating system by the\n",
  "           echo data task.\n",  
   NULL};


