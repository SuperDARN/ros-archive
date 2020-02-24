/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

char *hlpstr[]={
  "tdisplay - Text console display.\n\n",
  "Usage: tdisplay [--help] [-v] [-p] [-w] [scl] [-ec echo]\n",
  "       tdisplay -a [--help] [rng] [-ec echo] \n",
  "       tdisplay -a -m [--help] [-ec echo] \n\n",

  "--help    display the help message.\n",
  "-ec echo  the name registered with the operating system by the\n",
  "          echo data task.\n",  
  "-v        display velocity data.\n",
  "-p        display lambda power data.\n",
  "-w        display lambda spectral width data.\n",
  "scl       upper limit of the color scale.\n\n",
  "-a        display the raw ACF data.\n",
  "rng       range number to plot.\n\n",
  "-m        plot the range with the maximum lag zero power.\n",
   NULL};


