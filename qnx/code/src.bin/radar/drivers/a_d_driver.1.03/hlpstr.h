/* hlpstr.h
   ======== */

char *hlpstr[]={
"a_d_drive - A/D driver.\n\n",
"Usage: a_d_drive [--help] [-db] [-i irq] [-d dma] [-p port]\n",
"                 [-n bufnum] name\n\n",
"--help     display the help message.\n",
"-db        run in debug mode - do not communicate with the hardware.\n",
"-i irq     interrupt number (default=15).\n",
"-d dma     DMA channel (default=5).\n",
"-p port    IO port number in hexadecimal (default=240).\n", 
"-n bufnum  number of memory buffers to create (default=2).\n",
"name       the name registered with the operating system by the task.\n",
   NULL};


