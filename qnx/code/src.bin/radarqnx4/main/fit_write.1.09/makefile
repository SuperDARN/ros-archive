# Makefile for fit_write
# ======================
# Author: R.J.Barnes
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/superdarn -I$(IPATH)/radarqnx4
OBJS = fit_write.o
SRC=hlpstr.h errstr.h taskname.h fit_write.c
OUTPUT = $(BINPATH)/fit_write
LIBS=-loldfit.1 -ltaskid.1 -lrmsgrcv.1 -lfio.1 -lrtimer.1 -lradar.1 -lrtime.1 -lerrlog.1 -lopt.1 -lrcnv.1
SLIB=-lm
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

 

