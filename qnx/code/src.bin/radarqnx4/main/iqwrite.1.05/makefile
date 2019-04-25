# Makefile for iqwrite
# ====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/superdarn -I$(IPATH)/radarqnx4
OBJS = iqwrite.o
SRC=iqwrite.c hlpstr.h errstr.h taskname.h
OUTPUT = $(BINPATH)/iqwrite
SUDO=1
LIBS=-liqdata.1 -ltaskid.1 -lshmem.1 -lrmsgrcv.1 -lfio.1 -lrtimer.1 -lradar.1 -lrtime.1 -lerrlog.1 -ldmap.1 -lopt.1 -lrcnv.1
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

  

