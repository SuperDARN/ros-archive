/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/


char *hlpstr[]={
"rplot - An rPlot rendering engine.\n",
"rplot --help\n",
"rplot [options] [name]\n",
"rplot -ps [-land] [-xp xoff] [-yp yoff] [options] [name]\n",
"rplot -ppm [options] [name]\n",
"rplot -ppmx  [options] [name]\n",
"rplot -xml [options] [name]\n",
"rplot -x [options] [-display display] [-xoff xoff] [-yoff yoff] [name]\n",

"--help\tprint the help message and exit.\n",
"-ippm ppmfile\tname of a Portable PixMap (PPM) image file that will be used in the rPlot document. This option can be included on the command line multiple times to add more images\n",
"-ippmx ppmfiles\tname of an extended Portable PixMap (PPMX) image file that will be used in the rPlot document. This option can be included on the command line multiple times to add more images\n",
"-ixml ppmfiles\tname of an XML image representation file that will be used in the rPlot document. This option can be included on the command line multiple times to add more images\n",
"-fdb fonts\tread the font database from the XML document font. By default, the file specified by the environment variable FONTDB is used.\n",
"-ps\tproduce a PostScript plot as the output.\n",
"-land\tset the orientation of the PostScript plot to landscape.\n",
"-xp xoff\tset the X offset of the PostScript plot to xoff.\n",
"-yp xoff\tset the Y offset of the PostScript plot to yoff.\n",
"-ppm\tproduce a Portable PixMap (PPM) image as the output.\n",
"-ppmx\tproduce an extended Portable PixMap (PPMX) image as the output.\n",
"-xml\tproduce an XML image representation as the output.\n",
"-x\tplot on an X-terminal.\n",
"-display display\tconnect to the xterminal named display.\n",
"-xoff xoff\topen the window, xoff pixels from the left edge of the screen.\n",
"-yoff yoff\topen the window ypad pixels from the top edge of the screen.\n",
"name\tname of the rPlot document to plot, if this is omitted the document will be read from standard input.\n",

NULL};
