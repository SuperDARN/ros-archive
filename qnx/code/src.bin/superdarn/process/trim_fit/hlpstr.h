/* hlpstr.h
   ======== */


char *hlpstr[]={
   "trim_fit - Trims a section from a fit file.\n\n",
   "Usage: trim_fit [--help] [-st hr:mn] [-et hr:mn]\n",
   "                [-ex hr:mn] infitfile [ininxfile] outfitfile\n",
   "       trim_fit [--help] -i [-st hr:mn] [-et hr:mn]\n",
   "                [-ex hr:mn] infitfile [ininxfile] outfitfile outinxfile\n",
   "\n--help\tgive this message\n",
   "-i\tgenerate index file as well\n",
   "-st hr:mn\tstart time of the data period to store\n",
   "-et hr:mn\tend time of the data period to store\n",
   "-ex hr:mn\textent or length of time to store\n",
   "infitfile\tname of fit file to trim\n",
   "ininxfile\tname of index file associated with the input fit file\n",
   "outfitfile\tname of the fit file to create\n",
   "outinxfile\tname of the index file to create\n",
   NULL};
