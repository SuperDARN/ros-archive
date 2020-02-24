/* hlpstr.h
   ========
   Author: R.J.Barnes
*/


char *hlpstr[]={
   "make_smr - Create a summary file from a fit file.\n",
   "           Result is written to standard output.\n\n",
   "Usage: make_smr [--help] [-t pwr] [-b bm,...] fitfile...\n\n",
   "--help       display the help message.\n",
   "-vb          verbose. Log status to the standard error.\n",
   "-t pwr       reject ranges with lag zero power lower than pwr dB.\n",  
   "-b bm,..     list of beams to include in the summary file."   
   "fitfile...   name of the fit file to process.\n",         
   NULL};
