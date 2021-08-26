# Makefile for echoraw
# ====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base  -I$(IPATH)/radarqnx4
OBJS = echoraw.o
SRC=hlpstr.h errstr.h echoraw.c taskname.h echotask.h
OUTPUT = $(BINPATH)/echoraw
LIBS=-ltaskid.1 -lrmsgrcv.1 -lrtimer.1 -lerrlog.1 -lad.1 -lopt.1
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

  

 

