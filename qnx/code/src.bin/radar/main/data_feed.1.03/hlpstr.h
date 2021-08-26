/* hlpstr.h
   ======== */


char *hlpstr[]={
   "data_feed - Read data from a file and send it to another task.\n\n",
    "Usage: data_feed [--help] [-d] [-o hr:mn] [-b sec] [-s st]\n",
    "                 [-i sec] [-a] [-r] task... file\n\n",
    "--help    display the help message.\n",
    "-d        input file is a dat (raw) file.\n",
    "-o hr:mn  length of time to wait between sending open and close file\n",
    "          messages. By default no messages are sent.\n",
    "-b sec    synchronize data with a boundary of this number of seconds.\n",
    "-s st     replace radar id with that from radar with this station id\n",
    "          letter.\n",
    "-i sec    override the integration time in the file with this value.\n",
    "-a        substitute actual time for the time in the file.\n",
    "-r        loop continuously, rewinding the file when it is exhausted.\n",
    "task...   list of names registered with the operating system of the\n",
    "          tasks that the data is to be sent to.\n",
    "file      name of the data file to serve.\n",
   NULL};
