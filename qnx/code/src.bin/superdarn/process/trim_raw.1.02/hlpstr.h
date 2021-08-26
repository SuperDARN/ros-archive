/* hlpstr.h
   ======== */


char *hlpstr[]={
   "trim_raw - Trims a section from a raw (dat) file.\n\n",
   "Usage: trim_raw [--help] [-st hr:mn] [-et hr:mn]  [-sd yyyymmdd]\n",
   "                 [-ed yyyymmdd] [-ex hr:mn] inrawfile outfitfile\n\n",
   "--help        display the help message.\n",
   "-vb           verbose.Log status to the standard error.\n",  
   "-st hr:mn     start time of the data period to process.\n",
   "-et hr:mn     end time of the data period to process.\n",
   "-sd yyyymmdd  start date of the data period to process.\n",
   "-ed yyyymmdd  end date of the data period to process.\n",
   "-ex hr:mn     extent or length of time to process.\n",
    "-t thr       threshold to apply to the lag-zero power,\n",
   "inrawfile     name of raw file to trim.\n",
   "outrawfile    name of the raw file to create.\n",
   NULL};
