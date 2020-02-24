/* hlpstr.h
   ========
   Author: R.J.Barnes
*/


char *hlpstr[]={
"makedst - Build a distribution tree from a source code archive.\n",
"          The program searches a directory tree and builds a copy of it.\n",
"          If a source code directory managed under RCS is discovered,\n",
"          the program will check out the most recent version of the code\n",
"          into the destination directory.\n\n",
"Usage: makedst [--help] [-vb] [-p post] src dst lst...\n",
"       makedst [--help] [-vb] [-p post] src dst stdin\n\n",
"--help   display this help message and exit.\n",
"-vb      verbose. Log status to standard error in addition to standard\n",
"         out.\n",
"-p post  post process the source code with the command given by post.\n",
"src      source directory in which to search.\n",
"dst      destination directory in which to create distribution.\n",
"lst...   list of subdirectories to include in the distribution.\n\n",
"stdin    read the source list from standard input.\n",
NULL};

