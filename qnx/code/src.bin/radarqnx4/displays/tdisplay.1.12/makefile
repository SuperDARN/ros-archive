# Makefile for tdisplay
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/radar
OBJS = main.o display.o
SRC=colours.h hlpstr.h display.c display.h errstr.h main.c taskname.h
OUTPUT = $(BINPATH)/tdisplay
LIBS= -lecho.1 -lrmsgrcv.1 -lradar.1 -ltermlib -lrtime.1 -lopt.1
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

  

