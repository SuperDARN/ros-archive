/* hlpstr.h
   ======== */

char *hlpstr[]={
"gridfit - Create summary grid files.\n\n",
"Usage: gridfit [--help] [-fwgt wgt] [-pmax max] [-pmin min] [-vmax max]\n",
"               [-vmin min] [-wmax max] [-wmin min] [-vemax max]\n",
"               [-vemin min] [-gs] [-ion] [-both] [-nav] [-nlm] [-nb]\n",
"               [-xtd] [-inertial] [-e errlog] [-ec echo]\n\n",
"--help      display the help message.\n",
"-fwgt wgt   adjust weighting factor of the median filter, 0 = no filter.\n",
"-pmax max   maximum threshold for the lag zero power.\n",
"-pmin min   minimum threshold for the lag zero power.\n",
"-vmax max   maximum threshold for the velocity.\n",
"-vmin min   minumum threshold for the velocity.\n",
"-wmax max   maximum threshold for the spectral width.\n",
"-wmin min   minumum threshold for the spectral width.\n",
"-vemax max  maximum threshold for the velocity error.\n",
"-vemin min  minumum threshold for the velocity error.\n",
"-gs         store only vectors assumed to be ground scatter.\n",
"-ion        store only vectors assumed to by ionospheric scatter.\n",
"-both       store both ionospheric and ground scatter vectors.\n",
"-nav        do not perform temporal averaging.\n",
"-nlm        do not apply limits to changes in the radar parameters\n",
"            between scans.\n",
"-nb         do not apply bounding thresholds to the data.\n",
"-xtd        record extended information in the files.\n",
"-intertial    operate in an intertial reference frame.\n",

"-e errlog   the name registered with the operating system by the\n",
"            error log task.\n",  
"-ec echo    the name registered with the operating system by the\n",
"            echo data task.\n",  
   NULL};





