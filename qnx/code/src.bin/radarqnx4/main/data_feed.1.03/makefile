# Makefile for data_feed
# ======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn \
		-I$(IPATH)/radarqnx4
OBJS = data_feed.o
SRC=hlpstr.h errstr.h data_feed.c
OUTPUT = $(BINPATH)/data_feed
LIBS=-lrmsgsnd.1 -ltaskid.1 -lrtimer.1 -loldraw.1 -loldfit.1 -lradar.1 -lfitacf.1 -lraw.1 -lfit.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1
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

  

  

 

