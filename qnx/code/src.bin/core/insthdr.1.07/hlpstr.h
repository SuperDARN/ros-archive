/* hlpstr.h
   ========
   Author: R.J.Barnes
*/


char *hlpstr[]={
"insthdr - Install symbolic links to library headers.\n",
"          The program creates symbolic links to the headers of\n",
"          A library in the specified directory.\n\n",
"Usage: insthdr [--help] [-vb] [-a] [-q] src dst\n",
"       insthdr [--help] [-vb] [-a] [-q] src dst lst...\n",
"       insthdr [--help] [-vb] [-a] [-q] src dst stdin\n\n",
"--help  display this help message and exit.\n",
"-vb     verbose. Log status to standard error in addition to standard out.\n",
"-a      link all libraries found, do not perform version checking.\n",
"-q      quiet mode. Keep going if an error occurs.\n",
"src     source directory in which to search for libraries.\n",
"dst     destination directory in which to create links.\n\n",
"lst...  list of library subdirectories to create links for.\n\n",
"stdin   read library list from standard input.\n",
NULL};

