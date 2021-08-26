# Makefile for rtserver
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general  -I$(IPATH)/superdarn \
	-I$(IPATH)/radarqnx4
OBJS = fitpacket.o connect.o msgmem.o socket.o fork.o rtserver.o
SRC=hlpstr.h errstr.h rtserver.c connect.c connect.h fork.c fork.h taskname.h fitpacket.c fitpacket.h msgmem.c msgmem.h socket.c socket.h
OUTPUT = $(BINPATH)/rtserver 
LIBS=-lfit.1 -lecho.1 -ltaskid.1 -lrmsgrcv.1 -lfio.1 -lrtimer.1 -lradar.1 -lrtime.1 -lerrlog.1 -lcnx.1 -ldmap.1 -lopt.1 -lrcnv.1
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

  

