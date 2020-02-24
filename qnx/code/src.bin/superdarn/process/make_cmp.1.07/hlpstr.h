/* hlpstr.h
   ========
   Author: R.J.Barnes
*/


char *hlpstr[]={
  "make_cmp - Create a Color Map File (CMP) from a fit file.\n",
  "        Result is written to standard output.\n\n",
  "Usage: make_cmp [--help] [-vb] [-a] [-p max] [-v max] [-w max] [-t pwr]\n",
  "                [-b bm] fitfile...\n\n",
  "--help       display the help message.\n",
  "-vb          verbose. Log status to the standard error.\n",
  "-a           record all three parameters in the file.\n", 
  "-p max       record back scattered power in the file. The upper limit\n",
  "             is max.\n"
  "-v max       record velocity in the file. The upper limit on the\n",
  "             velocity magnitude is max.\n"
  "-w max       record spectral width in the file. The upper limit is max.\n"
  "-t pwr       reject ranges with lag zero power lower than pwr dB\n"
  "-b bm        record only information about the beam specified by bm.\n", 
  "fitfile...   list of fit files to convert\n",            
   NULL};
