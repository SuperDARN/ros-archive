/* hlpstr.h
   ======== */


char *hlpstr[]={
   "make_smr - Create a summary file from a fit file.\n",
   "        Result is written to standard output.\n\n",
   "Usage: make_smr [--help] [-t pwr] [-b {blst}] fitfile\n\n",
   "--help\tgive this message\n",
   "-t pwr\treject ranges with lag zero power lower than pwr dB\n",  
   "-b {blst}\ta comma separated list of beams to include"
   " in the file\n",   
   "fitfile\tthe fit file to open\n",         
   NULL};
