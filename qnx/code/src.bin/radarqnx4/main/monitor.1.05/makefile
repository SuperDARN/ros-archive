# Makefile for monitor
# ====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base  -I$(IPATH)/radarqnx4
OBJS = monitor.o connect.o msgmem.o socket.o fork.o 
SRC=hlpstr.h errstr.h connect.c connect.h fork.c fork.h taskname.h msgmem.c msgmem.h monitor.c socket.c socket.h
OUTPUT = $(BINPATH)/monitor
LIBS=-lrtimer.1 -lgc214.1 -lcnx.1 -lopt.1
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

  

 

