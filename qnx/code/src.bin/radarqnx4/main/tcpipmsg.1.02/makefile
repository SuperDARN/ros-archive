# Makefile for tcpipmsg
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base  -I$(IPATH)/radarqnx4
OBJS = tcpipmsg.o connect.o msgmem.o socket.o fork.o 
SRC=hlpstr.h errstr.h connect.c connect.h fork.c fork.h taskname.h tcpipmsg.c msgmem.c msgmem.h socket.c socket.h
OUTPUT = $(BINPATH)/tcpipmsg
LIBS=-lcnx.1 -lopt.1
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

 

