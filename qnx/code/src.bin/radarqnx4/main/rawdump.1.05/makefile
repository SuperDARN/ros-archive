# Makefile for monitor
# ====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/radarqnx4
OBJS = rawdump.o
SRC=hlpstr.h errstr.h taskname.h rawdump.c
OUTPUT = $(BINPATH)/rawdump
LIBS=-lrtimer.1 -lechoraw.1 -lad.1 -lrtime.1 -ldmap.1 -lrcnv.1 -lopt.1
SUDO = 1
ifeq ($(OSTYPE),qnx4)
include $(MAKEBIN)

else
version: version.info $(SRC)
	$(BINPATH)/rcsv -t version.info
	$(BINPATH)/rcsv -h version.info > version.h
clean:
	rm -f *.o
	rm -f $(OUTPUT)

endif

  

 

