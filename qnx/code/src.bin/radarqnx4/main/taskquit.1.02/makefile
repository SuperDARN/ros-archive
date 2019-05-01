# Makefile for taskquit
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/radar
OBJS = taskquit.o
SRC=hlpstr.h errstr.h taskquit.c
OUTPUT = $(BINPATH)/taskquit
LIBS=-ltaskid.1 -lrmsgsnd.1 -lrtimer.1 -lrtime.1 -lopt.1
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

  

 

