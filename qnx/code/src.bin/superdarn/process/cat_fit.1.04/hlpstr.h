/* hlpstr.h
   ========
   Author: R.J.Barnes
*/


char *hlpstr[]={
   "cat_fit - Concatenates two or more fit files together, and generates\n",
   "           an index file.\n\n",
  "Usage: cat_fit [--help] infitfile... outfitfile\n",
  "       cat_fit -i [--help] infitfile... outfitfile outinxfile\n\n",
  "--help      display this help message.\n",
  "-i          generate index file.\n",
  "infitfile   list of fit files to concatenate.\n",
  "outfitfile  name of the fit file to create.\n",
  "outinxfile  name of the index file to create.\n",
   NULL};
