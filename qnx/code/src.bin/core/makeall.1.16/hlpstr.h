/* hlpstr.h
   ========
   Author: R.J.Barnes
*/


char *hlpstr[]={
"makeall - Compiles multiple programs.\n",
"          The program searches a directory tree looking for source\n",
"          code directories, and compiles them.\n\n",
"Usage: makeall [--help]\n",
"       makeall [-vb] [-a] [-q] src\n",
"       makeall [-vb] [-a] [-q] src lst...\n",
"       makeall [-vb] [-a] [-q] src stdin\n\n",
"--help  display this help message and exit.\n",
"-vb     verbose. Log status to standard error in addition to standard out.\n",
"-a      compile all directories found, do not perform version checking.\n",
"-q      quiet mode. Keep going if an error occurs.\n",
"src     source directory in which to search for source code.\n\n",
"lst...  list of source code directories to compile.\n\n",
"stdin   read source code list from standard input.\n",
NULL};

