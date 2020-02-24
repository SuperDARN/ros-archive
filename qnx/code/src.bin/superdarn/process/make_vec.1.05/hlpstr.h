/* hlpstr.h
   ========
   Author: R.J.Barnes
*/


char *hlpstr[]={
   "make_vec - Creates a vec file from a grid file.\n",
   "           Result is written to standard output.\n\n",
   "Usage: make_vec [--help] [-vb] [-{st}] [file]\n\n",
   "--help  display the help message.\n",
   "-vb     verbose.Log status to the standard error.\n", 
   "-{st}   generate file for the radar with station id letter {st}.\n",  
   "file    file to convert. If none given, use standard input.\n",            
   NULL};
