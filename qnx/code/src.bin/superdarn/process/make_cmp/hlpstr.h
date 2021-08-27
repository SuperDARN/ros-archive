/* hlpstr.h
   ======== */


char *hlpstr[]={
   "make_cmp - Create a Color Map File (CMP) from a fit file.\n",
   "        Result is written to standard output.\n\n",
   "Usage: make_cmp [--help] [-vb] [-a] [-p max] [-v max] [-w max] [-t pwr]\n",
   "                [-b bm] fitfile\n\n",
   "--help\tgive this message\n",
   "-vb\tverbose. Log status to the standard error\n",
   "-a\trecord all three parameters in the fils\n", 
   "-p\trecord back scattered power in the file. The upper limit is max\n"
   "-v\trecord velocity in the file. The upper limit is max\n"
   "-w\trecord spectral width in the file. The upper limit is max\n"
   "-t pwr\treject ranges with lag zero power lower than pwr dB\n"
   "-b bm\trecord only information about the beam specified by bm\n", 
   "fitfile\tfit file to convert\n",            
   NULL};
