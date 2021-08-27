/* hlpstr.h
   ======== */


char *hlpstr[]={
   "trim_fit - Trims a section from a fit file.\n\n",
   "Usage: trim_raw [--help] [-t threshold] [-st hr:mn] [-et hr:mn]\n",
   "                [-ex hr:mn] inrawfile outrawfile\n\n",
   "--help\tgive this message\n",
   "-t threshold\tApply this threshold to the lag zero power\n",
   "-st hr:mn\tstart time of the data period to store\n",
   "-et hr:mn\tend time of the data period to store\n",
   "-ex hr:mn\textent or length of time to store\n",
   "inrawfile\tname of raw file to trim\n",
   "outrawfile\tname of the raw file to create\n",
   NULL};
