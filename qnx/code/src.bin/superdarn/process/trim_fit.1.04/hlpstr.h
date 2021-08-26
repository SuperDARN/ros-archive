/* hlpstr.h
   ======== */


char *hlpstr[]={
   "trim_fit - Trims a section from a fit file.\n\n",
   "Usage: trim_fit [--help] [-st hr:mn] [-et hr:mn]  [-sd yyyymmdd]\n",
   "                 [-ed yyyymmdd] [-ex hr:mn] infitfile [ininxfile] \n",
   "                 outfitfile\n",
   "       trim_fit -i [--help] [-st hr:mn] [-et hr:mn] [-sd yyyymmdd]\n",
   "                [-ed yyyymmdd] [-ex hr:mn] infitfile [ininxfile] \n",
   "                outfitfile outinxfile\n\n",
   "--help        display the help message.\n",
   "-vb           verbose.Log status to the standard error.\n",  
   "-st hr:mn     start time of the data period to process.\n",
   "-et hr:mn     end time of the data period to process.\n",
   "-sd yyyymmdd  start date of the data period to process.\n",
   "-ed yyyymmdd  end date of the data period to process.\n",
   "-ex hr:mn     extent or length of time to process.\n",
   "infitfile     name of fit file to trim.\n",
   "ininxfile     name of index file associated with the input fit file.\n",
   "outfitfile    name of the fit file to create.\n",
   "outinxfile    name of the index file to create.\n",
   NULL};
