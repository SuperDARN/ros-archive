/* hlpstr.h
   ========
   Author: R.J.Barnes
*/


char *hlpstr[]={
 "meteor_proc - Meteor winds processing software.\n",
 "              Result is written to standard output.\n",
 "Usage: meteor_proc [--help] [-vb] [-hr hr] [-mv maxvel] [-mw maxwidth]\n",
 "                   [-ms minsn] [-me maxerr]  [-mr maxrange]\n",
 "                   [-bm vmbeam] [-mb minbeam] [-mz meridional/zonal]\n",
 "                   file....\n\n",
 "--help           display the help message.\n",
 "-vb              verbose. Log status to the standard error.\n", 
 "-hr hr           hour to process.\n",
 "-mv maxvel       maximum velocity threshold\n",
 "-mw maxwidth     maximum spectral width threshold\n",
 "-ms minsn        minimum signal to noise ratio (power)\n",
 "-me maxerr       maximum velocity error threshold\n",
 "-mr maxrange     maximum range in kilometers threshold.\n",
 "-bm vmbeam       beam number for the meridional wind veclocity\n",
 "-mb minbeam      minimum number of beams to accept during fitting\n",
 "-mz meridional/  wind component to record meridional or zonal\n",
 "   zonal\n",       
 "file...       list of fit files to convert.\n",           
 NULL};
