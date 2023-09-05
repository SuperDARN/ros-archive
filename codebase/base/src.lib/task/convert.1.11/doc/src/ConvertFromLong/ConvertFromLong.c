/* ConvertFromLong.c
   =================
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:



#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

int main(int argc,char *argv[]) {
  int i;
  int64 val;
  unsigned char buf[8];
  
  val=1023;
  ConvertFromLong(val,buf);
  
  fprintf(stdout,"val=%lld\n",val);
  fprintf(stdout,"buf=");
  for (i=0;i<8;i++) fprintf(stdout,"%.2x",buf[i]);
  fprintf(stdout,"\n");
  return 0;
}
   

